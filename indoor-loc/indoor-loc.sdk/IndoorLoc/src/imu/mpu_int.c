/*
 * mpu_int.c: Interrupt-based readout of MPU
 * Author: Stephanie Kunkel
 * Source: http://www.xilinx.com/support/documentation/xcell_articles/how-to-use-interrupts-on-zynqsoc.pdf
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
	XScuGic_Disable(&Intc, IRQ_ID_GPIO);

	//Connect to HW
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
			(Xil_ExceptionHandler) XScuGic_InterruptHandler, &Intc);

	//Set Interrupt Type
	XGpioPs_SetIntrTypePin(&Gpio, GPIO_INT_PIN, XGPIOPS_IRQ_TYPE_EDGE_RISING);

	//Set Callback Handler
	XGpioPs_SetCallbackHandler(&Gpio, (void *) &Gpio,
			(XGpioPs_Handler) IntrHandler);

	//Enable GPIO Interrupt
	XGpioPs_IntrEnablePin(&Gpio, GPIO_INT_PIN);
	if (!XGpioPs_IntrGetEnabledPin(&Gpio, GPIO_INT_PIN)) {
		myprintf("mpu_int.c: Interrupt not enabled.\r\n");
	}

	//Connect GPIO Interrupt Handler with GIC
	XScuGic_Connect(&Intc, IRQ_ID_GPIO,
			(Xil_ExceptionHandler) XGpioPs_IntrHandler, (void*) &Gpio);

	//Enable Interrupts from GPIO
	XScuGic_Enable(&Intc, IRQ_ID_GPIO);

	//Enable Xil Exceptions
	Xil_ExceptionEnable();

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
	myprintf("MPU IRQ Status: %x\r\n", irq);

	//Variables
	XGpioPs *GpioInt = (XGpioPs *) CallBackRef;
	int status;

	//Disable Interrupts
	XGpioPs_IntrDisablePin(GpioInt, GPIO_INT_PIN);

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
}

