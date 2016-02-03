/*
 * timer_int.c: AXI Timer Control
 * Author: Stephanie Kunkel
 * Source: http://ece.gmu.edu/coursewebpages/ECE/ECE699_SW_HW/S15/viewgraphs/ECE699_lecture_4.pdf
 */

/*
 * Includes
 */
#include "timer_int.h"

/*
 * Global Vairables
 */
XTmrCtr tmrCtr;
XScuGic Intc;
static bool tmrExpired = BOOL_FALSE;

/*
 * Function Prototypes
 */
void TMR_Intr_Handler(void *InstancePtr);

/*
 * Is Timer Expired?
 */
bool isTimerExpired() {
	if (tmrExpired == BOOL_TRUE) {
		tmrExpired = BOOL_FALSE;
		return BOOL_TRUE;
	} else {
		return BOOL_FALSE;
	}
}

/*
 * Timer Interrupt Handler
 */
void tmrIntrHandler(void *InstancePtr) {
	if (XTmrCtr_IsExpired(&tmrCtr, 0)) {
		//Stop Timer Controller
		XTmrCtr_Stop(&tmrCtr, 0);

		//Set flag
		tmrExpired = BOOL_TRUE;

		//Reset Timer Controller
		XTmrCtr_Reset(&tmrCtr, 0);

		//Start Timer
		XTmrCtr_Start(&tmrCtr, 0);
	}
}

/*
 * Initialize Timer Interrupt
 */
int initTmrInt() {
	//Variables
	int status;
	XScuGic_Config* IntcConfig;

	//Initialize Timer Controller
	status = XTmrCtr_Initialize(&tmrCtr, TMR_DEVICE_ID);
	if (status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	//Set Interrupt Handler for Timer Controller
	XTmrCtr_SetHandler(&tmrCtr, (XTmrCtr_Handler) tmrIntrHandler, &tmrCtr);

	//Reset Timer Value
	XTmrCtr_SetResetValue(&tmrCtr, 0, TMR_LOAD);

	//Set Options
	XTmrCtr_SetOptions(&tmrCtr, 0, TMR_OPTIONS);

	//Initialize Xil Exceptions
	Xil_ExceptionInit();

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
	XScuGic_Connect(&Intc, INTC_TMR_INT_ID,
			(Xil_ExceptionHandler) tmrIntrHandler, (void*) &tmrCtr);

	//Enable Interrupts for Timer
	XScuGic_Enable(&Intc, INTC_TMR_INT_ID);

	//Enable Interrupts in Processor
	Xil_ExceptionEnableMask(XIL_EXCEPTION_IRQ);

	//Return
	return XST_SUCCESS;

}
