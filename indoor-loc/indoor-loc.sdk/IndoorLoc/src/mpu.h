/*
 * mpu.h
 *
 *  Created on: Nov 3, 2015
 *      Author: ruppi
 */

#ifndef MPU_H_
#define MPU_H_

//Includes
#include <stdio.h>
#include <stdlib.h>
#include "xstatus.h"
#include "mpu_utils.h"
#include "mpu9150/inv_mpu.h"
#include "mpu9150/mpl/invensense_adv.h"

//IMU Parameters
#define GYRO_SENS_FRS_0		131			//LSB/(°/s) for FS_SEL = 0, 16-bit
#define GYRO_SENS_FRS_1		65.5		//LSB/(°/s) for FS_SEL = 1
#define GYRO_SENS_FRS_2		32.8		//LSB/(°/s) for FS_SEL = 2
#define GYRO_SENS_FRS_3		16.4		//LSB/(°/s) for FS_SEL = 3
#define ACC_SENS_FRS_0		16384		//LSB/g for AFS_SEL = 0, 16-bit, output in 2's complement format
#define ACC_SENS_FRS_1		8192		//LSB/g for AFS_SEL = 1
#define ACC_SENS_FRS_2		4096		//LSB/g for AFS_SEL = 2
#define ACC_SENS_FRS_3		2048		//LSB/g for AFS_SEL = 3
#define MAG_SENS_FRS_1200	0.3			//T/LSB for FRS = 1200, 13-bit, output in 2's complement format
#define TEMP_SENS_UNTRIMMED	340			//LSB/°C
#define TEMP_OFFSET_35C		-521		//LSB
#define NUMBER_OF_AXES		3			//x, y, z

//Functions
int printDataNoMPU();
int convertGyroData(short raw[NUMBER_OF_AXES], float converted[NUMBER_OF_AXES]);
int convertAccData(short raw[NUMBER_OF_AXES], float converted[NUMBER_OF_AXES]);
int convertCompassData(short raw[NUMBER_OF_AXES], float converted[NUMBER_OF_AXES]);
int convertTemperaturetoC(long* raw, float* converted);
int getImuAddr(u8* addr);
int initMPU();

#endif /* MPU_H_ */
