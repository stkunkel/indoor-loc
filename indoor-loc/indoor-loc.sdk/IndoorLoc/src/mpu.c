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
 * Function Prototypes
 */
void printGyro(float conv[NUMBER_OF_AXES]);
void printAccel(float conv[NUMBER_OF_AXES]);
void printCompass(float conv[NUMBER_OF_AXES]);
void printTemp(float* temp_conv);
void printQuat(float quat[QUATERNION_AMOUNT]);
void printRotationAngle(long quat[QUATERNION_AMOUNT]);
void printEulerAngles(float* sigma, float* theta, float* psi);
int convertGyroData(short raw[NUMBER_OF_AXES], float converted[NUMBER_OF_AXES]);
int convertAccData(short raw[NUMBER_OF_AXES], float converted[NUMBER_OF_AXES]);
int convertCompassData(short raw[NUMBER_OF_AXES],
		float converted[NUMBER_OF_AXES]);
int convertTemperaturetoC(long* raw, float* converted);
int convertQuatenion(long raw[QUATERNION_AMOUNT], float conv[QUATERNION_AMOUNT]);

/*
 * Variables
 */
static u8 imuAddr = 0;
static char dmpReady = 0;
static int count = 0;
float quat_conv[QUATERNION_AMOUNT];
static int gyrAccIsCal = 0;

/*
 * Print one Quaternion for Display
 * (No error messages!)
 */
int printQuatForDisplay() {
	//Variables
	int status = XST_FAILURE, i = 0;
	short gyro[NUMBER_OF_AXES], accel[NUMBER_OF_AXES];
	long quat[QUATERNION_AMOUNT];
	unsigned long timestamp;
	short int sensors;
	unsigned char* more = (unsigned char *) malloc(100);
	//float quat_conv[QUATERNION_AMOUNT];

	//init DMP
	if (!dmpReady) {
		status = initDMP(FEATURES, DMP_FIFO_RATE, 1);
		if (status != XST_SUCCESS) {
			return status;
		}
	}

	//Read FIFO
	count++;
	if (count >= 500) {
		//Reset Count, increment read count
		count = 0;

		//Read Data
		status = dmp_read_fifo(gyro, accel, quat, &timestamp, &sensors, more);

		//Convert Quaternion
		if (status == XST_SUCCESS) {
			status = convertQuatenion(quat, quat_conv);
		}

		//Print Quaternion
		if (status == XST_SUCCESS) {
			for (i = 0; i < QUATERNION_AMOUNT; i++) {
				printf("%f ", quat_conv[i]);
			}
			printf("\n\r");
		}
	}
	free(more);
	return status;
}

/*
 * Print Data using DMP
 */
int printDataWithDMP() {
	//Variables
	int status, i = 0;
	short gyro[NUMBER_OF_AXES], accel[NUMBER_OF_AXES], compass[NUMBER_OF_AXES];
	long temp_raw, quat[QUATERNION_AMOUNT];
	unsigned long timestamp;
	short int sensors;
	unsigned char* more = (unsigned char *) malloc(100);
	float conv[NUMBER_OF_AXES], temp_conv, quat_conv[QUATERNION_AMOUNT];

	//init DMP
	if (!dmpReady) {
		status = initDMP(FEATURES, DMP_FIFO_RATE, 1);
		if (status != XST_SUCCESS) {
#ifdef DEBUG
			printf("mpu.c Error initializing DMP.\r\n");
#endif
			return XST_FAILURE;
		}
	}

	//Read FIFO
	do {
		status = dmp_read_fifo(gyro, accel, quat, &timestamp, &sensors, more);
		i++;

		if (i >= 100) {
#ifdef DEBUG
			printf("mpu.c: Could not read DMP FIFO.\n\r");
#endif
			return XST_FAILURE;
		}
	} while (status != XST_SUCCESS);

	//Convert Gyro
	status = convertGyroData(gyro, conv);
	if (status != 0) {
#ifdef DEBUG
		printf("mpu.c Error converting Gyroscope data.");
#endif
		return XST_FAILURE;
	} else {

		//Print Gyro
		printGyro(conv);
	}
	printf(" | ");

	//Convert Acc
	status = convertAccData(accel, conv);
	if (status != 0) {
#ifdef DEBUG
		printf("mpu.c Error converting Acc data.");
#endif
		return XST_FAILURE;
	} else {

		//Print Acc
		printAccel(conv);
	}
	printf(" | ");

	//Get Compass
	status = mpu_get_compass_reg(compass, &timestamp);
	if (status != 0) {
#ifdef DEBUG
		printf("mpu.c Error getting Compass data.");
#endif
		return XST_FAILURE;
	} else {

		//Convert Compass
		status = convertCompassData(compass, conv);
		if (status != 0) {
#ifdef DEBUG
			printf("mpu.c Error converting Compass data.");
#endif
			return XST_FAILURE;
		} else {

			//Print Compass
			printCompass(conv);
		}
	}
	printf(" | ");

	//Get Temperature
	status = mpu_get_temperature(&temp_raw, &timestamp);
	if (status != 0) {
#ifdef DEBUG
		printf("mpu.c Error getting Temperature data.");
#endif
		return XST_FAILURE;
	} else {

		//Convert Temperature
		status = convertTemperaturetoC(&temp_raw, &temp_conv);
		if (status != 0) {
#ifdef DEBUG
			printf("mpu.c Error converting Temperature data.");
#endif
			return XST_FAILURE;
		} else {

			//Print Temperature
			printTemp(&temp_conv);
		}
	}
	printf(" | ");

//Convert Quaternion
	status = convertQuatenion(quat, quat_conv);
	if (status != 0) {
#ifdef DEBUG
		printf("mpu.c Error converting quaternion.");
#endif
		return XST_FAILURE;
	} else {
		//Print Quaternion
		printQuat(quat_conv);
	}

//Print new line
	printf("\r\n");

	//Free Memory and Return
	free(more);
	return XST_SUCCESS;
}

