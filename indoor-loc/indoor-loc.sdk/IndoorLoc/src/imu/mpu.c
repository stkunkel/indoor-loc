/*
 * mpu.c: Setting up and getting information from MPU9150
 * Author: Stephanie Kunkel
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
int readFromFifo(short *gyro, short *accel, long *quat,
		unsigned long *timestamp, short *sensors, unsigned char *more);
int readFromRegs(short *gyro, short *accel, short* comp,
		unsigned long *timestamp, short *sensors);

/*
 * Variables
 */
static u8 imuAddr = 0;
static char dmpReady = 0;
static int count = 0;
float quat_conv[QUATERNION_AMOUNT];
static char gyrAccIsCal = 0;
static char gyroCalEnabled = 0;

/*
 * Print Drift w/o and with Calibration
 */
void printQuatDrift(unsigned int time_min) {
	//Variables
	int status;
	float quat_drift[4] = { 0.0, 0.0, 0.0, 0.0 };
	char calibration = 0;

	//w/o initial calibration
	status = getQuatDrift(quat_drift, calibration, time_min);
	if (status != XST_SUCCESS) {
		myprintf("Error in drift calculation without calibration\r\n.");
	} else {
		myprintf(
				"Quat Drift after %d minute(s) without initial calibration: %f %f %f %f\r\n",
				time_min, quat_drift[0], quat_drift[1], quat_drift[2],
				quat_drift[3]);
	}

	//w/ initial calibration
	calibration = 1;
	status = getQuatDrift(quat_drift, calibration, time_min);
	if (status != XST_SUCCESS) {
		myprintf("Error in drift calculation with calibration\r\n.");
	} else {
		myprintf(
				"Quat Drift after %d minute(s) with initial calibration: %f %f %f %f\r\n",
				time_min, quat_drift[0], quat_drift[1],
				quat_drift[2], quat_drift[3]);
	}

	//also enable dynamic calibration in DMP TODO: does it work?
	dmpGyroCalibration(1);
	status = getQuatDrift(quat_drift, calibration, time_min);
	if (status != XST_SUCCESS) {
		myprintf("Error in drift calculation with initial calibration and DMP dynamic calibration\r\n.");
	} else {
		myprintf(
				"Quat Drift after %d minutes with initial calibration and DMP dynamic calibration: %f %f %f %f\r\n",
				time_min, quat_drift[0], quat_drift[1],
				quat_drift[2], quat_drift[3]);
	}
}

/*
 * Get drift after x minutes
 * Returns: 0 if successful
 * In: time in minutes
 * Out: quat drift
 */
int getQuatDrift(float *quat_drift, char calibration, unsigned int time_min) {
//Variables
	XTime time_s = (XTime) (time_min * 60);
	XTime start, now;
	int status = XST_FAILURE, i;
	short gyro[NUMBER_OF_AXES], accel[NUMBER_OF_AXES];
	long quat[QUATERNION_AMOUNT];
	unsigned long timestamp;
	short int sensors;
	unsigned char* more = (unsigned char *) malloc(100);
	float quat_init[QUATERNION_AMOUNT];

//Calibrate if required
	if (calibration) {
		status = calibrateGyrAcc();
		if (status != XST_SUCCESS) {
			myprintf("mpu.c Could not calibrate Gyr. and Acc.\r\n");
			return status;
		}
	} else {
		//init and configure DMP
		if (!dmpReady) {
			status = configureDMP(FEATURES_RAW, DMP_FIFO_RATE);
			if (status != XST_SUCCESS) {
				myprintf("mpu.c Could initialize and configure DMP.\r\n");
				return status;
			}
		}
	}

	//Get initial quaternion
	do {
		status = readFromFifo(gyro, accel, quat, &timestamp, &sensors, more);
		usleep(100);
	} while (status != XST_SUCCESS);

	//Convert initial quaternion
	status = convertQuatenion(quat, quat_init);
	if (status != XST_SUCCESS) {
		myprintf("mpu.c: Could not convert initial quaternion.\r\n");
		return XST_FAILURE;
	}

//Get start time
	XTime_GetTime(&start);

//read until time is up
	do {
		//get current time
		XTime_GetTime(&now);

		//Get Quaternion
		status = readFromFifo(gyro, accel, quat, &timestamp, &sensors, more);
		if (status != XST_SUCCESS) {
			usleep(100);
			continue;
		}

	} while (now < start + time_s * COUNTS_PER_SECOND);

	//Convert quaternion drift
	status = convertQuatenion(quat, quat_drift);
	if (status != XST_SUCCESS) {
		myprintf("mpu.c: Could not convert final quaternion.\r\n");
		return XST_FAILURE;
	}

	//Compute difference
	for (i = 0; i < QUATERNION_AMOUNT; i++) {
		quat_drift[i] = quat_drift[i] - quat_init[i];
	}

	//Return
	free(more);
	return XST_SUCCESS;
}

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

