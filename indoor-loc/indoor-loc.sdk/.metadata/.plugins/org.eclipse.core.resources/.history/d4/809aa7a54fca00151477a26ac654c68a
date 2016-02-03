/*
 * mpu_int.h: Interrupt-based readout of MPU
 * Author: Stephanie Kunkel
 */


#ifndef MPU_INT_H_
#define MPU_INT_H_

//Includes
#include "xparameters.h"
#include "xscugic.h"
#include "xil_exception.h"
#include "xgpiops.h"
#include "xil_types.h"
#include "mpu.h"
#include "../print_utils.h"
#include "../program_parameters.h"

//Defines
#define GPIO_DEVICE_ID		XPAR_XGPIOPS_0_DEVICE_ID
#define INTC_DEVICE_ID		XPAR_SCUGIC_SINGLE_DEVICE_ID
#define GPIO_INT_ID			XPS_GPIO_INT_ID
#define GPIO_INT_PIN		13

//Bit Masks
#define INT_LEVEL_BIT			0x80
#define INT_OPEN_BIT			0x40
#define INT_RD_CLEAR_BIT		0x10
#define LATCH_INT_EN_BIT		0x20
#define DATA_RDY__INT_BIT		0x01
#define IIC_MST_INT_BIT			0x08
#define FIFO_OFLOW_INT_BIT		0x10

//Functions
char imuDataAvailable();
int waitForInterrupt();
int setupMPUInt();


#endif /* MPU_INT_H_ */