/*
 * Print Data and don't use DMP
 */
int printDataNoDMP() {
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
#ifdef DEBUG
			printf("mpu.c: Error initializing IMU.\n\r");
#endif
			return XST_FAILURE;
		}
	}

	//Get Gyro
	status = mpu_get_gyro_reg(raw, &timestamp);
	if (status != XST_SUCCESS) {
#ifdef DEBUG
		printf("mpu.c Error getting Gyroscope data.");
#endif
		return XST_FAILURE;
	} else {

		//Convert Gyro
		status = convertGyroData(raw, conv);
		if (status != 0) {
#ifdef DEBUG
			printf("mpu.c Error converting Gyroscope data.");
#endif
			return XST_FAILURE;
		} else {

			//Print Gyro
			printGyro(conv);
		}
	}
	printf(" | ");

	//Get Acc
	status = mpu_get_accel_reg(raw, &timestamp);
	if (status != 0) {
#ifdef DEBUG
		printf("mpu.c Error getting Acc data.");
#endif
		return XST_FAILURE;
	} else {

		//Convert Acc
		status = convertAccData(raw, conv);
		if (status != 0) {
#ifdef DEBUG
			printf("mpu.c Error converting Acc data.");
#endif
			return XST_FAILURE;
		} else {

			//Print Acc
			printAccel(conv);
		}
	}
	printf(" | ");

	//Get Compass
	status = mpu_get_compass_reg(raw, &timestamp);
	if (status != 0) {
#ifdef DEBUG
		printf("mpu.c Error getting Compass data.");
#endif
		return XST_FAILURE;
	} else {

		//Convert Compass
		status = convertCompassData(raw, conv);
		if (status != 0) {
#ifdef DEBUG
			printf("mpu.c Error converting Compass data.");
#endif
			return XST_FAILURE;
		} else {

			//Print Compass
			printCompass(conv);
		}
	}
	printf(" | ");

	//Get Temperature
	status = mpu_get_temperature(&temp_raw, &temp_timestamp);
	if (status != 0) {
#ifdef DEBUG
		printf("mpu.c Error getting Temperature data.");
#endif
		return XST_FAILURE;
	} else {

		//Convert Temperature
		status = convertTemperaturetoC(&temp_raw, &temp_conv);
		if (status != 0) {
#ifdef DEBUG
			printf("mpu.c Error converting Temperature data.");
#endif
			return XST_FAILURE;
		} else {

			//Print Temperature
			printTemp(&temp_conv);
		}
	}

	//Print new line
	printf("\r\n");

	//Return
	return XST_SUCCESS;
}

/*
 * Print Gyro Data (already converted)
 */
void printGyro(float conv[NUMBER_OF_AXES]) {
	//Variables
	int i;

	//Print Gyro
	printf("Gyro: (");
	for (i = 0; i < NUMBER_OF_AXES; i++) {
		printf("%.2fdgr/s", conv[i]);
		if (i < NUMBER_OF_AXES - 1) {
			printf(", ");
		} else {
			printf(")");
		}
	}

}

