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
int printCurrentPositionForDisplay();
int printQuatForDisplay();
void printGyro(float conv[NUMBER_OF_AXES]);
void printAccel(float conv[NUMBER_OF_AXES]);
void printCompass(float conv[NUMBER_OF_AXES]);
void printTemp(float* temp_conv);
void printQuat(float quat[QUATERNION_AMOUNT]);
void printRotationAngle(long quat[QUATERNION_AMOUNT]);
void printEulerAngles(float* sigma, float* theta, float* psi);
void printPosition(Vector* position);
void printVelocity(Vector* velocity);
void updatePosition(float* accel_conv, float* quat_conv,
		unsigned long* p_timestamp, Vector* p_recentAccelInertial,
		Vector* p_recentVelocity, Vector* p_recentPosition,
		unsigned long* p_recent_ts);
int convertGyroData(short raw[NUMBER_OF_AXES], float converted[NUMBER_OF_AXES]);
int convertAccData(short raw[NUMBER_OF_AXES], float converted[NUMBER_OF_AXES]);
int convertCompassData(short raw[NUMBER_OF_AXES],
		float converted[NUMBER_OF_AXES]);
int convertTemperaturetoC(long* raw, float* converted);
int convertQuatenion(long raw[QUATERNION_AMOUNT], float conv[QUATERNION_AMOUNT]);
int pushBiasesToDMP(long* gyro_bias, long* accel_bias);
int readFromFifo(short *gyro, short *accel, long *quat,
		unsigned long *timestamp, short *sensors, unsigned char *more);
int readFromRegs(short *gyro, short *accel, short* comp,
		unsigned long *timestamp, short *sensors);

/*
 * Variables
 */
static u8 imuAddr = 0;
static bool dmpReady = BOOL_FALSE;
static bool gyrAccIsCal = BOOL_FALSE;
static long glob_gyro_bias[NUMBER_OF_AXES] = { 0, 0, 0 };
static long glob_accel_bias[NUMBER_OF_AXES] = { 0, 0, 0 };
static Vector recentVelocity = { .value[0] = 0.0, .value[1] = 0.0, .value[2
		] = 0.0 };
static Vector recentPosition = { .value[0] = 0.0, .value[1] = 0.0, .value[2
		] = 0.0 };
static unsigned long recent_ts = 0; //timestamp in ms
static float recentGyro[NUMBER_OF_AXES] = { 0.0, 0.0, 0.0 }, 		//dgr/s
		recentAccel[NUMBER_OF_AXES] = { 0.0, 0.0, 0.0 },			//g
		recentCompass[NUMBER_OF_AXES] = { 0.0, 0.0, 0.0 },			//uT
		recentQuat[QUATERNION_AMOUNT] = { 1.0, 0.0, 0.0, 0.0 };		//normalized
static float recentTemp = 0.0;										//dgrC
static Vector recentAccelInertial = { .value[0] = 0.0, .value[1] = 0.0, .value[2
		] = 0.0 };
static Vector normal_force = { .value[0] = 0.0, .value[1] = 0.0, .value[2
		] = 0.0 }; //measure in G

/*
 * Print for Display
 * Format: "<quat: w> <quat: x> <quat: y> <quat: z> | <pos: x> <pos: y> <pos: z>" (no new line)
 * You have to call updateData() first to get most recent value.
 * In: print mask
 * Returns 0 if successful
 */
int printforDisplay(short int *printMask, char* separator) {
	//Variables
	float gyro[NUMBER_OF_AXES], accel[NUMBER_OF_AXES], comp[NUMBER_OF_AXES],
			quat[QUATERNION_AMOUNT], temp;
	Vector velocity, position;
	short l_printMask;

	//Create Copy of recent data
	memcpy(&gyro, &recentGyro, NUMBER_OF_AXES * sizeof(float));
	memcpy(&accel, &recentAccel, NUMBER_OF_AXES * sizeof(float));
	memcpy(&comp, &recentCompass, NUMBER_OF_AXES * sizeof(float));
	memcpy(&quat, &recentQuat, QUATERNION_AMOUNT * sizeof(float));
	temp = recentTemp;
	velocity = recentVelocity;
	position = recentPosition;

	//Create Copy of Print Mask
	memcpy(&l_printMask, printMask, sizeof(short));

	//Print Timestamp
	myprintf("TS: %dms", recent_ts);
	myprintf(separator);

	//Print Gyroscope
	if (l_printMask & PRINT_GYRO) {
		//Print Velocity
		printGyro(gyro);

		//Adjust Print Mask
		l_printMask = l_printMask & ~PRINT_GYRO;

		//Print separator
		if (l_printMask != 0x00) {
			printf(separator);
		} else {
			return XST_SUCCESS;
		}
	}

	//Print Acceleration
	if (l_printMask & PRINT_ACCEL) {
		//Print Velocity
		printAccel(accel);

		//Adjust Print Mask
		l_printMask = l_printMask & ~PRINT_ACCEL;

		//Print separator
		if (l_printMask != 0x00) {
			printf(separator);
		} else {
			return XST_SUCCESS;
		}
	}

	//Print Compass
	if (l_printMask & PRINT_COMP) {
		//Print Velocity
		printCompass(comp);

		//Adjust Print Mask
		l_printMask = l_printMask & ~PRINT_COMP;

		//Print separator
		if (l_printMask != 0x00) {
			printf(separator);
		} else {
			return XST_SUCCESS;
		}
	}

	//Print Temperature
	if (l_printMask & PRINT_TEMP) {
		//Print Velocity
		printTemp(&temp);

		//Adjust Print Mask
		l_printMask = l_printMask & ~PRINT_TEMP;

		//Print separator
		if (l_printMask != 0x00) {
			printf(separator);
		} else {
			return XST_SUCCESS;
		}
	}

	//Print Quaternion
	if (l_printMask & PRINT_QUAT) {
		//Print Quat
		printQuat(quat);

		//Adjust Print Mask
		l_printMask = l_printMask & ~PRINT_QUAT;

		//Print separator
		if (l_printMask != 0x00) {
			printf(separator);
		} else {
			return XST_SUCCESS;
		}
	}

	//Print Velocity
	if (l_printMask & PRINT_VEL) {
		//Print Velocity
		printVelocity(&velocity);

		//Adjust Print Mask
		l_printMask = l_printMask & ~PRINT_VEL;

		//Print separator
		if (l_printMask != 0x00) {
			printf(separator);
		} else {
			return XST_SUCCESS;
		}
	}

	//Print Position
	if (l_printMask & PRINT_POS) {
		printPosition(&position);
	} else {
		return XST_SUCCESS;
	}

	//Didnt print anything
	return XST_FAILURE;
}

