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
#include "xtime_l.h"
#include "mpu_utils.h"
#include "mpu9150/inv_mpu.h"
#include "mpu9150/inv_mpu_dmp_motion_driver.h"
#include "mpu9150/mpl/invensense_adv.h"
#include "mpu9150/mpl/ml_math_func.h"
#include "math_utils.h"
#include "print_utils.h"

//IMU Parameters
#define GYRO_SENS_FRS_0			131			//LSB/(°/s) for FS_SEL = 0, 16-bit
#define GYRO_SENS_FRS_1			65.5		//LSB/(°/s) for FS_SEL = 1
#define GYRO_SENS_FRS_2			32.8		//LSB/(°/s) for FS_SEL = 2
#define GYRO_SENS_FRS_3			16.4		//LSB/(°/s) for FS_SEL = 3
#define ACC_SENS_FRS_0			16384		//LSB/g for AFS_SEL = 0, 16-bit, output in 2's complement format
#define ACC_SENS_FRS_1			8192		//LSB/g for AFS_SEL = 1
#define ACC_SENS_FRS_2			4096		//LSB/g for AFS_SEL = 2
#define ACC_SENS_FRS_3			2048		//LSB/g for AFS_SEL = 3
#define MAG_SENS_FRS_1200		0.3			//T/LSB for FRS = 1200, 13-bit, output in 2's complement format
#define TEMP_SENS_UNTRIMMED		340			//LSB/°C
#define TEMP_OFFSET_35C			-521		//LSB
#define NUMBER_OF_AXES			3			//x, y, z
#define QUATERNION_AMOUNT		4			//w, x, y, z rotational angles
#define QUATERNION_SCALING		1073741824	//Internal values: 1.0 is scaled to 2^30 = 1073741824
#define SENSOR_TEMP				0x80		//my define
#define SENSORS_ALL				INV_XYZ_GYRO | INV_XYZ_ACCEL | INV_XYZ_COMPASS | SENSOR_TEMP//all sensors
#define FEATURES_CAL			DMP_FEATURE_6X_LP_QUAT | DMP_FEATURE_SEND_CAL_GYRO | DMP_FEATURE_SEND_RAW_ACCEL | DMP_FEATURE_TAP //DMP_FEATURE_TAP for ensure DMP sends interrupts at specified rate
#define FEATURES_RAW			DMP_FEATURE_6X_LP_QUAT | DMP_FEATURE_SEND_RAW_GYRO | DMP_FEATURE_SEND_RAW_ACCEL | DMP_FEATURE_TAP //DMP_FEATURE_TAP for ensure DMP sends interrupts at specified rate
#define MPU_SAMPLE_RATE			200			//Hz
#define DMP_FIFO_RATE			10			//Hz
#define CAL_SAMPLES				1000.0
#define GYRO_CAL_MASK			0x01
#define ACCEL_CAL_MASK			0x02
#define MAG_CAL_MASK			0x04

//Functions
void printQuatDrift(unsigned int time_min);
int getQuatDrift(float *quat_drift, char calibration, unsigned int time_min);
int printQuatForDisplay();
int printDataWithDMP();
int printDataNoDMP(short int *sensors);
int calibrateGyrAcc();
int dmpGyroCalibration(char enable);
int configureDMP(unsigned short int features, unsigned short fifoRate);
int initDMP();
int getImuAddr(u8* addr);
int initMPU();

#endif /* MPU_H_ */