/*
 * Print Accelerometer Data (already converted)
 */
void printAccel(float conv[NUMBER_OF_AXES]) {
	//Variables
	int i;

	//Print Acc
	printf("Acc: (");
	for (i = 0; i < NUMBER_OF_AXES; i++) {
		printf("%.2fg", conv[i]);
		if (i < NUMBER_OF_AXES - 1) {
			printf(", ");
		} else {
			printf(")");
		}
	}

}

/*
 * Print Compass Data  (already converted)
 */
void printCompass(float conv[NUMBER_OF_AXES]) {
	//Variables
	int i;

	//Print Compass
	printf("Comp: (");
	for (i = 0; i < NUMBER_OF_AXES; i++) {
		printf("%.1fuT", conv[i]);
		if (i < NUMBER_OF_AXES - 1) {
			printf(", ");
		} else {
			printf(")");
		}
	}
}

/*
 * Convert and Print Temperature Data  (already converted)
 */
void printTemp(float* temp_conv) {
	//Print Temperature
	printf("Temp: %.2fdgrC\r\n", *temp_conv);
}

/*
 * Print Quaternions (already converted)
 */
void printQuat(float quat[QUATERNION_AMOUNT]) {
	//Variables
	int i;

	//Print
	printf("Quat: ");
	for (i = 0; i < QUATERNION_AMOUNT; i++) {
		printf("%f", quat[i]);
		if (i < QUATERNION_AMOUNT - 1) {
			printf(", ");
		}
	}

}

/*
 * Print Rotation Angles
 */
void printRotationAngle(long quat[QUATERNION_AMOUNT]) {
	printf("Rotation Angle: %f", quaternion_to_rotation_angle(quat));

}

/*
 * Print Euler Angles
 *
 * Input_ Quaternions, already converted
 */
void printEulerAngles(float* sigma, float* theta, float* psi) {
//Print
	printf("Euler Angles: roll=%f, pitch=%f, yaw=%f", *sigma, *theta, *psi);
}

/*
 * Calibrate Gyroscope and Accelerometer
 */
int calibrateGyrAcc() {
	//Variables
	int samples, status, i;
	long gyro_bias[NUMBER_OF_AXES] = { 0, 0, 0 };
	long accel_bias[NUMBER_OF_AXES] = { 0, 0, 0 };
	unsigned long gyro_timestamp, accel_timestamp;
	short gyro_data[NUMBER_OF_AXES], accel_data[NUMBER_OF_AXES];

	//Init MPU and DMP
	status = initDMP(FEATURES, DMP_FIFO_RATE, 0);
	if (status != XST_SUCCESS) {
		gyrAccIsCal = 0;
		return status;
	}

	//Set Gyro Bias to zero
	status = mpu_set_gyro_bias_reg(gyro_bias);
	if (status != XST_SUCCESS) {
		gyrAccIsCal = 0;
		return status;
	}

	//Set Accel Bias to zero
	status = mpu_set_accel_bias_6050_reg(accel_bias);
	if (status != XST_SUCCESS) {
		gyrAccIsCal = 0;
		return status;
	}

	//Get Samples
	for (samples = 0; samples <= (1 << CAL_SAMPLES_EXP); samples++) {
		//Read out raw gyro data
		status = mpu_get_gyro_reg(gyro_data, &gyro_timestamp);
		if (status != XST_SUCCESS) {
			samples--;
			continue;
		}

		//Read out raw accel data
		status = mpu_get_accel_reg(accel_data, &accel_timestamp);
		if (status != XST_SUCCESS) {
			samples--;
			continue;
		}

		//Compare timestamp
		if (gyro_timestamp != accel_timestamp) {
			samples--;
			continue;
		}

		//Add gyro and accel data to averages
		for (i = 0; i < NUMBER_OF_AXES; i++) {
			gyro_bias[i] += (gyro_data[i] >> CAL_SAMPLES_EXP);
			accel_bias[i] += (accel_data[i] >> CAL_SAMPLES_EXP); //TODO
		}
	}

	//Set Gyro Bias (Bias inputs are LSB in +-1000dps format)
	status = mpu_set_gyro_bias_reg(gyro_bias);
	if (status != XST_SUCCESS) {
		gyrAccIsCal = 0;
		return status;
	}

	//Set Accel Bias (Bias inputs are LSB in +-16G format.)
	status = mpu_set_accel_bias_6050_reg(accel_bias);
	if (status != XST_SUCCESS) {
		gyrAccIsCal = 0;
		return status;
	}

	//Set calibrated flag
	gyrAccIsCal = 1;

	//Return
	return gyrAccIsCal;
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
#ifdef DEBUG
		printf("mpu.c: Error getting sensitivity of gyroscope.\r\n");
#endif
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
#ifdef DEBUG
		printf("mpu.c: Error getting sensitivity of gyroscope.\r\n");
#endif
		return XST_FAILURE;
	}

