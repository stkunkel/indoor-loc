/*
 * mpu_int.c: Interrupt-based readout of MPU
 * Author: Stephanie Kunkel
 * Sources:	http://www.xilinx.com/support/documentation/xcell_articles/how-to-use-interrupts-on-zynqsoc.pdf
 * 			https://forums.xilinx.com/t5/Xcell-Daily-Blog/More-About-MicroZed-Interrupts-Adam-Taylor-s-MicroZed-Chronicles/ba-p/398357
 */

/*
 * Includes
 */
#include "mpu_int.h"

/*
 * Global Variables
 */
static XScuGic Intc; // Interrupt Controller Driver
static XGpioPs Gpio; //GPIO Device
u32 lastIntVal = 0;

/*
 * Functions
 */

/*
 * Wait for Interrupt
 * Returns 1 when interrupt occured (rising edge on interrupt pin)
 */
int waitForInterrupt() {
	//Variables
	u32 data;

	//Loop
	while (1) {
		//Read from interrupt pin
		readInt(&data);

		//Rising edge?
		if (lastIntVal == 0 && data == 1) {
			lastIntVal = data;
			return 1;
		} else {
			lastIntVal = data;
		}
	}
}

/*
 * Set Up Interrupt for IMU
 */
int setupMPUInt() {
	//Variables
	int status;
	XScuGic_Config* IntcConfig;
	XGpioPs_Config* GpioConfig;

	//MPU
	//Disable all interrupts
	unsigned char* data = 0;
	imuI2cWrite(0x68, 0x38, 2, data);

	//Set Interrupt level
	status = mpu_set_int_level(0); //Set Interrupt for "active high" (0)
	if (status != XST_SUCCESS) {
		myprintf("mpu_int.c: Could not set interrupt level.\r\n");
		return XST_FAILURE;
	}

	//Set Interrupt Mode
	status = dmp_set_interrupt_mode(DMP_INT_CONTINUOUS); //Interrupt when one FIFO period has elapsed
	if (status != XST_SUCCESS) {
		myprintf("mpu_int.c: Could not set interrupt mode.\r\n");
		return XST_FAILURE;
	}

	//Initialize Xil Exceptions
	Xil_ExceptionInit();

	//Initialize GPIO
	GpioConfig = XGpioPs_LookupConfig(GPIO_DEVICE_ID);
	status = XGpioPs_CfgInitialize(&Gpio, GpioConfig, GpioConfig->BaseAddr);
	if (status != XST_SUCCESS) {
		myprintf("mpu_utils.c: Error initializing GPIO Config.\r\n");
		return XST_FAILURE;
	}

	//Initialize GIC
	IntcConfig = XScuGic_LookupConfig(INTC_DEVICE_ID);
	status = XScuGic_CfgInitialize(&Intc, IntcConfig,
			IntcConfig->CpuBaseAddress);
	if (status != XST_SUCCESS) {
		myprintf("mpu_utils.c: Error initializing SCU GIC Config.\r\n");
		return XST_FAILURE;
	}

	//Disable Interrupts from GPIO
	//XScuGic_Disable(&Intc, GPIO_INT_ID);

	//Connect interrupt controller interrupt handler to HW interrupt handling logic in PS
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
			(Xil_ExceptionHandler) XScuGic_InterruptHandler, &Intc);

	//Connect driver handler (GIC) called when interrupt occurs to HW defined above
	XScuGic_Connect(&Intc, GPIO_INT_ID,
			(Xil_ExceptionHandler) XGpioPs_IntrHandler, (void*) &Gpio);

	//Enable Interrupt for Pin
	XGpioPs_SetIntrTypePin(&Gpio, GPIO_INT_PIN, XGPIOPS_IRQ_TYPE_EDGE_RISING);

	//Set Callback Handler for GPIO Interrupts
	XGpioPs_SetCallbackHandler(&Gpio, (void *) &Gpio,
			(XGpioPs_Handler) IntrHandler);

	//Enable GPIO Interrupt for Pin
	XGpioPs_IntrEnablePin(&Gpio, GPIO_INT_PIN);
	if (!XGpioPs_IntrGetEnabledPin(&Gpio, GPIO_INT_PIN)) {
		myprintf("mpu_int.c: Interrupt not enabled.\r\n");
	}

	//Enable Interrupts for GPIO
	XScuGic_Enable(&Intc, GPIO_INT_ID);

	//Enable Interrupts in Processor
	//Xil_ExceptionEnable();
	Xil_ExceptionEnableMask(XIL_EXCEPTION_IRQ);

	//Return
	return XST_SUCCESS;
}

/*
 * Interrupt Handler for IMU
 * Updates internal copy of sensor data.
 * In: Call-back reference, bank, status
 */
void IntrHandler(void *CallBackRef, u32 Bank, u32 Status) {
	//Debug
	short irq;
	mpu_get_int_status(&irq);
	myprintf("MPU IRQ Status: 0x%x\r\n", irq);
	usleep(100000);

	//Variables
	XGpioPs *GpioInt = (XGpioPs *) CallBackRef;
	int status;

	//Disable Interrupts
	XGpioPs_IntrDisablePin(GpioInt, GPIO_INT_PIN);
	//XGpioPs_IntrDisable(GpioInt, Bank, XIL_EXCEPTION_IRQ);

	//Check whether interrupt came from INT pin of IMU
	if (XGpioPs_IntrGetStatusPin(GpioInt, GPIO_INT_PIN)) {
		//Update Sensor Data
		updateData();

		//Print Data
		status = printforDisplay(1, 1);
		if (status == XST_SUCCESS) {
			printf("\r\n");
		}
	}

	//Clear Interrupt Pin
	XGpioPs_IntrClearPin(GpioInt, GPIO_INT_PIN);

	//Enable Interrupts
	XGpioPs_IntrEnablePin(GpioInt, GPIO_INT_PIN);
	//XGpioPs_IntrEnable(GpioInt, Bank, XIL_EXCEPTION_IRQ);
}

