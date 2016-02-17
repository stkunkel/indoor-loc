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
static volatile bool dataAvailable = BOOL_FALSE;

/*
 * Function Prototypes
 */
void ImuIntrHandler(void *CallBackRef, u32 Bank, u32 Status);
void myXGpioPs_IntrHandler(XGpioPs *InstancePtr);

/*
 * Need to Update Data?
 * if USE_DMP is defined: check whether flag was set by ISR of DMP Interrupt
 * else: check whether flag was set by ISR of AXI Timer Interrupt
 */
bool needToUpdateData() {
	//Variables
	bool update;

	//Check if data should be updated
#ifdef USE_DMP
	update = dmpDataAvailable();
#else
	update = isTimerExpired();
#endif

	//Return
	return update;
}

/*
 * Setup Interrupt (depending on USE_DMP define)
 * Returns 0 if successful
 */
int setupInt() {
	//Variables
	int status;

	//Setup
#ifdef USE_DMP
	status = setupDMPInt();
#else
	status = initTmrInt(FIFO_RATE);
#endif

	//Return
	return status;
}

/*
 * Check if data from IMU FIFO is available
 * Returns BOOL_TRUE if there is new data, BOOL_FALSE if not
 */
bool dmpDataAvailable() {
	if (dataAvailable == BOOL_TRUE) {
		dataAvailable = BOOL_FALSE;
		return BOOL_TRUE;
	} else {
		return BOOL_FALSE;
	}
}

/*
 * Set Up Interrupt for IMU
 */
int setupDMPInt() {
	//Variables
	int status;
	XScuGic_Config* IntcConfig;
	XGpioPs_Config* GpioConfig;
	unsigned char data;

	//MPU and DMP
	//Configure MPU Interrupt Pin
	status = imuReadIntConfig(&data);	//Read Register
	if (status != XST_SUCCESS) {
		return status;
	}
	data &= (~INT_LEVEL_BIT); //Clear bit --> active high
	data &= (~INT_OPEN_BIT); //Clear bit --> push-pull
	data &= (~INT_RD_CLEAR_BIT); //Clear bit --> interrupt status bits are cleared only by reading INT_STATUS (Register 58)
	data |= LATCH_INT_EN_BIT; //Set bit --> INT pin is held high until the interrupt is cleared

	//Configure Int Pin
	status = imuConfigureInt(&data);
	if (status != XST_SUCCESS) {
		return status;
	}

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

	//Set DMP Interrupt Mode
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

	//Free memory
	return XST_SUCCESS;
}

/*
 * Interrupt Handler for IMU
 * Updates internal copy of sensor data.
 * In: Call-back reference, bank, status
 */
void ImuIntrHandler(void *CallBackRef, u32 Bank, u32 Status) {
	//Variables
	int status = XST_SUCCESS;
	short irq;

	//Get IRQ Status
	status = imuGetIntStatus(&irq);
	if (status == XST_SUCCESS) {
		if ((irq & MPU_INT_STATUS_DMP_0) == MPU_INT_STATUS_DMP_0) { //DMP Interrupt
			dataAvailable = 1;
		}
	}
}

/*
 * XGpioPS Interrupt Handler
 * Compared to the original Interrupt Handler, this one only handles an interrupt if it has been enabled.
 * (Source: https://forums.xilinx.com/t5/Zynq-All-Programmable-SoC/GPIOPS-interrupt-problem/td-p/441640)
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

