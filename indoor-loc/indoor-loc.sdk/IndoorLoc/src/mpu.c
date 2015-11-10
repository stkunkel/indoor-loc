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
static char dmpReady = 0;

/*
 * Print Data using DMP
 */
void printDataWithDMP() {
	//Variables
	int status, i = 0;
	short gyro[NUMBER_OF_AXES], accel[NUMBER_OF_AXES], compass[NUMBER_OF_AXES];
	long temp_raw, quat[QUATERNION_AMOUNT];
	unsigned long timestamp;
	short int sensors;
	unsigned char* more = (unsigned char *) malloc(100);
	float conv[NUMBER_OF_AXES], temp_conv;

	//init DMP
	if (!dmpReady) {
		status = initDMP(FEATURES, DMP_FIFO_RATE);
		if (status != XST_SUCCESS) {
			printf("Error initializing DMP.\r\n");
			return;
		}
	}

	//Read FIFO
	do {
		status = dmp_read_fifo(gyro, accel, quat, &timestamp, &sensors, more);
		i++;

		if (i >= 100) {
			printf("mpu.c: Could not read DMP FIFO.\n\r");
			return;
		}
	} while (status != XST_SUCCESS);

	//Convert Gyro
	status = convertGyroData(gyro, conv);
	if (status != 0) {
		printf("Error converting Gyroscope data.");
	} else {

		//Print Gyro
		printGyro(conv);
	}
	printf(" | ");

	//Convert Acc
	status = convertAccData(accel, conv);
	if (status != 0) {
		printf("Error converting Acc data.");
	} else {

		//Print Acc
		printAccel(conv);
	}
	printf(" | ");

	//Get Compass
	status = mpu_get_compass_reg(compass, &timestamp);
	if (status != 0) {
		printf("Error getting Compass data.");
	} else {

		//Convert Compass
		status = convertCompassData(compass, conv);
		if (status != 0) {
			printf("Error converting Compass data.");
		} else {

			//Print Compass
			printCompass(conv);
		}
	}
	printf(" | ");

	//Get Temperature
	status = mpu_get_temperature(&temp_raw, &timestamp);
	if (status != 0) {
		printf("Error getting Temperature data.");
	} else {

		//Convert Temperature
		status = convertTemperaturetoC(&temp_raw, &temp_conv);
		if (status != 0) {
			printf("Error converting Temperature data.");
		} else {

			//Print Temperature
			printTemp(&temp_conv);
		}
	}
	printf(" | ");

	//Print Quaternions
	printQuat(quat);

	//Print new line
	printf("\r\n");

}

/*
 * Print Data and don't use DMP
 */
void printDataNoDMP() {
	//Variables
	int status;
	long temp_raw;
	short raw[NUMBER_OF_AXES];
	float conv[NUMBER_OF_AXES], temp_conv;
	unsigned long temp_timestamp;
	unsigned long int timestamp;

	//Initialize IMU first if required
	if (imuAddr == 0) {
		status = initMPU();
		if (status != XST_SUCCESS) {
			printf("mpu.c: Error initializing IMU.\n\r");
			return;
		}
	}

	//Get Gyro
	status = mpu_get_gyro_reg(raw, &timestamp);
	if (status != XST_SUCCESS) {
		printf("Error getting Gyroscope data.");
	} else {

		//Convert Gyro
		status = convertGyroData(raw, conv);
		if (status != 0) {
			printf("Error converting Gyroscope data.");
		} else {

			//Print Gyro
			printGyro(conv);
		}
	}
	printf(" | ");

	//Get Acc
	status = mpu_get_accel_reg(raw, &timestamp);
	if (status != 0) {
		printf("Error getting Acc data.");
	} else {

		//Convert Acc
		status = convertAccData(raw, conv);
		if (status != 0) {
			printf("Error converting Acc data.");
		} else {

			//Print Acc
			printAccel(conv);
		}
	}
	printf(" | ");

	//Get Compass
	status = mpu_get_compass_reg(raw, &timestamp);
	if (status != 0) {
		printf("Error getting Compass data.");
	} else {

		//Convert Compass
		status = convertCompassData(raw, conv);
		if (status != 0) {
			printf("Error converting Compass data.");
		} else {

			//Print Compass
			printCompass(conv);
		}
	}
	printf(" | ");

	//Get Temperature
	status = mpu_get_temperature(&temp_raw, &temp_timestamp);
	if (status != 0) {
		printf("Error getting Temperature data.");
	} else {

		//Convert Temperature
		status = convertTemperaturetoC(&temp_raw, &temp_conv);
		if (status != 0) {
			printf("Error converting Temperature data.");
		} else {

			//Print Temperature
			printTemp(&temp_conv);
		}
	}

	//Print new line
	printf("\r\n");
}

