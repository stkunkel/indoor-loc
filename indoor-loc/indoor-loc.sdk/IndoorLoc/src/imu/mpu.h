/*
 * mpu.h
 * Author: Stephanie Kunkel
 */

#ifndef MPU_H_
#define MPU_H_

//Includes
#include <stdio.h>
#include <stdlib.h>
#include "xstatus.h"
#include "mpu_utils.h"
#include "mpu_int.h"
#include "mpu9150/inv_mpu.h"
#include "mpu9150/inv_mpu_dmp_motion_driver.h"
#include "mpu9150/mpl/invensense_adv.h"
#include "mpu9150/mpl/ml_math_func.h"
#include "../math_utils.h"
#include "../zedboard/print_utils.h"
#include "../zedboard/time_utils.h"
#include "../zedboard/timer_int.h"
#include "../zedboard/gpio_utils.h"
#include "../bool.h"
#include "../program_parameters.h"

//IMU Parameters
#define GYRO_SENS				1000		//°/s (the smaller, the more accurate, but max is too small and more rustle)
#define ACCEL_SENS				2			//G
#define GYRO_SENS_FRS_0			131			//LSB/(°/s) for FS_SEL = 0, 16-bit
#define GYRO_SENS_FRS_1			65.5		//LSB/(°/s) for FS_SEL = 1
#define GYRO_SENS_FRS_2			32.8		//LSB/(°/s) for FS_SEL = 2
#define GYRO_SENS_FRS_3			16.4		//LSB/(°/s) for FS_SEL = 3
#define ACC_SENS_FRS_0			16384		//LSB/g for AFS_SEL = 0, 16-bit, output in 2's complement format
#define ACC_SENS_FRS_1			8192		//LSB/g for AFS_SEL = 1
#define ACC_SENS_FRS_2			4096		//LSB/g for AFS_SEL = 2
#define ACC_SENS_FRS_3			2048		//LSB/g for AFS_SEL = 3
#define MAG_SENS_FRS_1200		0.3			//uT/LSB for FRS = 1200, 13-bit, output in 2's complement format
#define TEMP_SENS_UNTRIMMED		340			//LSB/°C
#define TEMP_OFFSET_35C			-521		//LSB
#define QUATERNION_SCALING		1073741824	//Internal values: 1.0 is scaled to 2^30 = 1073741824 (msp430/eMD-6.0/core/mllite/results_holder.c --> inv_get_6axis_quaternion)
#define SENSOR_TEMP				0x80		//my define: works together with INV_XYZ_GYRO, etc.
#define SENSORS_INV				(INV_XYZ_GYRO | INV_XYZ_ACCEL | INV_XYZ_COMPASS)
#define SENSORS_ALL				(SENSORS_INV | SENSOR_TEMP)//all sensors
#define FEATURES_CAL			(DMP_FEATURE_6X_LP_QUAT | DMP_FEATURE_SEND_CAL_GYRO | DMP_FEATURE_SEND_RAW_ACCEL | DMP_FEATURE_TAP) //DMP_FEATURE_TAP for ensure DMP sends interrupts at specified rate
#define FEATURES_RAW			(DMP_FEATURE_6X_LP_QUAT | DMP_FEATURE_SEND_RAW_GYRO | DMP_FEATURE_SEND_RAW_ACCEL | DMP_FEATURE_TAP) //DMP_FEATURE_TAP for ensure DMP sends interrupts at specified rate
#define MPU_SAMPLE_RATE			200			//Hz
#define GYRO_CAL_MASK			0x01
#define ACCEL_CAL_MASK			0x02
#define MAG_CAL_MASK			0x04
#define CAL_IGNORE_SAMPLES		50
#define GRAVITY_AXIS			2			//gravity along z axis
#define FIFO_RATE				500//200

//Print Parameters
#define PRINT_NOTHING			0x00
#define PRINT_GYRO				0x01
#define PRINT_ACCEL				0x02
#define PRINT_COMP				0x04
#define PRINT_TEMP				0x08
#define PRINT_QUAT				0x10
#define PRINT_VEL				0x20
#define PRINT_POS				0x40
#define PRINT_SENSORS			(PRINT_GYRO | PRINT_ACCEL)
#define PRINT_FOR_VIEWER		(PRINT_QUAT | PRINT_POS)

//XModem
#define DATA_NUMBER_OF_BYTES	sizeof(MpuRegisterData)
#define BYTE0					0x000000FF
#define BYTE1					0x0000FF00
#define BYTE2					0x00FF0000
#define BYTE3					0xFF000000
#define SAMPLES_PER_PACKET		((int) (XMODEM_PKG_CONT_SIZE / DATA_NUMBER_OF_BYTES))
#define XMODEM_PADDING_BYTES	((int) (XMODEM_PKG_CONT_SIZE % DATA_NUMBER_OF_BYTES))

//Constants
#define GRAVITY					9.80665		//m/s^2

//Typedefs
typedef struct {
	short gyro[NUMBER_OF_AXES];
	short accel[NUMBER_OF_AXES];
	short compass[NUMBER_OF_AXES];
	long temp;
//	short fill;
} MpuRegisterData;

typedef struct {
	Vector coordinates;
	unsigned long timestamp;
} Position;

//Functions
int getFifoCount();
int printforDisplay(short printMask, char* separator);
void printRaw(short sensor[NUMBER_OF_AXES]);
void printDataWithDMP(short sensors, char* separator);
int printDataNoDMP(short sensors, char* separator);
void getRecent(float gyro[NUMBER_OF_AXES], float accel[NUMBER_OF_AXES],
		float compass[NUMBER_OF_AXES], float* temperature,
		float quat[QUATERNION_AMOUNT], float* velocity, float* position,
		unsigned long* timestamp);
void testPositionUpdate();
void quaternionTest();
int updateData();
int calibrateGyrAcc(unsigned int samples);
int getFifoCount();
int dmpGyroCalibration(bool enable);
int initIMU(unsigned int calibrationTime);
int configureDMP(unsigned short int features);
void testQuaternionComputation();
void collectRegisterData(unsigned int sampleTime,
		unsigned int calibrationTime);
int readFromRegs(short *gyro, short *accel, short* comp, long* temp,
		unsigned long *timestamp, short sensorMask);
int getImuAddr(u8* addr);

#endif /* MPU_H_ */
