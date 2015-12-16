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
static volatile char dataAvailable = 0;

/*
 * Function Prototypes
 */
void ImuIntrHandler(void *CallBackRef, u32 Bank, u32 Status);
void myXGpioPs_IntrHandler(XGpioPs *InstancePtr);

/*
 * Check if data from IMU FIFO is available
 * Returns 1 if there is new data, 0 if not
 */
char imuDataAvailable() {
	if (dataAvailable) {
		dataAvailable = 0;
		return 1;
	} else {
		return 0;
	}
}

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
	unsigned char data;

	//MPU
	//Disable all interrupts
//	data = 0x15;
//	imuI2cWrite(0x68, 0x38, 2, &data);

	//Configure MPU Interrupt Pin
	status = imuI2cRead(0x68, 0x37, 2, &data);	//Read Register
	if (status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	data &= (~INT_LEVEL_BIT); //Clear bit
	data &= (~INT_OPEN_BIT); //Clear bit
	data &= (~INT_RD_CLEAR_BIT); //Clear bit
	data |= LATCH_INT_EN_BIT; //Set bit
	//data = 0x24; //debug
	do {
		status = imuI2cWrite(0x68, 0x37, 1, &data); //Write Register
	} while (status != XST_SUCCESS);

	//Set Interrupt level
	status = mpu_set_int_level(0); //Set Interrupt for "active high" (0)
	if (status != XST_SUCCESS) {
		myprintf("mpu_int.c: Could not set interrupt level.\r\n");
		return XST_FAILURE;
	}

	//Enable Latched Interrupt
	status = mpu_set_int_latched(1);
	if (status != XST_SUCCESS) {
		myprintf("mpu_int.c: Could not set latched interrupt.\r\n");
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

	//Connect interrupt controller interrupt handler to HW interrupt handling logic in PS
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
			(Xil_ExceptionHandler) XScuGic_InterruptHandler, &Intc);

	//Connect driver handler (GIC) called when interrupt occurs to HW defined above
	XScuGic_Connect(&Intc, GPIO_INT_ID,
			(Xil_ExceptionHandler) myXGpioPs_IntrHandler, (void*) &Gpio); //Use my own Intr Handler

	//Enable Interrupt for Pin
	XGpioPs_SetIntrTypePin(&Gpio, GPIO_INT_PIN, XGPIOPS_IRQ_TYPE_EDGE_RISING);

	//Set Callback Handler for GPIO Interrupts
	XGpioPs_SetCallbackHandler(&Gpio, (void *) &Gpio,
			(XGpioPs_Handler) ImuIntrHandler);

	//Enable GPIO Interrupt for Pin
	XGpioPs_IntrEnablePin(&Gpio, GPIO_INT_PIN);
	if (!XGpioPs_IntrGetEnabledPin(&Gpio, GPIO_INT_PIN)) {
		myprintf("mpu_int.c: Interrupt not enabled.\r\n");
	}

	//Enable Interrupts for GPIO
	XScuGic_Enable(&Intc, GPIO_INT_ID);

	//Enable Interrupts in Processor
	Xil_ExceptionEnableMask(XIL_EXCEPTION_IRQ);

	//Free memory and return
	return XST_SUCCESS;
}

/*
 * Interrupt Handler for IMU
 * Updates internal copy of sensor data.
 * In: Call-back reference, bank, status
 */
void ImuIntrHandler(void *CallBackRef, u32 Bank, u32 Status) {
	//Variables
	XGpioPs *GpioInt = (XGpioPs *) CallBackRef;
	int status = XST_SUCCESS;
	short irq;

	//Disable Interrupts
//	XGpioPs_IntrDisablePin(GpioInt, GPIO_INT_PIN);

	//Get IRQ Status
	status = mpu_get_int_status(&irq);
	if ((status == XST_SUCCESS)
			&& ((irq & MPU_INT_STATUS_DMP_0) == MPU_INT_STATUS_DMP_0)) { //INT from DMP
		dataAvailable = 1;
	}
//
//	//Enable Interrupts
//	XGpioPs_IntrEnablePin(GpioInt, GPIO_INT_PIN);

}

/*
 * XGpioPS Interrupt Handler
 * Compared to the original Interrupt Handler, this one only handles an interrupt if it has been enabled.
 * In: Instance Pointer
 */
void myXGpioPs_IntrHandler(XGpioPs *InstancePtr) {
	u8 Bank;
	u32 IntrStatus;
	u32 IntrEnabled;

	Xil_AssertVoid(InstancePtr != NULL);
	Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

	for (Bank = 0U; Bank < InstancePtr->MaxBanks; Bank++) {
		IntrStatus = XGpioPs_IntrGetStatus(InstancePtr, Bank);
		IntrEnabled = XGpioPs_IntrGetEnabled(InstancePtr, Bank);
		if ((IntrStatus & IntrEnabled) != (u32) 0) {
			XGpioPs_IntrClear((XGpioPs *) InstancePtr, Bank,
					(IntrStatus & IntrEnabled));
			InstancePtr->Handler(InstancePtr->CallBackRef, Bank,
					(IntrStatus & IntrEnabled));
		}
	}
}