/*
 * Print Current Position for Display
 * (You have to call updateData() to get most recent value.)
 */
int printCurrentPositionForDisplay() {
//Variables
	int status = XST_SUCCESS;
	Vector position;

//Create copy of recent position
	position = recentPosition;

//Print
	if (status == XST_SUCCESS) {
		printPosition(&position);
	}

//Return
	return status;
}

/*
 * Print Current Velocity for Display
 * (You have to call updateData() to get most recent value.)
 */
int printCurrentVelocityForDisplay() {
//Variables
	int status = XST_SUCCESS;
	Vector velocity;

//Create copy of recent position
	velocity = recentVelocity;

//Print
	if (status == XST_SUCCESS) {
		printVelocity(&velocity);
	}

//Return
	return status;
}

/*
 * Print Current Acceleration for Display
 * (You have to call updateData() to get most recent value.)
 */
int printCurrentAccelForDisplay() {
//Variables
	int status = XST_SUCCESS;
	float accel[NUMBER_OF_AXES];

//Create copy of recent position
	memcpy(accel, recentAccel, NUMBER_OF_AXES * sizeof(float));

//Print
	if (status == XST_SUCCESS) {
		printAccel(accel);
	}

//Return
	return status;
}

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
				time_min, quat_drift[0], quat_drift[1], quat_drift[2],
				quat_drift[3]);
	}

