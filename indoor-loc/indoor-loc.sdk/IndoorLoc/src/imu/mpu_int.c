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

/*
 * Functions
 */

/*
 * Set Up Interrupt for IMU
 */
int setupMPUInt() {
	//Variables
	int status;
//	XScuGic_Config* IntcConfig;
//	XGpioPs_Config *GpioConfig;
//
//	//Initialize Xil Exceptions
//	Xil_ExceptionInit();
//
//	//Initialize GIC
//	IntcConfig = XScuGic_LookupConfig(INTC_DEVICE_ID);
//	status = XScuGic_CfgInitialize(&Intc, IntcConfig,
//			IntcConfig->CpuBaseAddress);
//	if (status != XST_SUCCESS) {
//		myprintf("mpu_utils.c: Error initializing SCU GIC Config.\r\n");
//		return XST_FAILURE;
//	}
//
//	//Initialize GPIO
//	GpioConfig = XGpioPs_LookupConfig(GPIO_DEVICE_ID);
//	status = XGpioPs_CfgInitialize(&Gpio, GpioConfig, GpioConfig->BaseAddr);
//	if (status != XST_SUCCESS) {
//		myprintf("mpu_utils.c: Error initializing GPIO Config.\r\n");
//		return XST_FAILURE;
//	}
//
//	//Connect to HW
//	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
//			(Xil_ExceptionHandler) XScuGic_InterruptHandler, &Intc);
//
//	//Enable GPIO Interrupt
//	XGpioPs_IntrEnablePin(&Gpio, GPIO_INT_PIN);
//
//	//Set Interrupt Type
//	XGpioPs_SetIntrTypePin(&Gpio, GPIO_INT_PIN, XGPIOPS_IRQ_TYPE_EDGE_RISING); //TODO: XGPIOPS_IRQ_TYPE_LEVEL_HIGH?
//
//	//Set Callback Handler
//	XGpioPs_SetCallbackHandler(&Gpio, &Gpio, (void *) IntrHandler);

	//Set Interrupt level
	status = mpu_set_int_level(0); //Set Interrupt for "active high" (0)
	if (status != XST_SUCCESS) {
		myprintf("mpu.c: Could not set interrupt level.\r\n");
		return XST_FAILURE;
	}

	//Set Interrupt Mode
	status = dmp_set_interrupt_mode(DMP_INT_CONTINUOUS); //Interrupt when one FIFO period has elapsed
	if (status != XST_SUCCESS) {
		myprintf("mpu.c: Could not set interrupt mode.\r\n");
		return XST_FAILURE;
	}

	//Return
	return XST_SUCCESS;
}

/*
 * Interrupt Handler for IMU
 * Updates internal copy of sensor data.
 * In: Call-back reference, bank, status
 */
void IntrHandler(void *CallBackRef, int Bank, u32 Status) {
	//Variables
	XGpioPs *Gpioint = (XGpioPs *) CallBackRef;
	int status;

	//Check whether interrupt came from INT pin of IMU
	if (XGpioPs_IntrGetStatusPin(Gpioint, GPIO_INT_PIN)) {
		//Update Sensor Data
		updateData();

		//Print Data
		status = printforDisplay(1, 1);
		if (status == XST_SUCCESS) {
			printf("\r\n");
		}

		//Clear Interrupt Pin
		XGpioPs_IntrClearPin(Gpioint, GPIO_INT_PIN);
	}
}
