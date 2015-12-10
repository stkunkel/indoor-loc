/*
 * mpu_utils.h
 * Author: Stephanie Kunkel
 */

#ifndef IMU_H_
#define IMU_H_

//Includes
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <xil_io.h>
#include "xil_types.h"
#include "xparameters.h"
#include "xgpiops.h"
#include "xstatus.h"
#include "xtime_l.h"
#include "iic_utils.h"
#include "../print_utils.h"

//Zynq Parameters
#define GPIO_DEVICE_ID  	XPAR_XGPIOPS_0_DEVICE_ID //from xparameters.h
#define PS_IMU_AD0_PIN 	12 //MIO12
#define PS_IMU_INT_PIN 	13 //MIO13


//IMU Parameters
#define IMU_ADDR_MASK		0b1101000 	//b110100X - X determined by logic level on AD0
#define IMU_LAST_ADDR_BIT	0			//one IMU

//Functions
int imuI2cRead(unsigned char slave_addr, unsigned char reg_addr, unsigned char length, unsigned char *data);
int imuI2cWrite(unsigned char slave_addr, unsigned char reg_addr, unsigned char length, unsigned char *data);
void imuDelay(unsigned long ms);
int imuInit(u8 *imuAddr);
void imuGet_ms(unsigned long* timestamp);
int imuLog_i(const char *format, ...);
int imuLog_e(const char *format, ...);
int readInt(u32 *DataRead);

#endif /* IMU_H_ */
