/*
 * mpu_int.h: Interrupt-based readout of MPU
 * Author: Stephanie Kunkel
 */


#ifndef MPU_INT_H_
#define MPU_INT_H_

//Includes
//#include <stdio.h>
//#include <stdlib.h>
//#include <unistd.h>
#include "xparameters.h"
#include "xscugic.h"
#include "xil_exception.h"
#include "xgpiops.h"
#include "xil_types.h"
#include "mpu.h"
#include "../print_utils.h"

//Defines
#define GPIO_DEVICE_ID		XPAR_XGPIOPS_0_DEVICE_ID
#define INTC_DEVICE_ID		XPAR_SCUGIC_SINGLE_DEVICE_ID
#define GPIO_INT_ID			XPS_GPIO_INT_ID
#define GPIO_INT_PIN		13

//Functions
char imuDataAvailable();
int waitForInterrupt();
int setupMPUInt();


#endif /* MPU_INT_H_ */