//init and configure DMP
	if (!dmpReady) {
		status = configureDMP(FEATURES_CAL, DMP_FIFO_RATE);
		if (status != XST_SUCCESS) {
			return status;
		}
	}

//Get Data
	status = readFromFifo(gyro, accel, quat, &timestamp, &sensors, more);

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

//Free memory and return
	free(more);
	return status;
}

/*
 * Print Data using DMP
 */
int printDataWithDMP() {
//Variables
	int status, return_val = XST_SUCCESS;
	short gyro[NUMBER_OF_AXES], accel[NUMBER_OF_AXES], compass[NUMBER_OF_AXES];
	long temp_raw, quat[QUATERNION_AMOUNT];
	unsigned long timestamp;
	short int sensors;
	unsigned char* more = (unsigned char *) malloc(100);
	float conv[NUMBER_OF_AXES], temp_conv; //, quat_conv[QUATERNION_AMOUNT];

//init and configre DMP
	if (!dmpReady) {
		status = configureDMP(FEATURES_CAL, DMP_FIFO_RATE);
		if (status != XST_SUCCESS) {
			myprintf("mpu.c Error initializing DMP.\r\n");
			return XST_FAILURE;
		}
	}

//Get Data
	status = readFromFifo(gyro, accel, quat, &timestamp, &sensors, more);
	if (status != XST_SUCCESS) {
		return_val = XST_FAILURE;
	} else {

//Convert Gyro
		status = convertGyroData(gyro, conv);
		if (status != 0) {
			myprintf("mpu.c Error converting Gyroscope data.");
			return_val = XST_FAILURE;
		} else {

			//Print Gyro
			printGyro(conv);
		}
		printf(" | ");

//Convert Acc
		status = convertAccData(accel, conv);
		if (status != 0) {
			myprintf("mpu.c Error converting Acc data.");
			return_val = XST_FAILURE;
		} else {

			//Print Acc
			printAccel(conv);
		}
		printf(" | ");

//Get Compass
		status = mpu_get_compass_reg(compass, &timestamp);
		if (status != 0) {
			myprintf("mpu.c Error getting Compass data.");
			return_val = XST_FAILURE;
		} else {

			//Convert Compass
			status = convertCompassData(compass, conv);
			if (status != 0) {
				myprintf("mpu.c Error converting Compass data.");
				return_val = XST_FAILURE;
			} else {

				//Print Compass
				printCompass(conv);
			}
		}
		printf(" | ");

//Get Temperature
		status = mpu_get_temperature(&temp_raw, &timestamp);
		if (status != 0) {
			myprintf("mpu.c Error getting Temperature data.");
			return_val = XST_FAILURE;
		} else {

			//Convert Temperature
			status = convertTemperaturetoC(&temp_raw, &temp_conv);
			if (status != 0) {
				myprintf("mpu.c Error converting Temperature data.");
				return_val = XST_FAILURE;
			} else {

				//Print Temperature
				printTemp(&temp_conv);
			}
		}
//	printf(" | ");
//
////Convert Quaternion
//	status = convertQuatenion(quat, quat_conv);
//	if (status != 0) {
//		myprintf("mpu.c Error converting quaternion.");
//		return XST_FAILURE;
//	} else {
//		//Print Quaternion
//		printQuat(quat_conv);
//	}

//Print new line
		printf("\r\n");
	}

//Free Memory and Return
	free(more);
	return return_val;
}

