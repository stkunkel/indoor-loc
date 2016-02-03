/*
 * timer_int.h: AXI Timer Control
 * Author: Stephanie Kunkel
 */

#ifndef TIMER_INT_H_
#define TIMER_INT_H_

//Includes
#include "xparameters.h"
#include "xscugic.h"
#include "xil_exception.h"
#include "xtmrctr.h"
#include "../bool.h"
#include "../print_utils.h"

//Parameters
#define TMR_DEVICE_ID		XPAR_TMRCTR_0_DEVICE_ID
#define INTC_TMR_INT_ID		XPAR_FABRIC_AXI_TIMER_0_INTERRUPT_INTR
#define INTC_DEVICE_ID		XPAR_SCUGIC_SINGLE_DEVICE_ID
#define	TMR_LOAD			0xF8000000
#define TMR_OPTIONS			(XTC_INT_MODE_OPTION | XTC_AUTO_RELOAD_OPTION)

//Functions
int initTmrInt();

#endif /* TIMER_INT_H_ */