//also enable dynamic calibration in DMP TODO: does it work?
	dmpGyroCalibration(1);
	status = getQuatDrift(quat_drift, calibration, time_min);
	if (status != XST_SUCCESS) {
		myprintf(
				"Error in drift calculation with initial calibration and DMP dynamic calibration\r\n.");
	} else {
		myprintf(
				"Quat Drift after %d minutes with initial calibration and DMP dynamic calibration: %f %f %f %f\r\n",
				time_min, quat_drift[0], quat_drift[1], quat_drift[2],
				quat_drift[3]);
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
	unsigned char* more = (unsigned char *) malloc(100 * sizeof(char));
	float quat_init[QUATERNION_AMOUNT];

//Calibrate if required
	if (calibration) {
		status = calibrateGyrAcc(CAL_SAMPLES);
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
 * (You have to call updateData() to get most recent value.)
 */
int printQuatForDisplay() {
//Variables
	int status = XST_FAILURE, i = 0;
	float quat[QUATERNION_AMOUNT];

//Create Copy of recent Quat
	memcpy(&quat, &recentQuat, QUATERNION_AMOUNT * sizeof(float));

//Print Quaternion
	myprintf("Quat: ");
	if (status == XST_SUCCESS) {
		for (i = 0; i < QUATERNION_AMOUNT; i++) {
			printf("%f ", quat[i]);
		}
	}

//Return
	return status;
}

/*
 * Print Data using DMP
 */
void printDataWithDMP(short int *sensors, char* separator) {
	//Variables
	float gyro[NUMBER_OF_AXES], accel[NUMBER_OF_AXES], compass[NUMBER_OF_AXES];
	float temp;

	//Create Copy of recent values
	memcpy(&gyro, &recentGyro, NUMBER_OF_AXES * sizeof(float));
	memcpy(&accel, &recentAccel, NUMBER_OF_AXES * sizeof(float));
	memcpy(&compass, &recentCompass, NUMBER_OF_AXES * sizeof(float));
	temp = recentTemp;

	//Print Gyro
	if (sensors[0] & INV_XYZ_GYRO) {
		printGyro(gyro);

		//Print Separator
		if ((sensors[0] & INV_XYZ_ACCEL) || (sensors[0] & INV_XYZ_COMPASS)
				|| (sensors[0] & SENSOR_TEMP)) {
			printf(separator);
		}
	}

	//Print Acc
	if (sensors[0] & INV_XYZ_ACCEL) {
		printAccel(accel);

		//Print Separator
		if ((sensors[0] & INV_XYZ_COMPASS) || (sensors[0] & SENSOR_TEMP)) {
			printf(separator);
		}
	}

	//Print Compass
	if (sensors[0] & INV_XYZ_COMPASS) {
		printCompass(compass);

		//Print Separator
		if (sensors[0] & SENSOR_TEMP) {
			printf(separator);
		}
	}

	//Print Temperature
	if (sensors[0] & SENSOR_TEMP) {
		printTemp(&temp);
	}
}

/*
 * Print Data and don't use DMP
 */
int printDataNoDMP(short int *sensors, char* separator) {
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
				printf(separator);
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
				printf(separator);
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
				printf(separator);
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
	myprintf("Gyro: ");
	for (i = 0; i < NUMBER_OF_AXES; i++) {
		printf("%+.2f", conv[i]);
		myprintf("dgr/s");
		if (i < NUMBER_OF_AXES - 1) {
			printf(" ");
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
	myprintf("Acc: ");
	for (i = 0; i < NUMBER_OF_AXES; i++) {
		printf("%+.2f", conv[i]);
		myprintf("g");
		if (i < NUMBER_OF_AXES - 1) {
			printf(" ");
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
	myprintf("Comp: ");
	for (i = 0; i < NUMBER_OF_AXES; i++) {
		printf("%+.1f", conv[i]);
		myprintf("uT");
		if (i < NUMBER_OF_AXES - 1) {
			printf(" ");
		}
	}
}

/*
 * Convert and Print Temperature Data  (already converted)
 */
void printTemp(float* temp_conv) {
//Print Temperature
	myprintf("Temp: ");
	printf("%+.2f", *temp_conv);
	myprintf("dgrC");
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
		printf(" %+.6f", quat[i]); //Print space so IMU Viewer can distinguish negative sign of first value from other characters
//		if (i < QUATERNION_AMOUNT - 1) {
//			printf(" ");
//		}
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
 * Print Position
 */
void printPosition(Vector* position) {
//Variables
	int i;

//Print
	myprintf("Pos: ");
	for (i = 0; i < NUMBER_OF_AXES; i++) {
		printf("%+.1f", position->value[i]);
		myprintf("m");
		if (i < NUMBER_OF_AXES - 1) {
			printf(" ");
		}
	}
}

/*
 * Print Velocity
 */
void printVelocity(Vector* velocity) {
//Variables
	int i;

//Print
	myprintf("Vel: ");
	for (i = 0; i < NUMBER_OF_AXES; i++) {
		printf("%+.2f", velocity->value[i] * 100);
		myprintf("m/s");
		if (i < NUMBER_OF_AXES - 1) {
			printf(" ");
		}
	}
}

/*
 * Test Position Update function
 */
void testPositionUpdate() {
//Variables
	int i, cnt = 0;
	float l_accel_conv[NUMBER_OF_AXES] = { 0.0, 0.0, 0.0 };
	float l_quat_conv[QUATERNION_AMOUNT] = { 1.0, 0.0, 0.0, 0.0 };
	unsigned long l_timestamp = 0;
	unsigned long l_recent_ts = 0;
	Vector l_recentAccelInertial = { .value[0] = 0.0, .value[1] = 0.0, .value[2
			] = 0.0 };
	Vector l_recentVelocity = { .value[0] = 0.0, .value[1] = 0.0, .value[2
			] = 0.0 };
	Vector l_recentPosition = { .value[0] = 0.0, .value[1] = 0.0, .value[2
			] = 0.0 };
	float normal_force_measured[NUMBER_OF_AXES] = { 0.0, 0.0, 1.0 };

	// Test Parameters
	// Acceleration in Y_AXIS;
	// Rotation around X_AXIS;
	// Gravity in Z_AXIS
	const short acc_number = 5;
	float test_acc[] = { 0.0, 1.0, -1.0, 0.0, 0.0 };

	// Set gravity after calibration
	normal_force.value[X_AXIS] = 0.0;
	normal_force.value[Y_AXIS] = 0.0;
	normal_force.value[Z_AXIS] = 1.0;

	// Test 1: No Rotation
	myprintf("Test 1: No Rotation:\r\n");
	l_quat_conv[0] = 1.0;
	l_quat_conv[1] = 0.0;
	l_quat_conv[2] = 0.0;
	l_quat_conv[3] = 0.0;

	// Acc in all axes
	for (i = 0; i < NUMBER_OF_AXES; i++) {
		myprintf("Axis: %d:\r\n", i);

		//Set measured gravity
		memcpy(&normal_force_measured, &normal_force,
		NUMBER_OF_AXES * sizeof(float));
		memcpy(&l_accel_conv, &normal_force_measured,
		NUMBER_OF_AXES * sizeof(float));

		//Reset Values
		l_recentAccelInertial.value[0] = 0.0;
		l_recentAccelInertial.value[1] = 0.0;
		l_recentAccelInertial.value[2] = 0.0;
		l_recentVelocity.value[0] = 0.0;
		l_recentVelocity.value[1] = 0.0;
		l_recentVelocity.value[2] = 0.0;
		l_recentPosition.value[0] = 0.0;
		l_recentPosition.value[1] = 0.0;
		l_recentPosition.value[2] = 0.0;

		//Move
		for (cnt = 0; cnt < acc_number; cnt++) {
			l_accel_conv[i] = normal_force_measured[i] + test_acc[cnt];
			l_timestamp += 1000 / DMP_FIFO_RATE;

			//Update and Print
			updatePosition(l_accel_conv, l_quat_conv, &l_timestamp,
					&l_recentAccelInertial, &l_recentVelocity,
					&l_recentPosition, &l_recent_ts);
			printPosition(&l_recentPosition);
			myprintf("\r\n");
		}
	}

	// Test 2: Rotation: 90 and 180dgr
	myprintf("Test 2a: Rotation 90dgr:\r\n");
	l_quat_conv[0] = 0.7071067811865475;
	l_quat_conv[1] = 0.7071067811865475;
	l_quat_conv[2] = 0.0;
	l_quat_conv[3] = 0.0;

	//Set measured gravity
	normal_force_measured[X_AXIS] = 0.0;
	normal_force_measured[Y_AXIS] = 1.0;
	normal_force_measured[Z_AXIS] = 0.0;
	memcpy(&l_accel_conv, &normal_force_measured,
	NUMBER_OF_AXES * sizeof(float));

	//Reset Values
	l_recentAccelInertial.value[0] = 0.0;
	l_recentAccelInertial.value[1] = 0.0;
	l_recentAccelInertial.value[2] = 0.0;
	l_recentVelocity.value[0] = 0.0;
	l_recentVelocity.value[1] = 0.0;
	l_recentVelocity.value[2] = 0.0;
	l_recentPosition.value[0] = 0.0;
	l_recentPosition.value[1] = 0.0;
	l_recentPosition.value[2] = 0.0;

	//Acc in Y_AXIS
	i = Y_AXIS;
	myprintf("Axis: %d:\r\n", i);

	// Move
	for (cnt = 0; cnt < acc_number; cnt++) {
		l_accel_conv[i] = normal_force_measured[i] + test_acc[cnt];
		l_timestamp += 1000 / DMP_FIFO_RATE;

		// Update and print
		updatePosition(l_accel_conv, l_quat_conv, &l_timestamp,
				&l_recentAccelInertial, &l_recentVelocity, &l_recentPosition,
				&l_recent_ts);
		printPosition(&l_recentPosition);
		myprintf("\r\n");
	}

	//2b: Rotation 180dgr
	myprintf("Test 2b: Rotation 180dgr:\r\n");
	l_quat_conv[0] = 0.0;
	l_quat_conv[1] = 1.0;
	l_quat_conv[2] = 0.0;
	l_quat_conv[3] = 0.0;

	//Set measured gravity
	normal_force_measured[X_AXIS] = 0.0;
	normal_force_measured[Y_AXIS] = 0.0;
	normal_force_measured[Z_AXIS] = -1.0;
	memcpy(&l_accel_conv, &normal_force_measured,
	NUMBER_OF_AXES * sizeof(float));

	//Reset Values
	l_recentAccelInertial.value[0] = 0.0;
	l_recentAccelInertial.value[1] = 0.0;
	l_recentAccelInertial.value[2] = 0.0;
	l_recentVelocity.value[0] = 0.0;
	l_recentVelocity.value[1] = 0.0;
	l_recentVelocity.value[2] = 0.0;
	l_recentPosition.value[0] = 0.0;
	l_recentPosition.value[1] = 0.0;
	l_recentPosition.value[2] = 0.0;

	//Acc in Y_AXIS
	i = Y_AXIS;
	myprintf("Axis: %d:\r\n", i);

	// Move
	for (cnt = 0; cnt < acc_number; cnt++) {
		l_accel_conv[i] = normal_force_measured[i] + test_acc[cnt];
		l_timestamp += 1000 / DMP_FIFO_RATE;

		//Update and Print
		updatePosition(l_accel_conv, l_quat_conv, &l_timestamp,
				&l_recentAccelInertial, &l_recentVelocity, &l_recentPosition,
				&l_recent_ts);
		printPosition(&l_recentPosition);
		myprintf("\r\n");
	}

	// Test 3a: Rotation: 45dgr
	myprintf("Test 3a: Rotation 45dgr:\r\n");
	l_quat_conv[0] = 0.9238795325112867;
	l_quat_conv[1] = 0.3826834323650897;
	l_quat_conv[2] = 0.0;
	l_quat_conv[3] = 0.0;

	//Set measured gravity
	normal_force_measured[X_AXIS] = 0.0;
	normal_force_measured[Y_AXIS] = 0.7071067811865475;
	normal_force_measured[Z_AXIS] = 0.7071067811865475;
	memcpy(&l_accel_conv, &normal_force_measured,
	NUMBER_OF_AXES * sizeof(float));

	//Reset Values
	l_recentAccelInertial.value[0] = 0.0;
	l_recentAccelInertial.value[1] = 0.0;
	l_recentAccelInertial.value[2] = 0.0;
	l_recentVelocity.value[0] = 0.0;
	l_recentVelocity.value[1] = 0.0;
	l_recentVelocity.value[2] = 0.0;
	l_recentPosition.value[0] = 0.0;
	l_recentPosition.value[1] = 0.0;
	l_recentPosition.value[2] = 0.0;

	//Acc in Y_AXIS
	i = Y_AXIS;
	myprintf("Axis: %d:\r\n", i);

	// Move
	for (cnt = 0; cnt < acc_number; cnt++) {
		l_accel_conv[i] = normal_force_measured[i] + test_acc[cnt];
		l_timestamp += 1000 / DMP_FIFO_RATE;

		//Update and Print
		updatePosition(l_accel_conv, l_quat_conv, &l_timestamp,
				&l_recentAccelInertial, &l_recentVelocity, &l_recentPosition,
				&l_recent_ts);
		printPosition(&l_recentPosition);
		myprintf("\r\n");
	}

	// Test 3b: Rotation: 1dgr
	myprintf("Test 3: Rotation 1dgr:\r\n");
	l_quat_conv[0] = 0.9999619230641712;
	l_quat_conv[1] = 0.008726535498373935;
	l_quat_conv[2] = 0.0;
	l_quat_conv[3] = 0.0;

	//Set measured gravity
	normal_force_measured[X_AXIS] = 0.0;
	normal_force_measured[Y_AXIS] = sin(degToRad(1.0));
	normal_force_measured[Z_AXIS] = cos(degToRad(1.0));
	memcpy(&l_accel_conv, &normal_force_measured,
	NUMBER_OF_AXES * sizeof(float));

	//Reset Values
	l_recentAccelInertial.value[0] = 0.0;
	l_recentAccelInertial.value[1] = 0.0;
	l_recentAccelInertial.value[2] = 0.0;
	l_recentVelocity.value[0] = 0.0;
	l_recentVelocity.value[1] = 0.0;
	l_recentVelocity.value[2] = 0.0;
	l_recentPosition.value[0] = 0.0;
	l_recentPosition.value[1] = 0.0;
	l_recentPosition.value[2] = 0.0;

	//Acc in Y_AXIS
	i = Y_AXIS;
	myprintf("Axis: %d:\r\n", i);

	// Move
	for (cnt = 0; cnt < acc_number; cnt++) {
		l_accel_conv[i] = normal_force_measured[i] + test_acc[cnt];
		l_timestamp += 1000 / DMP_FIFO_RATE;

		//Update and Print
		updatePosition(l_accel_conv, l_quat_conv, &l_timestamp,
				&l_recentAccelInertial, &l_recentVelocity, &l_recentPosition,
				&l_recent_ts);
		printPosition(&l_recentPosition);
		myprintf("\r\n");
	}
}

/*
 * Update Sensor Data and Position
 * Returns 0 if successful
 */
int updateData() {
//Variables
	int i, status = XST_SUCCESS;
	short gyro[NUMBER_OF_AXES], accel[NUMBER_OF_AXES], compass[NUMBER_OF_AXES],
			sensors;
	long quat[QUATERNION_AMOUNT], temperature;
	unsigned long timestamp, temp_ts;
	unsigned char* more = (unsigned char *) malloc(100 * sizeof(char));
	float gyro_conv[NUMBER_OF_AXES], accel_conv[NUMBER_OF_AXES],
			compass_conv[NUMBER_OF_AXES], quat_conv[QUATERNION_AMOUNT],
			temp_conv;
	;

//Get latest data
	do {
		status = readFromFifo(gyro, accel, quat, &timestamp, &sensors, more);
		usleep(100);
	} while (status != XST_SUCCESS);

//Free memory
	free(more);

//Get Compass //TODO: Handle unequal timestamp
	status = mpu_get_compass_reg(compass, &temp_ts);
	if (status != 0) {
		//Print
		myprintf("mpu.c Error getting Compass data.");

		//Set compass data invalid
		for (i = 0; i < NUMBER_OF_AXES; i++) {
			compass[i] = -1;
		}
	}

//Get Temperature //TODO: Handle unequal timestamp
	status = mpu_get_temperature(&temperature, &temp_ts);
	if (status != 0) {
		myprintf("mpu.c Error getting Temperature data.");
		temperature = -1;
	}

//Convert Gyro
	status = convertGyroData(gyro, gyro_conv);
	if (status != XST_SUCCESS) {
		return status;
	}

//Convert Accel
	status = convertAccData(accel, accel_conv);
	if (status != XST_SUCCESS) {
		return status;
	}

//Convert Compass
	status = convertCompassData(compass, compass_conv);
	if (status != XST_SUCCESS) {
		return status;
	}

//Convert Quat
	status = convertQuatenion(quat, quat_conv);
	if (status != XST_SUCCESS) {
		return status;
	}

//Convert Temp
	status = convertTemperaturetoC(&temperature, &temp_conv);
	if (status != XST_SUCCESS) {
		return status;
	}

//Update Position and timestamps
	updatePosition(accel_conv, quat_conv, &timestamp, &recentAccelInertial,
			&recentVelocity, &recentPosition, &recent_ts);

//Update Sensor Data
	memcpy(&recentGyro, &gyro_conv, NUMBER_OF_AXES * sizeof(float));
	memcpy(&recentAccel, &accel_conv, NUMBER_OF_AXES * sizeof(float));
	memcpy(&recentCompass, &compass_conv, NUMBER_OF_AXES * sizeof(float));
	memcpy(&recentQuat, &quat_conv, QUATERNION_AMOUNT * sizeof(float));

//Update Temperature
	recentTemp = temp_conv;

//Return
	return status;
}

/*
 * Update Position (also updates timestamps)
 */
void updatePosition(float* accel_conv, float* quat_conv,
		unsigned long* p_timestamp, Vector* p_recentAccelInertial,
		Vector* p_recentVelocity, Vector* p_recentPosition,
		unsigned long* p_recent_ts) {
//Variables
	Vector accel_measuremt, normal_force_rot, accel_inertial, velocity,
			newPosition;
	Matrix rotation, rotation_inv;
	float time_diff;
	int i;

//	//DEBUG
//	quat_conv[0] = 1.0;
//	quat_conv[1] = 0.0;
//	quat_conv[2] = 0.0;
//	quat_conv[3] = 0.0;
//	//DEBUG END

//Create measured accel vector
	accel_measuremt = toVector(accel_conv);

//Create rotation matrix
	rotation = toRotationMatrix(quat_conv);

//Get inverse of rotation matrix
	//rotation_inv = multMatrixByScalar(rotation, 1);
	//rotation_inv = multMatrixByScalar(rotation, -1);
	rotation_inv = getTranspose(rotation);
	//getInverseOfMatrix(&rotation, &rotation_inv);

	//Rotate normal force
	normal_force_rot = multMatrixAndVector(rotation, normal_force);

	//Remove gravity
	accel_measuremt = addVectors(accel_measuremt,
			multVectorByScalar(normal_force_rot, -1));

//Compute Inertial Acceleration Vector
	accel_inertial = multMatrixAndVector(rotation_inv, accel_measuremt); //rotation_inv

//Convert 1g to 9.81 m/s^2
	accel_inertial = multVectorByScalar(accel_inertial, GRAVITY);

//Handle first function call
	if (*p_recent_ts == 0) {
		//Set recent timestamp
		*p_recent_ts = *p_timestamp - (1000 / DMP_FIFO_RATE);

		//Set recent velocity and speed
		for (i = 0; i < NUMBER_OF_AXES; i++) {
			p_recentVelocity->value[i] = 0.0;
			p_recentPosition->value[i] = 0.0;
		}
	}

//Get time difference in s
	time_diff = (*p_timestamp - *p_recent_ts) / 1000.0;

//Compute Velocity
	computeVelocity(p_recentVelocity, &accel_inertial, time_diff, &velocity);

//Compute Position
	computePosition(p_recentPosition, p_recentVelocity, &accel_inertial,
			time_diff, &newPosition);

//Set new Values
	*p_recentAccelInertial = accel_inertial;
	*p_recentVelocity = velocity;
	*p_recentPosition = newPosition;
	*p_recent_ts = *p_timestamp;
}

/*
 * Quaternion Test
 */
void quaternionTest() {
	//Variables
	int i;
	float quat[17 * QUATERNION_AMOUNT] = { 1.000000000, 0.000000000,
			0.000000000, 0.000000000, 0.923879533, 0.382683432, 0.000000000,
			0.000000000, 0.707106781, 0.707106781, 0.000000000, 0.000000000,
			0.923879533, 0.382683432, 0.000000000, 0.000000000, 1.000000000,
			0.000000000, 0.000000000, 0.000000000, 0.923879533, 0.000000000,
			0.382683432, 0.000000000, 0.707106781, 0.000000000, 0.707106781,
			0.000000000, 0.923879533, 0.000000000, 0.382683432, 0.000000000,
			1.000000000, 0.000000000, 0.000000000, 0.000000000, 0.923879533,
			0.000000000, 0.000000000, 0.382683432, 0.707106781, 0.000000000,
			0.000000000, 0.707106781, 0.923879533, 0.000000000, 0.000000000,
			0.382683432, 1.000000000, 0.000000000, 0.000000000, 0.000000000,
			0.923879533, 0.270598050, 0.270598050, 0.000000000, 0.707106781,
			0.500000000, 0.500000000, 0.000000000, 0.923879533, 0.270598050,
			0.270598050, 0.000000000, 1.000000000, 0.000000000, 0.000000000,
			0.000000000 };

	//Print
	for (i = 0; i < 17; i++) {
		printQuat(&quat[i * QUATERNION_AMOUNT]);
		printf("\r\n");
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
		myprintf("mpu.c: Error getting sensitivity of gyroscope.\r\n");
		return XST_FAILURE;
	}

//convert data
	for (i = 0; i < NUMBER_OF_AXES; i++) {
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
	for (i = 0; i < NUMBER_OF_AXES; i++) {
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
	for (i = 0; i < NUMBER_OF_AXES; i++) {
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
 * In: number of samples used for calibration
 */
int calibrateGyrAcc(unsigned int samples) {
//Variables
	int status = XST_FAILURE;
	unsigned int i = 0, sample = 0;
	short gyro[NUMBER_OF_AXES], accel[NUMBER_OF_AXES], compass[NUMBER_OF_AXES];
	unsigned long timestamp;
	short int sensors = INV_XYZ_GYRO | INV_XYZ_ACCEL;
	unsigned char* more = (unsigned char *) malloc(100 * sizeof(char));
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
			gyrAccIsCal = BOOL_FALSE;
			return status;
		}
	}

//Disable DMP
	status = mpu_set_dmp_state(0);
	if (status != XST_SUCCESS) {
		gyrAccIsCal = BOOL_FALSE;
		myprintf("mpu.c: Could not disable DMP.\r\n");
		return status;
	}

//Set Gyro Bias to 0
	status = mpu_set_gyro_bias_reg(gyro_bias);
	if (status != XST_SUCCESS) {
		gyrAccIsCal = BOOL_FALSE;
		myprintf("mpu.c: Could not initially set gyro bias.\r\n");
		return XST_FAILURE;
	}

//Set Accel Bias to 0
	status = mpu_set_accel_bias_6050_reg(accel_bias);
	if (status != XST_SUCCESS) {
		gyrAccIsCal = BOOL_FALSE;
		myprintf("mpu.c: Could not initially set accel bias.\r\n");
		return XST_FAILURE;
	}

//Reset FIFO
	status = mpu_reset_fifo();
	if (status != XST_SUCCESS) {
		gyrAccIsCal = BOOL_FALSE;
		myprintf("mpu.c: Could not reset FIFO.\r\n");
		return status;
	}

//Compute offsets
	for (sample = 0; sample < samples; sample++) {
		//Get Data
		status = readFromRegs(gyro, accel, compass, &timestamp, &sensors);

		//Use only successful reads
		if (status != 0) {
			sample--;
		} else {
			//Convert accel
			status = convertAccData(accel, accel_bias_conv);
			if (status != XST_SUCCESS) {
				gyrAccIsCal = BOOL_FALSE;
				return status;
			}

			//Convert gyro
			status = convertGyroData(gyro, gyro_bias_conv);
			if (status != XST_SUCCESS) {
				gyrAccIsCal = BOOL_FALSE;
				return status;
			}

			//Compute Average
			for (i = 0; i < NUMBER_OF_AXES; i++) {
				gyro_bias_f[i] += (gyro_bias_conv[i] / samples);
				accel_bias_f[i] += (accel_bias_conv[i] / samples);
			}
		}
	}

	//Make sure gravity is 1.0
////Save gravity for later
//	normal_force.value[GRAVITY_AXIS] = 1.0;
//
////Make sure gravity is not cancelled
//	accel_bias_f[GRAVITY_AXIS] = -1.0 + accel_bias_f[GRAVITY_AXIS];

	//Use gravity as it is
	//Save gravity for later
	normal_force.value[GRAVITY_AXIS] = accel_bias_f[GRAVITY_AXIS];

	//Make sure gravity is not cancelled
	accel_bias_f[GRAVITY_AXIS] = 0.0;

//Convert biases
	for (i = 0; i < NUMBER_OF_AXES; i++) {
		gyro_bias[i] = (long) (gyro_bias_f[i] * GYRO_SENS_FRS_2);
		accel_bias[i] = (long) (accel_bias_f[i] / 2 * ACC_SENS_FRS_2);
	}

	//Save biases for later
	memcpy(&glob_gyro_bias, &gyro_bias, NUMBER_OF_AXES * sizeof(long));
	memcpy(&glob_accel_bias, &accel_bias, NUMBER_OF_AXES * sizeof(long));

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
			gyrAccIsCal = BOOL_FALSE;
			return XST_FAILURE;
		}

		//Push Gyro Bias to register
		status = mpu_set_gyro_bias_reg(gyro_bias);
		if (status != XST_SUCCESS) {
			gyrAccIsCal = BOOL_FALSE;
			return XST_FAILURE;
		}

		//Calibration successful --> set flag
		gyrAccIsCal = BOOL_TRUE;
		status = XST_SUCCESS;
	} else {
		//Calibration successful --> set flag
		gyrAccIsCal = BOOL_FALSE;
		status = XST_FAILURE;
	}

	//Configure and enable DMP
	status = configureDMP(FEATURES_CAL, DMP_FIFO_RATE);
	if (status != XST_SUCCESS) {
		return status;
	}

//Print Success
	if (status != XST_SUCCESS) {
		myprintf("Calibration failed.\r\n");
		return status;
	} else {
		myprintf("Calibration done.\r\n");
	}

//Sleep
//sleep(1);

//Free memory and return
	free(more);
	return status;
}

/*
 * Push calibration results to DMP
 * In: gyro and accel biases in q1 format
 * Returns 0 if successful
 */
int pushBiasesToDMP(long* gyro_bias, long* accel_bias) {
	//Variables
	int status;

	//Set Gyro and Accel Bias
	status = dmp_set_gyro_bias(gyro_bias);
	if (status != XST_SUCCESS) {
		myprintf("mpu.c: Could not initially set gyro bias.\r\n");
	}

	status = dmp_set_accel_bias(accel_bias);
	if (status != XST_SUCCESS) {
		myprintf("mpu.c: Could not initially set accel bias.\r\n");
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}

/*
 * Read from FIFO
 */
int readFromFifo(short *gyro, short *accel, long *quat,
		unsigned long *timestamp, short *sensors, unsigned char *more) {
//Variables
	int status = XST_FAILURE;

//Increase count
//	count++;

//Make sure the read command is not called too often
//	if (count >= 500) { //was: 500
//	Reset Count, increment read count
//	count = 0;

//Read FIFO
	status = dmp_read_fifo(gyro, accel, quat, timestamp, sensors, more);
//		if (status != XST_SUCCESS) {
//	usleep(1000); //sleep to prevent IIC bus from becoming busy (was: 100, worked ok with 10000)
//	myprintf("mpu.c: Could not read DMP FIFO.\n\r");
//		}
//	}

//Return
	return status;
}

/*
 * Get FIFO Count
 * Returns DMP FIFO Count
 */
int getFifoCount() {
//Variables
	int size;
	unsigned char* data = (unsigned char*) malloc(100 * sizeof(char));

//Read FIFO Count Registers
	imuI2cRead(imuAddr, 0x72, 2, data);

//Convert
	size = (data[0] << 8) | data[1];

//Free memory and return
	free(data);
	return size;
}

/*
 * Enable Gyro Calibration
 */
int dmpGyroCalibration(bool enable) {
//Variables
	int status = XST_SUCCESS;
	char enableCal = 0;

	//Set Enable Variable
	if (enable == BOOL_TRUE) {
		enableCal = 1;
	} else {
		enableCal = 0;
	}

	//Enable gyro cal if requested
	status = dmp_enable_gyro_cal(enableCal);

	if (status != XST_SUCCESS) {
		myprintf("mpu.c: Could not enable/disable Gyroscope Calibration.\n\r");
	}

	//Return
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

//Push Biases to DMP if calibration is finished
	if (gyrAccIsCal == BOOL_TRUE) {
		status = pushBiasesToDMP(glob_gyro_bias, glob_accel_bias);
		if (status != XST_SUCCESS) {
			myprintf("mpu.c: Unable to push biases to DMP.\r\n");
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

//Set flag, sleep and return
	dmpReady = 1;
//sleep(1); //don't remove or readings will be weird
	return XST_SUCCESS;
}

/*
 * Init DMP
 */
int initDMP() {
//Variables
	int status;

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

//Enable DMP
	status = mpu_set_dmp_state(1);
	if (status != XST_SUCCESS) {
		myprintf("mpu.c: Could not enable DMP.\r\n");
		return XST_FAILURE;
	}

//Enable Interrupt
//	status = setupMPUInt();
//	if (status != XST_SUCCESS) {
//		myprintf("mpu.c: Could not set up MPU interrupt.\r\n");
//		return XST_FAILURE;
//	}

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
	int status;
	unsigned long ts_gyro = 0, ts_accel = 0, ts_comp = 0;

//Get Gyro
	if (sensors[0] & INV_XYZ_GYRO) {
//		for (i = 0; i < NUMBER_OF_AXES; i++) {
		status = mpu_get_gyro_reg(gyro, &ts_gyro);
		if (status != XST_SUCCESS) {
			myprintf("mpu.c Error getting Gyroscope data.");
			return XST_FAILURE;
		}
//		}
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

//Make sure timestamp is the same TODO
	if (ts_gyro != 0) {
		*timestamp = ts_gyro;
	}
//
//	if ((ts_accel != 0) && (*timestamp != ts_accel)) {
//		return XST_FAILURE;
//	}
//
//	if ((ts_comp) && (*timestamp != ts_comp)) {
//		return XST_FAILURE;
//	}

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
		myprintf("mpu.c: Error setting MPU sample rate.\r\n");
		return XST_FAILURE;
	}

//5. Set sensitivities
//Gyro
	status = mpu_set_gyro_fsr(GYRO_SENS);
	if (status != 0) {
		myprintf("mpu.c: Error setting gyroscope FRS.\r\n");
		return XST_FAILURE;
	}
//Accel
	status = mpu_set_accel_fsr(ACCEL_SENS);
	if (status != 0) {
		myprintf("mpu.c: Error setting gyroscope FRS.\r\n");
		return XST_FAILURE;
	}

//Sleep and Return
	return XST_SUCCESS;
}