//convert data
	for (i = 0; i <= NUMBER_OF_AXES; i++) {
		converted[i] = raw[i] * 1.0 / sensitivity;
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

/*
 * Convert Temperature Data
 */
int convertTemperaturetoC(long* raw, float* converted) {
//convert data
	*converted = *raw / 65536.0;

//Return
	return XST_SUCCESS;
}

/*
 * Convert Quaternions using Scaling
 */
int convertQuatenion(long raw[QUATERNION_AMOUNT], float conv[QUATERNION_AMOUNT]) {
//Variables
	int i;

//Conversion
	for (i = 0; i < QUATERNION_AMOUNT; i++) {
		conv[i] = (float) raw[i] / QUATERNION_SCALING; //quat[i]>>30
	}

//Return
	return XST_SUCCESS;
}

/*
 * Init DMP
 */
int initDMP(unsigned short int features, unsigned short fifoRate,
		char enableGyroCalibration) {
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
#ifdef DEBUG
		printf("mpu.c: Error loading firmware of DMP.\r\n");
#endif
		return XST_FAILURE;
	}

//IS DMP Enabled?
	unsigned char enabled;
	status = mpu_get_dmp_state(&enabled);
	if (status != XST_SUCCESS) {
#ifdef DEBUG
		printf("mpu.c: Could not get DMP enabled.\n\r");
#endif
	}

//Enable DMP
	if (!enabled) {
		enabled = 1;
		status = mpu_set_dmp_state(enabled);
		if (status != XST_SUCCESS) {
#ifdef DEBUG
			printf("mpu.c: Could not enable DMP.\r\n");
#endif
			return XST_FAILURE;
		}
	}

//Enable Features
	status = dmp_enable_feature(FEATURES);
	if (status != XST_SUCCESS) {
#ifdef DEBUG
		printf("mpu.c: Error enabling desired features.\r\n");
#endif
		return XST_FAILURE;
	}

//Enable MPU FIFO
	status = mpu_configure_fifo(INV_XYZ_ACCEL | INV_XYZ_GYRO);
	if (status != XST_SUCCESS) {
#ifdef DEBUG
		printf("mpu.c: Error configuring FIFO.\n\r");
#endif
	}

//Set FIFO rate
	status = dmp_set_fifo_rate(DMP_FIFO_RATE);
	if (status != XST_SUCCESS) {
#ifdef DEBUG
		printf("mpu.c: Error Setting FIFO rate.\n\r");
#endif
	}

	//Enable Gyro Calibration
	status = dmp_enable_gyro_cal(enableGyroCalibration);
	if (status != XST_SUCCESS) {
#ifdef DEBUG
		printf("mpu.c: Could not enable Gyroscope Calibration.\n\r");
#endif
	}

////Get Interrupt
//	short int interrupt;
//	status = mpu_get_int_status(&interrupt);
//	if (status == XST_SUCCESS) {
//#ifdef DEBUG
//		printf("MPU Interrupt Status: 0x%x.\n\r", interrupt);
//#endif
//	}

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
#ifdef DEBUG
		xil_printf("mpu.c: Error in Setting IMU Address.\n\r");
#endif
		return XST_FAILURE;
	}

//2. Init IMU
	struct int_param_s param;
	status = mpu_init(&param);
	if (status != 0) {
#ifdef DEBUG
		xil_printf("mpu.c: Error initializing IMU\r\n.");
#endif
		return XST_FAILURE;
	}

//3. Select Sensors
	unsigned char sensors = SENSORS;
	status = mpu_set_sensors(sensors);
	if (status != 0) {
#ifdef DEBUG
		xil_printf("mpu.c: Error setting sensors.\r\n");
#endif
		return XST_FAILURE;
	}

//4. Set Sample Rate
	status = mpu_set_sample_rate(MPU_SAMPLE_RATE);
	if (status != 0) {
#ifdef DEBUG
		xil_printf("mpu.c: Error MPU sample rate.\r\n");
#endif
		return XST_FAILURE;
	}

//Sleep and Return
	sleep(1);
	return XST_SUCCESS;
}
