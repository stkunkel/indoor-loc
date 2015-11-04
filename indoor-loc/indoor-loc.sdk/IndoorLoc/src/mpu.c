/*
 * mpu.c
 *
 *  Created on: Nov 3, 2015
 *      Author: ruppi
 */

/*
 * Includes
 */
#include "mpu.h"

/*
 * Variables
 */
static u8 imuAddr = 0;

/*
 * Print Data and don't use MPU
 */
int printDataNoMPU() {
	//Variables
	int status, i;
	long temp_raw;
	short raw[NUMBER_OF_AXES];
	float conv[NUMBER_OF_AXES], temp_conv;
	unsigned long temp_timestamp;
	unsigned long int timestamp;

	//Get Gyro
	status = mpu_get_gyro_reg(raw, &timestamp);
	if (status != XST_SUCCESS) {
		printf("mpu.c: Error getting Gyroscope data.\n\r");
		return 0;
	}

	//Convert Gyro
	status = convertGyroData(raw, conv);
	if (status != 0) {
		printf("mpu.c: Error converting Gyroscope data.\n\r");
		return XST_FAILURE;
	}

	//Print Gyro
	printf("Gyro: (");
	for (i = 0; i < NUMBER_OF_AXES; i++) {
		printf("%.6fdgr/s", conv[i]);
		if (i < NUMBER_OF_AXES - 1) {
			printf(", ");
		} else {
			printf(")| ");
		}
	}

	//Get Acc
	status = mpu_get_accel_reg(raw, &timestamp);
	if (status != 0) {
		printf("mpu.c: Error getting Acc data.\n\r");
		return XST_FAILURE;
	}

	//Convert Acc
	status = convertAccData(raw, conv);
	if (status != 0) {
		printf("mpu.c: Error converting Acc data.\n\r");
		return XST_FAILURE;
	}

	//Print Acc
	printf("Acc: (");
	for (i = 0; i < NUMBER_OF_AXES; i++) {
		printf("%.0fg", conv[i]);
		if (i < NUMBER_OF_AXES - 1) {
			printf(", ");
		} else {
			printf(") | ");
		}
	}

	//Get Compass
	status = mpu_get_compass_reg(raw, &timestamp);
	if (status != 0) {
		printf("mpu.c: Error getting Compass data.\n\r");
		return XST_FAILURE;
	}

	//Convert Compass
	status = convertCompassData(raw, conv);
	if (status != 0) {
		printf("mpu.c: Error converting Compass data.\n\r");
		return XST_FAILURE;
	}

	//Print Compass
	printf("Compass: (");
	for (i = 0; i < NUMBER_OF_AXES; i++) {
		printf("%.1fT", conv[i]);
		if (i < NUMBER_OF_AXES - 1) {
			printf(", ");
		} else {
			printf(") | ");
		}
	}

	//Get Temperature
	status = mpu_get_temperature(&temp_raw, &temp_timestamp);
	if (status != 0) {
		printf("mpu.c: Error getting Temperature data.\r\n");
		return XST_FAILURE;
	}

	//Convert Temperature
	status = convertTemperaturetoC(&temp_raw, &temp_conv);
	if (status != 0) {
		printf("mpu.c: Error converting Temperature data.\r\n");
		return XST_FAILURE;
	}

	//Print Temperature
	printf("Temperature: %.4fdgrC\r\n", temp_conv);

	//Return
	return XST_SUCCESS;
}

/*
 * Convert Gyroscope Data using Sensitivity
 */
int convertGyroData(short raw[NUMBER_OF_AXES], float converted[NUMBER_OF_AXES]) {
	//Variables
	int status, i;
	float sensitivity;

	//get sensitivity
	status = mpu_get_gyro_sens(&sensitivity);
	if (status != XST_SUCCESS) {
		printf("mpu.c: Error getting sensitivity of gyroscope.\r\n");
		return XST_FAILURE;
	}

	//convert data
	for (i = 0; i <= NUMBER_OF_AXES; i++) {
		converted[i] = raw[i] / sensitivity;
	}

	//Return
	return XST_SUCCESS;
}

/*
 * Convert Accelerometer Data using Sensitivity
 */
int convertAccData(short raw[NUMBER_OF_AXES], float converted[NUMBER_OF_AXES]) {
	//Variables
	int status, i;
	unsigned short sensitivity;

	//get sensitivity
	status = mpu_get_accel_sens(&sensitivity);
	if (status != XST_SUCCESS) {
		printf("mpu.c: Error getting sensitivity of gyroscope.\r\n");
		return XST_FAILURE;
	}

	//convert data
	for (i = 0; i <= NUMBER_OF_AXES; i++) {
		converted[i] = raw[i] / sensitivity * 1.0;
	}

	//Return
	return XST_SUCCESS;
}

/*
 * Convert Magnetometer Data using Sensitivity
 */
int convertCompassData(short raw[NUMBER_OF_AXES],
		float converted[NUMBER_OF_AXES]) {
	//Variables
	int i;

	//convert data
	for (i = 0; i <= NUMBER_OF_AXES; i++) {
		converted[i] = raw[i] * MAG_SENS_FRS_1200;
	}

	//Return
	return XST_SUCCESS;
}

int convertTemperaturetoC(long* raw, float* converted) {
	//convert data
	*converted = *raw / 65536.0;

	//Return
	return XST_SUCCESS;
}

/*
 * get IMU Address
 */
int getImuAddr(u8* addr) {
	if (imuAddr == 0) {
		initMPU();
	}
	*addr = imuAddr;
	return XST_SUCCESS;
}

/*
 * Initialize IMU
 */
int initMPU() {
	//Variables
	int status;

	//1. Init IMU (Set Address, etc.)
	status = imuInit(&imuAddr);
	if (status != XST_SUCCESS) {
		xil_printf("mpu.c: Error in Setting IMU Address.\n\r");
		return XST_FAILURE;
	}

	//2. Init IMU
	struct int_param_s param;
	status = mpu_init(&param);
	if (status != 0) {
		xil_printf("mpu.c: Error initializing IMU\r\n.");
		return XST_FAILURE;
	}

	//3. Select Sensors
	unsigned char sensors = INV_XYZ_GYRO | INV_XYZ_ACCEL | INV_XYZ_COMPASS;
	mpu_set_sensors(sensors);
	if (status != 0) {
		xil_printf("mpu.c: Error setting sensors.\r\n");
		return XST_FAILURE;
	}

	//Return
	return XST_SUCCESS;
}
