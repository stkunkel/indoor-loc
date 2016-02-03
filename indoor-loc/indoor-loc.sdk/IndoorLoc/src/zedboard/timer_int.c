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
 * Timer Test
 */
int timerTest(u32 int_freq){
	//Variables
	int status;

	//Init Timer
	status = initTmrInt(int_freq);
	if (status != XST_SUCCESS){
		myprintf("timer_int.c Could not initialize AXI Timer.\r\n");
	}

	while (1){
		if (isTimerExpired() == BOOL_TRUE){
			ledRun();
		}
	}

	//Return
	return status;
}

/*
 * Is Timer Expired?
 */
bool isTimerExpired() {
	if (tmrExpired == BOOL_TRUE) {
		tmrExpired = BOOL_FALSE;
		return BOOL_TRUE;
	} else {
		u32 value = XTmrCtr_GetValue(&tmrCtr, 0);
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
 * In: interrupt frequency in Hz
 */
int initTmrInt(u32 int_freq) {
	//Variables
	int status;
	u32 reset_val;
	XScuGic_Config* IntcConfig;

	//Timer
	//Initialize Timer Controller
	status = XTmrCtr_Initialize(&tmrCtr, TMR_DEVICE_ID);
	if (status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	//Set Interrupt Handler for Timer Controller
	XTmrCtr_SetHandler(&tmrCtr, (XTmrCtr_Handler) tmrIntrHandler, &tmrCtr);

	//Reset Timer Value
	int_freq = (u32) (FPGA_FREQ/int_freq);
	reset_val = ~int_freq;

	//Reset Timer Value
	XTmrCtr_SetResetValue(&tmrCtr, 0, reset_val);

	//Set Options
	XTmrCtr_SetOptions(&tmrCtr, 0, TMR_OPTIONS);

	//GIC
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
			(Xil_ExceptionHandler) XTmrCtr_InterruptHandler, (void*) &tmrCtr);

	//Set Callback Handler for Timer Interrupts
	XTmrCtr_SetHandler(&tmrCtr, (XTmrCtr_Handler) tmrIntrHandler, &tmrCtr);

	//Enable Timer Interrupts
	XTmrCtr_EnableIntr(tmrCtr.BaseAddress, 0);

	//Enable Interrupts for Timer
	XScuGic_Enable(&Intc, INTC_TMR_INT_ID);

	//Enable Interrupts in Processor
	Xil_ExceptionEnableMask(XIL_EXCEPTION_IRQ);

	//Start Timer
	XTmrCtr_Start(&tmrCtr, 0);

	//Return
	return XST_SUCCESS;

}