/*
 * Print Data and don't use DMP
 */
int printDataNoDMP(short int *sensors) {
//Variables
	int status, return_val = XST_SUCCESS;
	short gyro[NUMBER_OF_AXES], accel[NUMBER_OF_AXES], compass[NUMBER_OF_AXES];
	unsigned long timestamp;
	long temp_raw;
	float conv[NUMBER_OF_AXES], temp_conv;
	unsigned long int temp_timestamp;

//Initialize IMU first if required
	if (imuAddr == 0) {
		status = initMPU();
		if (status != XST_SUCCESS) {
			myprintf("mpu.c: Error initializing IMU.\n\r");
			return XST_FAILURE;
		}
	}

//Get Data
	status = readFromRegs(gyro, accel, compass, &timestamp, sensors);

//Get Gyro
	if (status != XST_SUCCESS) {
		return_val = XST_FAILURE;
	} else {
		//Gyro
		if (sensors[0] & INV_XYZ_GYRO) {
			//Convert Gyro
			status = convertGyroData(gyro, conv);
			if (status != XST_SUCCESS) {
				myprintf("mpu.c Error converting Gyroscope data.");
				return_val = XST_FAILURE;
			} else {

				//Print Gyro
				printGyro(conv);
				printf(" | ");
			}
		}

		//Accel
		if (sensors[0] & INV_XYZ_ACCEL) {
			//Convert Acc
			status = convertAccData(accel, conv);
			if (status != XST_SUCCESS) {
				myprintf("mpu.c Error converting Acc data.");
				return_val = XST_FAILURE;
			} else {

				//Print Acc
				printAccel(conv);
				printf(" | ");
			}
		}

		//Compass
		if (sensors[0] & INV_XYZ_COMPASS) {
			//Convert Compass
			status = convertCompassData(compass, conv);
			if (status != XST_SUCCESS) {
				myprintf("mpu.c Error converting Compass data.");
				return_val = XST_FAILURE;
			} else {

				//Print Compass
				printCompass(conv);
				printf(" | ");
			}
		}
	}

//Get Temperature
	if (sensors[0] & SENSOR_TEMP) {
		status = mpu_get_temperature(&temp_raw, &temp_timestamp);
		if (status != XST_SUCCESS) {
			myprintf("mpu.c Error getting Temperature data.");
			return_val = XST_FAILURE;
		} else {

			//Convert Temperature
			status = convertTemperaturetoC(&temp_raw, &temp_conv);
			if (status != XST_SUCCESS) {
				myprintf("mpu.c Error converting Temperature data.");
				return_val = XST_FAILURE;
			} else {

				//Print Temperature
				printTemp(&temp_conv);
			}
		}
	}

//Print new line
	printf("\r\n");

//Return
	return return_val;
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
	myprintf("Quat: ");
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
 * Convert Gyroscope Data using Sensitivity
 */
int convertGyroData(short raw[NUMBER_OF_AXES], float converted[NUMBER_OF_AXES]) {
//Variables
	int status, i;
	float sensitivity;

//get sensitivity
	status = mpu_get_gyro_sens(&sensitivity);
	if (status != XST_SUCCESS) {
		myprintf("mpu.c: Error getting sensitivity of gyroscope.\r\n");
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
		myprintf("mpu.c: Error getting sensitivity of gyroscope.\r\n");
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
 * Calibrate Gyroscope and Accelerometer
 * Note: My Calibration is only for Gyro and Accel //TODO
 * Note2: MPU9150 has to sit on a flat surface, x axis (where LED is) facing upwards or downward
 */
int calibrateGyrAcc() {
//Variables
	int status = XST_FAILURE, i = 0, sample = 0;
	short gyro[NUMBER_OF_AXES], accel[NUMBER_OF_AXES], compass[NUMBER_OF_AXES];
	long int quat[4];
	unsigned long timestamp;
	short int sensors = INV_XYZ_GYRO | INV_XYZ_ACCEL;
	unsigned char* more = (unsigned char *) malloc(100);
	long gyro_bias[NUMBER_OF_AXES] = { 0, 0, 0 };
	long accel_bias[NUMBER_OF_AXES] = { 0, 0, 0 };
	float gyro_bias_conv[NUMBER_OF_AXES] = { 0.0, 0.0, 0.0 };
	float accel_bias_conv[NUMBER_OF_AXES] = { 0.0, 0.0, 0.0 };
	float gyro_bias_f[NUMBER_OF_AXES] = { 0.0, 0.0, 0.0 };
	float accel_bias_f[NUMBER_OF_AXES] = { 0.0, 0.0, 0.0 };

//Init MPU
	//Initialize IMU first if required
	if (imuAddr == 0) {
		status = initMPU();
		if (status != XST_SUCCESS) {
			gyrAccIsCal = 0;
			return status;
		}
	}

//Disable DMP
	status = mpu_set_dmp_state(0);
	if (status != XST_SUCCESS) {
		gyrAccIsCal = 0;
		myprintf("mpu.c: Could not disable DMP.\r\n");
		return status;
	}

//Clear FIFO
	for (i = 0; i < 10; i++) {
		status = readFromFifo(gyro, accel, quat, &timestamp, &sensors, more);
		if (status == XST_SUCCESS) {
			i--;
		}
	}

//Reset FIFO
	status = mpu_reset_fifo();
	if (status != XST_SUCCESS) {
		gyrAccIsCal = 0;
		myprintf("mpu.c: Could not reset FIFO.\r\n");
		return status;
	}

//Set Gyro Bias to 0
	status = mpu_set_gyro_bias_reg(gyro_bias);
	if (status != XST_SUCCESS) {
		gyrAccIsCal = 0;
		myprintf("mpu.c: Could not initially set gyro bias.\r\n");
		return XST_FAILURE;
	}

//Set Accel Bias to 0
	status = mpu_set_accel_bias_6050_reg(accel_bias);
	if (status != XST_SUCCESS) {
		gyrAccIsCal = 0;
		myprintf("mpu.c: Could not initially set accel bias.\r\n");
		return XST_FAILURE;
	}

//Compute offsets
	for (sample = 0; sample < CAL_SAMPLES; sample++) {
		//Get Data
		status = readFromRegs(gyro, accel, compass, &timestamp, &sensors);

		//Use only successful reads
		if (status != 0) {
			sample--;
		} else {
			//Convert accel
			status = convertAccData(accel, accel_bias_conv);
			if (status != XST_SUCCESS) {
				gyrAccIsCal = 0;
				return status;
			}

			//Convert gyro
			status = convertGyroData(gyro, gyro_bias_conv);
			if (status != XST_SUCCESS) {
				gyrAccIsCal = 0;
				return status;
			}

			//Compute Average
			for (i = 0; i < NUMBER_OF_AXES; i++) {
				gyro_bias_f[i] += (gyro_bias_conv[i] / CAL_SAMPLES);
				accel_bias_f[i] += (accel_bias_conv[i] / CAL_SAMPLES);
			}
		}
	}

//Make sure z axis acceleration (gravity) is not cancelled
	accel_bias_f[2] = 0.0;

//Convert biases
	for (i = 0; i < NUMBER_OF_AXES; i++) {
		gyro_bias[i] = (long) (gyro_bias_f[i] * GYRO_SENS_FRS_2);
		accel_bias[i] = (long) (accel_bias_f[i] / 2 * ACC_SENS_FRS_2);
	}

//Configure and enable DMP
	status = configureDMP(FEATURES_CAL, DMP_FIFO_RATE);
	if (status != XST_SUCCESS) {
		gyrAccIsCal = 0;
		return status;
	}

//Set status mask
	status = GYRO_CAL_MASK | ACCEL_CAL_MASK;

//Print Offsets
	myprintf("Gyro Bias: ");
	for (i = 0; i < NUMBER_OF_AXES; i++) {
		myprintf("%d ", gyro_bias[i]);
	}
	myprintf("\r\nAccel Bias: ");
	for (i = 0; i < NUMBER_OF_AXES; i++) {
		myprintf("%d ", accel_bias[i]);
	}
	myprintf("\r\n");

//Set calibrated flag
	if (status & GYRO_CAL_MASK && status & ACCEL_CAL_MASK) { //gyro and accel calibrated
		//Push accel bias to register
		status = mpu_set_accel_bias_6050_reg(accel_bias);
		if (status != XST_SUCCESS) {
			gyrAccIsCal = 0;
			return XST_FAILURE;
		}

		//Push Gyro Bias to register
		status = mpu_set_gyro_bias_reg(gyro_bias);
		if (status != XST_SUCCESS) {
			gyrAccIsCal = 0;
			return XST_FAILURE;
		}

		//Calibration successful --> set flag
		gyrAccIsCal = 1;
		status = XST_SUCCESS;
	} else {
		//Calibration successful --> set flag
		gyrAccIsCal = 0;
		status = XST_FAILURE;
	}

//Free memory and return
	free(more);
	sleep(3); //sleep to ensure all following readings are calibrated
	return status;
}

/*
 * Read from FIFO
 */
int readFromFifo(short *gyro, short *accel, long *quat,
		unsigned long *timestamp, short *sensors, unsigned char *more) {
//Variables
	int status = XST_FAILURE;

//Increase count
	count++;

//Make sure the read command is not called too often
	if (count >= 500) {
		//Reset Count, increment read count
		count = 0;

		//Read FIFO
		status = dmp_read_fifo(gyro, accel, quat, timestamp, sensors, more);
		if (status != XST_SUCCESS) {
			usleep(100); //sleep to prevent IIC bus from becoming busy
			//myprintf("mpu.c: Could not read DMP FIFO.\n\r");
		}
	}

//Return
	return status;
}

/*
 * Enable Gyro Calibration
 */
int dmpGyroCalibration(char enable) {
//Variables
	int status;

//Enable gyro cal if requested
	if (enable != gyroCalEnabled) {
		status = dmp_enable_gyro_cal(enable);
		if (status != XST_SUCCESS) {
			myprintf(
					"mpu.c: Could not enable/disable Gyroscope Calibration.\n\r");
		} else {
			gyroCalEnabled = enable;
		}
	}
	return status;
}

/*
 * Configure DMP
 */
int configureDMP(unsigned short int features, unsigned short fifoRate) {
//Variables
	int status;
	unsigned short fifo_mask = 0;

//Initialize IMU first if required
	if (imuAddr == 0) {
		status = initMPU();
		if (status != XST_SUCCESS) {
			return XST_FAILURE;
		}
	}

//Initialize DMP
	if (!dmpReady) {
		status = initDMP();
		if (status != XST_SUCCESS) {
			myprintf("mpu.c: Error initializing DMP.\r\n");
			return XST_FAILURE;
		}
	}

//Enable DMP
	status = mpu_set_dmp_state(1);
	if (status != XST_SUCCESS) {
		myprintf("mpu.c: Could not enable DMP.\r\n");
		return XST_FAILURE;
	}

//Enable Features
	status = dmp_enable_feature(features);
	if (status != XST_SUCCESS) {
		myprintf("mpu.c: Error enabling desired features.\r\n");
		return XST_FAILURE;
	}

//Set fifo mask according to features
	if (features & DMP_FEATURE_SEND_CAL_GYRO
			|| features & DMP_FEATURE_SEND_RAW_GYRO) {
		fifo_mask |= INV_XYZ_GYRO;
	}
	if (features & DMP_FEATURE_SEND_RAW_ACCEL) {
		fifo_mask |= INV_XYZ_ACCEL;
	}

//Enable MPU FIFO
	status = mpu_configure_fifo(fifo_mask);
	if (status != XST_SUCCESS) {
		myprintf("mpu.c: Error configuring FIFO.\n\r");
	}

//Set FIFO rate
	status = dmp_set_fifo_rate(fifoRate);
	if (status != XST_SUCCESS) {
		myprintf("mpu.c: Error Setting FIFO rate.\n\r");
	}

//Reset FIFO
	status = mpu_reset_fifo();
	if (status != XST_SUCCESS) {
		myprintf("mpu.c: Could not reset FIFO.\n\r");
	}

////Get Interrupt
//	short int interrupt;
//	status = mpu_get_int_status(&interrupt);
//	if (status == XST_SUCCESS) {
//#ifdef DEBUG
//		myprintf("MPU Interrupt Status: 0x%x.\n\r", interrupt);
//#endif
//	}

//Set flag, sleep and return
	dmpReady = 1;
	sleep(1);
	return XST_SUCCESS;
}

/*
 * Init DMP
 */
int initDMP() {
//Variables
	int status;
	long gyro_bias[NUMBER_OF_AXES] = { 0, 0, 0 };
	long accel_bias[NUMBER_OF_AXES] = { 0, 0, 0 };

//Check wheter DMP has already been initialized
	if (!dmpReady) {
		//Load Firmware and set flag
		status = dmp_load_motion_driver_firmware();
		if (status != XST_SUCCESS) {
			dmpReady = 0;
			myprintf("mpu.c: Error loading firmware of DMP.\r\n");
			return XST_FAILURE;
		}
	}

//Set Gyro and Accel Bias to 0
	status = dmp_set_gyro_bias(gyro_bias);
	if (status != XST_SUCCESS) {
		myprintf("mpu.c: Could not initially set gyro bias.\r\n");
	}

	status = dmp_set_accel_bias(accel_bias);
	if (status != XST_SUCCESS) {
		myprintf("mpu.c: Could not initially set accel bias.\r\n");
		return XST_FAILURE;
	}

//Enable DMP
	status = mpu_set_dmp_state(1);
	if (status != XST_SUCCESS) {
		myprintf("mpu.c: Could not enable DMP.\r\n");
		return XST_FAILURE;
	}

//Return
	return XST_SUCCESS;
}

/*
 * Read from Registers
 * OUT: gyro, accel, comp, timestamp
 * IN: sensors (INV_XYZ_GYRO for gyro, INV_XYZ_ACCEL for accel, INV_XYZ_COMPASS for compass)
 */
int readFromRegs(short *gyro, short *accel, short* comp,
		unsigned long *timestamp, short *sensors) {
//Variables
	int status, i;
	unsigned long ts_gyro = 0, ts_accel = 0, ts_comp = 0;

//Get Gyro
	if (sensors[0] & INV_XYZ_GYRO) {
		for (i = 0; i < NUMBER_OF_AXES; i++) {
			status = mpu_get_gyro_reg(gyro, &ts_gyro);
			if (status != XST_SUCCESS) {
				myprintf("mpu.c Error getting Gyroscope data.");
				return XST_FAILURE;
			}
		}
	}

//Get Acc
	if (sensors[0] & INV_XYZ_ACCEL) {
		status = mpu_get_accel_reg(accel, &ts_accel);
		if (status != XST_SUCCESS) {
			myprintf("mpu.c Error getting Acc data.");
			return XST_FAILURE;
		}
	}

//Get Compass
	if (sensors[0] & INV_XYZ_COMPASS) {
		status = mpu_get_compass_reg(comp, &ts_comp);
		if (status != XST_SUCCESS) {
			myprintf("mpu.c Error getting Compass data.");
			return XST_FAILURE;
		}
	}

//Make sure timestamp is the same
	if (ts_gyro) {
		*timestamp = ts_gyro;
	}

	if (ts_accel && *timestamp != ts_accel) {
		return XST_FAILURE;
	}

	if (ts_comp && *timestamp != ts_comp) {
		return XST_FAILURE;
	}

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
		myprintf("mpu.c: Error in Setting IMU Address.\n\r");
		return XST_FAILURE;
	}

//2. Init IMU
	struct int_param_s param;
	status = mpu_init(&param);
	if (status != 0) {
		myprintf("mpu.c: Error initializing IMU\r\n.");
		return XST_FAILURE;
	}

//3. Select Sensors
	unsigned char sensors = SENSORS_ALL;
	status = mpu_set_sensors(sensors);
	if (status != 0) {
		myprintf("mpu.c: Error setting sensors.\r\n");
		return XST_FAILURE;
	}

//4. Set Sample Rate
	status = mpu_set_sample_rate(MPU_SAMPLE_RATE);
	if (status != 0) {
		myprintf("mpu.c: Error MPU sample rate.\r\n");
		return XST_FAILURE;
	}

//Sleep and Return
	sleep(1);
	return XST_SUCCESS;
}