/*
 * Convert and Print Gyro Data
 */
void printGyro(float conv[NUMBER_OF_AXES]) {
	//Variables
	int i;

	//Print Gyro
	printf("Gyro: (");
	for (i = 0; i < NUMBER_OF_AXES; i++) {
		printf("%.6fdgr/s", conv[i]);
		if (i < NUMBER_OF_AXES - 1) {
			printf(", ");
		} else {
			printf(")");
		}
	}

}

/*
 * Convert and Print Accelerometer Data
 */
void printAccel(float conv[NUMBER_OF_AXES]) {
	//Variables
	int i;

	//Print Acc
	printf("Acc: (");
	for (i = 0; i < NUMBER_OF_AXES; i++) {
		printf("%.0fg", conv[i]);
		if (i < NUMBER_OF_AXES - 1) {
			printf(", ");
		} else {
			printf(")");
		}
	}

}

/*
 * Convert and Print Compass Data
 */
void printCompass(float conv[NUMBER_OF_AXES]) {
	//Variables
	int i;

	//Print Compass
	printf("Compass: (");
	for (i = 0; i < NUMBER_OF_AXES; i++) {
		printf("%.1fT", conv[i]);
		if (i < NUMBER_OF_AXES - 1) {
			printf(", ");
		} else {
			printf(")");
		}
	}
}

/*
 * Convert and Print Temperature Data
 */
void printTemp(float* temp_conv) {
	//Print Temperature
	printf("Temperature: %.4fdgrC\r\n", *temp_conv);
}

/*
 * Print Quaternions
 */
void printQuat(long quat[QUATERNION_AMOUNT]) {
	//Variables
	int i;

	//Print
	printf("Quaternions: ");
	for (i = 0; i < QUATERNION_AMOUNT; i++) {
		printf("%lu", quat[i]);
		if (i < QUATERNION_AMOUNT - 1) {
			printf(", ");
		}
	}

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
 * Init DMP
 */
int initDMP(unsigned short int features, unsigned short fifoRate) {
	//Variables
	int status;

	//Initialize IMU first if required
	if (imuAddr == 0) {
		status = initMPU();
		if (status != XST_SUCCESS) {
			return XST_FAILURE;
		}
	}

	//Load Firmware and set flag
	status = dmp_load_motion_driver_firmware();
	if (status != XST_SUCCESS) {
		dmpReady = 0;
		printf("mpu.c: Error loading firmware of DMP.\r\n");
		return XST_FAILURE;
	}

	//IS DMP Enabled?
	unsigned char enabled;
	status = mpu_get_dmp_state(&enabled);
	if (status != XST_SUCCESS) {
		printf("mpu.c: Could not get DMP enabled.\n\r");
	}

	//Enable DMP
	if (!enabled) {
		enabled = 1;
		status = mpu_set_dmp_state(enabled);
		if (status != XST_SUCCESS) {
			printf("mpu.c: Could not enable DMP.\r\n");
			return XST_FAILURE;
		}
	}

	//Enable Features
	status = dmp_enable_feature(FEATURES);
	if (status != XST_SUCCESS) {
		printf("mpu.c: Error enabling desired features.\r\n");
		return XST_FAILURE;
	}

	//Enable MPU FIFO
	status = mpu_configure_fifo(INV_XYZ_ACCEL | INV_XYZ_GYRO);
	if (status != XST_SUCCESS) {
		printf("mpu.c: Error configuring FIFO.\n\r");
	}

	//Set FIFO rate
	status = dmp_set_fifo_rate(100);
	if (status != XST_SUCCESS) {
		printf("mpu.c: Error Setting FIFO rate.\n\r");
	}

	//Set flag, sleep and return
	dmpReady = 1;
	sleep(1);
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
	unsigned char sensors = SENSORS;
	mpu_set_sensors(sensors);
	if (status != 0) {
		xil_printf("mpu.c: Error setting sensors.\r\n");
		return XST_FAILURE;
	}

	//Sleep and Return
	sleep(1);
	return XST_SUCCESS;
}