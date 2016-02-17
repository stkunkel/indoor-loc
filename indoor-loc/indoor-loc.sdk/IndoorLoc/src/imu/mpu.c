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
		Vector* p_recentAccelInertial, Vector* p_recentVelocity,
		Vector* p_recentPosition, float* time_diff);
int convertGyroData(short raw[NUMBER_OF_AXES], float converted[NUMBER_OF_AXES]);
int convertAccData(short raw[NUMBER_OF_AXES], float converted[NUMBER_OF_AXES]);
int convertCompassData(short raw[NUMBER_OF_AXES],
		float converted[NUMBER_OF_AXES]);
int convertTemperaturetoC(long* raw, float* converted);
int convertQuatenion(long raw[QUATERNION_AMOUNT], float conv[QUATERNION_AMOUNT]);
int pushBiasesToDMP(long* gyro_bias, long* accel_bias);
int readFromFifo(short *gyro, short *accel, long *quat,
		unsigned long *timestamp, short *sensors, unsigned char *more);
void computeQuaternion(float gyro[NUMBER_OF_AXES], float accel[NUMBER_OF_AXES],
		float* delta_t, float quat[QUATERNION_AMOUNT]);
int initDMP();
int configureMpuFifo(unsigned char fifoMask);
int initMPU();
void initVar();

/*
 * Variables
 */
static u8 imuAddr = 0;
static bool dmpReady = BOOL_FALSE;
static bool gyrAccIsCal = BOOL_FALSE;
static long glob_gyro_bias[NUMBER_OF_AXES];
static long glob_accel_bias[NUMBER_OF_AXES];
static Vector recentVelocity;
static Vector recentPosition;
static unsigned long recent_ts = 0; //timestamp in ms
static float recentGyro[NUMBER_OF_AXES], 		//dgr/s
		recentAccel[NUMBER_OF_AXES],			//g
		recentCompass[NUMBER_OF_AXES],			//uT
		recentQuat[QUATERNION_AMOUNT];		//normalized
static float recentTemp = 0.0;										//dgrC
static Vector recentAccelInertial;
static Vector normal_force; //measure in G

/*
 * Print for Display
 * Format: "<quat: w> <quat: x> <quat: y> <quat: z> | <pos: x> <pos: y> <pos: z>" (no new line)
 * You have to call updateData() first to get most recent value.
 * In: print mask
 * Returns 0 if successful
 */
int printforDisplay(short printMask, char* separator) {
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
	l_printMask = printMask;

	//Print Timestamp
	myprintf("TS: %dms", recent_ts);
	myprintf(separator);

	//Print Gyroscope
	if (l_printMask & PRINT_GYRO) {
		//Print
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
		//Print
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
		//Print
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
		//Print
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
		//Print
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
		//Print
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
		//Print
		printPosition(&position);

		//Adjust Print Mask
		l_printMask = l_printMask & ~PRINT_POS;

		//Print separator

	}

	//Didnt print anything
	if (l_printMask == PRINT_NOTHING) {
		return XST_SUCCESS;
	} else {
		return XST_FAILURE;
	}
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
void printDataWithDMP(short sensors, char* separator) {
	//Variables
	float gyro[NUMBER_OF_AXES], accel[NUMBER_OF_AXES], compass[NUMBER_OF_AXES];
	float temp;

	//Create Copy of recent values
	memcpy(&gyro, &recentGyro, NUMBER_OF_AXES * sizeof(float));
	memcpy(&accel, &recentAccel, NUMBER_OF_AXES * sizeof(float));
	memcpy(&compass, &recentCompass, NUMBER_OF_AXES * sizeof(float));
	temp = recentTemp;

	//Print Gyro
	if (sensors & INV_XYZ_GYRO) {
		printGyro(gyro);

		//Print Separator
		if ((sensors & INV_XYZ_ACCEL) || (sensors & INV_XYZ_COMPASS)
				|| (sensors & SENSOR_TEMP)) {
			printf(separator);
		}
	}

	//Print Acc
	if (sensors & INV_XYZ_ACCEL) {
		printAccel(accel);

		//Print Separator
		if ((sensors & INV_XYZ_COMPASS) || (sensors & SENSOR_TEMP)) {
			printf(separator);
		}
	}

	//Print Compass
	if (sensors & INV_XYZ_COMPASS) {
		printCompass(compass);

		//Print Separator
		if (sensors & SENSOR_TEMP) {
			printf(separator);
		}
	}

	//Print Temperature
	if (sensors & SENSOR_TEMP) {
		printTemp(&temp);
	}
}

/*
 * Print Data and don't use DMP
 */
int printDataNoDMP(short sensors, char* separator) {
//Variables
	int status, return_val = XST_SUCCESS;
	short gyro[NUMBER_OF_AXES], accel[NUMBER_OF_AXES], compass[NUMBER_OF_AXES];
	unsigned long timestamp;
	long temp_raw;
	float conv[NUMBER_OF_AXES], temp_conv;

//Initialize IMU first if required
	if (imuAddr == 0) {
		status = initMPU();
		if (status != XST_SUCCESS) {
			myprintf("mpu.c: Error initializing IMU.\n\r");
			return XST_FAILURE;
		}
	}

//Get Data
	status = readFromRegs(gyro, accel, compass, &temp_raw, &timestamp, sensors);
	if (status == XST_DEVICE_BUSY){
		return status;
	}

//Get Gyro
	if (status != XST_SUCCESS) {
		return_val = XST_FAILURE;
	} else {
		//Gyro
		if (sensors & INV_XYZ_GYRO) {
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
		if (sensors & INV_XYZ_ACCEL) {
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
		if (sensors & INV_XYZ_COMPASS) {
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

		//Temperature
		if (sensors & SENSOR_TEMP) {
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
 * Print Temperature Data  (already converted)
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
		printf(" %+f", quat[i]); //Print space so IMU Viewer can distinguish negative sign of first value from other characters
//		if (i < QUATERNION_AMOUNT - 1) {
//			printf(" ");
//		}
	}
}

/*
 * Print Raw Gyro/Accel/Compass Data
 * In: Raw sensor data
 */
void printRaw(short sensor[NUMBER_OF_AXES]) {
	//Variables
	int i;

	//Print
	for (i = 0; i < NUMBER_OF_AXES; i++) {
		printf("%d ", sensor[i]);
		if (i < NUMBER_OF_AXES - 1) {
			printf(" ");
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
 * Print Position (cm)
 */
void printPosition(Vector* position) {
//Variables
	int i;

//Print
	myprintf("Pos: ");
	for (i = 0; i < NUMBER_OF_AXES; i++) {
		printf("%+2.4f", position->value[i]);
		myprintf("m");
		if (i < NUMBER_OF_AXES - 1) {
			printf(" ");
		}
	}
}

/*
 * Print Velocity (m/s)
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
 * Get Recent Data
 * Supply NULL Pointer if data is not required
 * In: Pointers for gyro, accel, compass, temperature, quaternion, velocity, position, timestamp
 * Out: Recent gyro, accel, compass, temperature, quaternion, velocity, position, timestamp
 */
void getRecent(float gyro[NUMBER_OF_AXES], float accel[NUMBER_OF_AXES],
		float compass[NUMBER_OF_AXES], float* temperature,
		float quat[QUATERNION_AMOUNT], float* velocity, float* position,
		unsigned long* timestamp) {
	//Gyro
	if (gyro != 0) {
		memcpy(gyro, recentGyro, NUMBER_OF_AXES * sizeof(float));
	}

	//Accel
	if (accel != 0) {
		memcpy(accel, recentAccel, NUMBER_OF_AXES * sizeof(float));
	}

	//Compass
	if (compass != 0) {
		memcpy(compass, recentCompass, NUMBER_OF_AXES * sizeof(float));
	}

	//Temp
	if (temperature != 0) {
		*temperature = recentTemp;
	}

	//Quat
	if (quat != 0) {
		memcpy(quat, recentQuat, QUATERNION_AMOUNT * sizeof(float));
	}

	//Velocity
	if (velocity != 0) {
		memcpy(velocity, recentVelocity.value, NUMBER_OF_AXES * sizeof(float));
	}

	//Position
	if (position != 0) {
		memcpy(position, recentPosition.value, NUMBER_OF_AXES * sizeof(float));
	}

	//Timestamp
	if (timestamp != 0) {
		*timestamp = recent_ts;
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
	float time_diff = 1.0 / FIFO_RATE; //in s
	Vector l_recentAccelInertial = { .value[0] = 0.0, .value[1] = 0.0, .value[2
			] = 0.0 };
	Vector l_recentVelocity = { .value[0] = 0.0, .value[1] = 0.0, .value[2
			] = 0.0 };
	Vector l_recentPosition = { .value[0] = 0.0, .value[1] = 0.0, .value[2
			] = 0.0 };
	float normal_force_measured[NUMBER_OF_AXES] = { 0.0, 0.0, 1.0 };

	//Init Global Variables
	initVar();

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

			//Update and Print
			updatePosition(l_accel_conv, l_quat_conv, &l_recentAccelInertial,
					&l_recentVelocity, &l_recentPosition, &time_diff);
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

		// Update and print
		updatePosition(l_accel_conv, l_quat_conv, &l_recentAccelInertial,
				&l_recentVelocity, &l_recentPosition, &time_diff);
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

		//Update and Print
		updatePosition(l_accel_conv, l_quat_conv, &l_recentAccelInertial,
				&l_recentVelocity, &l_recentPosition, &time_diff);
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

		//Update and Print
		updatePosition(l_accel_conv, l_quat_conv, &l_recentAccelInertial,
				&l_recentVelocity, &l_recentPosition, &time_diff);
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

		//Update and Print
		updatePosition(l_accel_conv, l_quat_conv, &l_recentAccelInertial,
				&l_recentVelocity, &l_recentPosition, &time_diff);
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
	long temperature;
	unsigned long timestamp;
	float gyro_conv[NUMBER_OF_AXES], accel_conv[NUMBER_OF_AXES],
			compass_conv[NUMBER_OF_AXES], quat_conv[QUATERNION_AMOUNT],
			temp_conv;
	float time_diff = 1.0 / FIFO_RATE;

//Variables for computing position only
#ifndef DISABLE_POS_COMP
	float quat_new[QUATERNION_AMOUNT];
#endif

//Variables for DMP only
#ifdef USE_DMP
	long quat[QUATERNION_AMOUNT];
	unsigned char* more = (unsigned char *) malloc(100 * sizeof(char));
	unsigned long temp_ts;
#endif

//Get latest data
// With DMP
#ifdef USE_DMP
	do {
		status = readFromFifo(gyro, accel, quat, &timestamp, &sensors, more);
		usleep(100);
		if (status == XST_DEVICE_BUSY) {
			return status;
		}
	}while (status != XST_SUCCESS);

	//Free memory
	free(more);

	if (status == XST_SUCCESS) {
		//Get Compass //TODO: Handle unequal timestamp
		status = mpu_get_compass_reg(compass, &temp_ts);
		if (status != XST_SUCCESS) {
			//Print
			myprintf("mpu.c Error getting Compass data.");

			//Set compass data invalid
			for (i = 0; i < NUMBER_OF_AXES; i++) {
				compass[i] = -1;
			}
		}

		//Get Temperature //TODO: Handle unequal timestamp
		status = mpu_get_temperature(&temperature, &temp_ts);
		if (status != XST_SUCCESS) {
			myprintf("mpu.c Error getting Temperature data.");
			temperature = -1;
		}
	}

// Without DMP
#else
	//Read Data from Registers
	sensors = SENSORS_ALL;
	do {
		status = readFromRegs(gyro, accel, compass, &temperature, &timestamp,
				sensors);

		//Check status
		if (status == XST_DEVICE_BUSY) {
			return status;
		}
	} while (status != XST_SUCCESS);
#endif

	//Update time difference
	time_diff = (timestamp - recent_ts) / 1000.0;

	//Handle first function call
	if (recent_ts == 0) {
		//Set recent timestamp (time diff is in s, so it has to be converted to ms)
		recent_ts = timestamp - (long) (1000 * time_diff);

		//Set recent velocity and speed
		for (i = 0; i < NUMBER_OF_AXES; i++) {
			recentVelocity.value[i] = 0.0;
			recentPosition.value[i] = 0.0;
		}
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

//Convert Temp
	status = convertTemperaturetoC(&temperature, &temp_conv);
	if (status != XST_SUCCESS) {
		return status;
	}
#ifndef DISABLE_POS_COMP
#ifdef USE_DMP
	//Convert Quaternion if using DMP
	status = convertQuatenion(quat, quat_conv);
	if (status != XST_SUCCESS) {
		return status;
	}
#else
	//Compute Quaternion quat_new if not using DMP
	computeQuaternion(gyro_conv, accel_conv, &time_diff, quat_new);

	//Update rotation (quat_conv = recentQuat * quat_new)
	multiplyQuaternions(recentQuat, quat_new, quat_conv);
#endif

//Update Position and Velocity
	updatePosition(accel_conv, quat_conv, &recentAccelInertial, &recentVelocity,
			&recentPosition, &time_diff);

#endif

//Update Timestamp
	recent_ts = timestamp;

//Update Sensor Data
	memcpy(&recentGyro, &gyro_conv, NUMBER_OF_AXES * sizeof(float));
	memcpy(&recentAccel, &accel_conv, NUMBER_OF_AXES * sizeof(float));
	memcpy(&recentCompass, &compass_conv, NUMBER_OF_AXES * sizeof(float));
	memcpy(&recentQuat, &quat_conv, QUATERNION_AMOUNT * sizeof(float));

//Update Temperature
	recentTemp = temp_conv;

//LED Run
	ledRun();

//Return
	return status;
}

/*
 * Update Position (also updates timestamps)
 * In: pointers accelerations, quaternion, recent acceleration, recent velocity, recent position, time difference to last sample
 */
void updatePosition(float* accel_conv, float* quat_conv,
		Vector* p_recentAccelInertial, Vector* p_recentVelocity,
		Vector* p_recentPosition, float* time_diff) {

	//Variables
	Vector accel_measuremt, normal_force_rot, accel_inertial, velocity,
			newPosition;
	Matrix rotation, rotation_inv;
//	float quat_inv[QUATERNION_AMOUNT];

	//Create measured accel vector
	accel_measuremt = toVector(accel_conv);

	///////////////QUAT
//	Vector normal_force_rot2, accel_measuremt2, accel_inertial2;
//
//	//Get Quaternion Conjugate
//	quatInverse(quat_conv, quat_inv);
//
//	//Rotate normal force
//	normal_force_rot2 = rotateVector(&normal_force, quat_conv);
//
//	//Remove gravity
//	accel_measuremt2 = addVectors(accel_measuremt,
//			multVectorByScalar(normal_force_rot2, -1));
//
//	//Compute Inertial Acceleration Vector
//	accel_inertial2 = rotateVector(&accel_measuremt2, quat_inv);

	//////////////Rotation Matrix
	//Create rotation matrix
	rotation = toRotationMatrix(quat_conv);

	//Get inverse of rotation matrix
	rotation_inv = getTranspose(rotation);

	//Rotate normal force
	normal_force_rot = multMatrixAndVector(rotation, normal_force);

	//Remove gravity
	accel_measuremt = addVectors(accel_measuremt,
			multVectorByScalar(normal_force_rot, -1));

	//Compute Inertial Acceleration Vector
	accel_inertial = multMatrixAndVector(rotation_inv, accel_measuremt); //rotation_inv

	//Convert 1g to 9.81 m/s^2
	accel_inertial = multVectorByScalar(accel_inertial, GRAVITY);

	//Compute Velocity
	computeVelocity(p_recentVelocity, &accel_inertial, *time_diff, &velocity);

	//Compute Position
	computePosition(p_recentPosition, p_recentVelocity, &accel_inertial,
			*time_diff, &newPosition);

	//Set new Values
	*p_recentAccelInertial = accel_inertial;
	*p_recentVelocity = velocity;
	*p_recentPosition = newPosition;
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
		converted[i] = raw[i] * 1.0 / sensitivity;
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
	long temp;
//	unsigned long timestamp;
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
	status = imuSetDmpState(0);
	if (status != XST_SUCCESS) {
		gyrAccIsCal = BOOL_FALSE;
		myprintf("mpu.c: Could not disable DMP.\r\n");
		return status;
	}

//Set Gyro Bias to 0
	status = imuSetGyroBias(gyro_bias);
	if (status != XST_SUCCESS) {
		gyrAccIsCal = BOOL_FALSE;
		myprintf("mpu.c: Could not initially set gyro bias.\r\n");
		return status;
	}

//Set Accel Bias to 0
	status = imuSetAccelBias(accel_bias);
	if (status != XST_SUCCESS) {
		gyrAccIsCal = BOOL_FALSE;
		myprintf("mpu.c: Could not initially set accel bias.\r\n");
		return status;
	}

//Reset FIFO
	status = imuResetFifo();
	if (status != XST_SUCCESS) {
		gyrAccIsCal = BOOL_FALSE;
		myprintf("mpu.c: Could not reset FIFO.\r\n");
		return status;
	}

	//Read out some data sets before sampling starts
	for (sample = 0; sample < CAL_IGNORE_SAMPLES; sample++) {
		//Read Sensor
		status = readFromRegs(gyro, accel, compass, &temp, 0, sensors);

		//Check status
		if (status != XST_SUCCESS) {
			if (status == XST_DEVICE_BUSY) {
				return status;
			} else {
				sample--;
			}
		}
	}

//Compute offsets
	for (sample = 0; sample < samples; sample++) {
		//Get Data
		status = readFromRegs(gyro, accel, compass, &temp, 0, sensors); //&timestamp

		//Use only successful reads
		if (status != XST_SUCCESS) {
			if (status == XST_DEVICE_BUSY) {
				return status;
			} else {
				sample--;
			}
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
//	normal_force.value[GRAVITY_AXIS] = accel_bias_f[GRAVITY_AXIS];
	memcpy(&normal_force, &accel_bias_f, NUMBER_OF_AXES * sizeof(float));

//Convert biases
	for (i = 0; i < NUMBER_OF_AXES; i++) {
		gyro_bias[i] = (long) (gyro_bias_f[i] * GYRO_SENS_FRS_2);
		accel_bias[i] = (long) (accel_bias_f[i] / 2 * ACC_SENS_FRS_2);
	}

	//Print Offsets
	myprintf("Gyro Bias: ");
	for (i = 0; i < NUMBER_OF_AXES; i++) {
		printf("%ld ", gyro_bias[i]); //TODO
	}
	myprintf("\r\nAccel Bias: ");
	for (i = 0; i < NUMBER_OF_AXES; i++) {
		printf("%ld ", accel_bias[i]); //TODO
	}
	printf("\r\n"); //TODO

	//Make sure gravity is not cancelled
	accel_bias_f[GRAVITY_AXIS] = 0.0;
	accel_bias[GRAVITY_AXIS] = 0;

	//Save biases for later
	memcpy(&glob_gyro_bias, &gyro_bias, NUMBER_OF_AXES * sizeof(long));
	memcpy(&glob_accel_bias, &accel_bias, NUMBER_OF_AXES * sizeof(long));

//Set status mask
	status = GYRO_CAL_MASK | ACCEL_CAL_MASK;

//Set calibrated flag
	if (status & GYRO_CAL_MASK && status & ACCEL_CAL_MASK) { //gyro and accel calibrated
		//Push accel bias to register
		status = imuSetAccelBias(accel_bias);
		if (status != XST_SUCCESS) {
			gyrAccIsCal = BOOL_FALSE;
			return status;
		}

		//Push Gyro Bias to register
		status = imuSetGyroBias(gyro_bias);
		if (status != XST_SUCCESS) {
			gyrAccIsCal = BOOL_FALSE;
			return status;
		}

		//Set Recent Gyro and Accel
		memcpy(&recentGyro, &gyro_bias_f, NUMBER_OF_AXES * sizeof(float));
		memcpy(&recentAccel, &normal_force, NUMBER_OF_AXES * sizeof(float));

		//Calibration successful --> set flag
		gyrAccIsCal = BOOL_TRUE;
		status = XST_SUCCESS;
	} else {
		//Calibration successful --> set flag
		gyrAccIsCal = BOOL_FALSE;
		status = XST_FAILURE;
	}
#ifdef USE_DMP
	//Configure and enable DMP
	status = configureDMP(FEATURES_CAL);
	if (status != XST_SUCCESS) {
		return status;
	}
#endif

//Print Success
	if (status != XST_SUCCESS) {
		myprintf("Calibration failed.\r\n");
		return status;
	} else {
		myprintf("Calibration done.\r\n");
	}

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
	status = imuSetDmpGyroBias(gyro_bias);
	if (status != XST_SUCCESS) {
		myprintf("mpu.c: Could not initially set gyro bias.\r\n");
		return status;
	}

	status = imuSetDmpAccelBias(accel_bias);
	if (status != XST_SUCCESS) {
		myprintf("mpu.c: Could not initially set accel bias.\r\n");
		return status;
	}

	return XST_SUCCESS;
}

/*
 * Read from FIFO
 * In: Pointers to gyro, accel, quat, timestamp, sensors and more
 * Returns 0 if successful
 */
int readFromFifo(short *gyro, short *accel, long *quat,
		unsigned long *timestamp, short *sensors, unsigned char *more) {
//Variables
	int status = XST_FAILURE;

//Read FIFO
	status = imuDmpReadFifo(gyro, accel, quat, timestamp, sensors, more);

//Return
	return status;
}

/*
 * Enable Gyro Calibration
 * In: bool whether to enable or disable
 * Returns 0 if successful
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
	status = imuEnableDmpGyroCal(enableCal);
	if (status != XST_SUCCESS) {
		myprintf("mpu.c: Could not enable/disable Gyroscope Calibration.\n\r");
	}

	//Return
	return status;
}

/*
 * Initialize IMU
 * In: calibration time in s
 * Returns 0 if successful
 */
int initIMU(unsigned int calibrationTime) {
	//Variables
	int status, sample;
	short gyro[NUMBER_OF_AXES], accel[NUMBER_OF_AXES], compass[NUMBER_OF_AXES];
	long temp;

	//Init MPU
	myprintf(".........Initialize MPU...........\n\r");
	status = initMPU();
	if (status != XST_SUCCESS) {
		return status;
	}

#ifdef USE_DMP
	//Init DMP
	myprintf(".........Initialize DMP...........\n\r");
	status = initDMP();
	if (status != XST_SUCCESS) {
		return status;
	}

	//Configure DMP
	myprintf(".........Configure DMP...........\n\r");
	status = configureDMP(FEATURES_RAW);
	if (status != XST_SUCCESS) {
		return status;
	}
#endif

	//Calibrate
#ifdef INITIAL_CALIBRATION
	myprintf(".........Calibrate...........\n\r");
	status = calibrateGyrAcc(calibrationTime * FIFO_RATE);
	if (status != XST_SUCCESS) {
		return status;
	}

#endif

	//Read some values to make sure upcoming values will be useful
	for (sample = 0; sample < CAL_IGNORE_SAMPLES; sample++) {
		//Read Sensor
		status = readFromRegs(gyro, accel, compass, &temp, 0, SENSORS_ALL);
		if (status != XST_SUCCESS) {
			if (status == XST_DEVICE_BUSY) {
				return status;
			} else {
				sample--;
			}
		}
	}

	//Setup Interrupt
	status = setupInt();
	if (status != XST_SUCCESS) {
		return status;
	}

	//Return
	return status;
}

/*
 * Configure DMP
 * In: feature mask
 * DMP_FEATURE_TAP
 * DMP_FEATURE_ANDROID_ORIENT
 * DMP_FEATURE_LP_QUAT
 * DMP_FEATURE_6X_LP_QUAT
 * DMP_FEATURE_GYRO_CAL
 * DMP_FEATURE_SEND_RAW_ACCEL
 * DMP_FEATURE_SEND_RAW_GYRO
 * NOTE: DMP_FEATURE_LP_QUAT and DMP_FEATURE_6X_LP_QUAT are mutually
 * Returns 0 if successful
 */
int configureDMP(unsigned short int features) {
//Variables
	int status;
	unsigned short fifoMask = 0;

//Initialize IMU first if required
	if (imuAddr == 0) {
		status = initMPU();
		if (status != XST_SUCCESS) {
			return status;
		}
	}

//Initialize DMP
	if (dmpReady == BOOL_FALSE) {
		status = initDMP();
		if (status != XST_SUCCESS) {
			myprintf("mpu.c: Error initializing DMP.\r\n");
			return status;
		}
	}

#ifdef INITIAL_CALIBRATION
//Push Biases to DMP if calibration is finished
	if (gyrAccIsCal == BOOL_TRUE) {
		status = pushBiasesToDMP(glob_gyro_bias, glob_accel_bias);
		if (status != XST_SUCCESS) {
			myprintf("mpu.c: Unable to push biases to DMP.\r\n");
		}
	}
#endif

//Enable DMP
	status = imuSetDmpState(1);
	if (status != XST_SUCCESS) {
		myprintf("mpu.c: Could not enable DMP.\r\n");
		if (status == XST_DEVICE_BUSY) {
			exit(0);
		} else {
			return status;
		}
	}

//Enable Features
	status = imuEnableDmpFeatures(features);
	if (status != XST_SUCCESS) {
		myprintf("mpu.c: Error enabling desired features.\r\n");
		if (status == XST_DEVICE_BUSY) {
			exit(0);
		} else {
			return status;
		}
	}

//Set fifo mask according to features
	if (features & DMP_FEATURE_SEND_CAL_GYRO
			|| features & DMP_FEATURE_SEND_RAW_GYRO) {
		fifoMask |= INV_XYZ_GYRO;
	}
	if (features & DMP_FEATURE_SEND_RAW_ACCEL) {
		fifoMask |= INV_XYZ_ACCEL;
	}

	//Set FIFO rate
	status = imuSetFifoRate(FIFO_RATE);
	if (status != XST_SUCCESS) {
		myprintf("mpu.c: Error Setting FIFO rate.\n\r");
		if (status == XST_DEVICE_BUSY) {
			exit(0);
		} else {
			return status;
		}
	}

	//Configure MPU FIFO
	status = configureMpuFifo(fifoMask);
	if (status != XST_SUCCESS) {
		myprintf("mpu.c: Error configuring FIFO.\n\r");
		if (status == XST_DEVICE_BUSY) {
			exit(0);
		} else {
			return status;
		}
	}

//Return
	return XST_SUCCESS;
}

/*
 * Init DMP
 * Returns 0 if successful
 */
int initDMP() {
//Variables
	int status;

//Check wheter DMP has already been initialized
	if (dmpReady == BOOL_FALSE) {
		//Load Firmware and set flag
		status = imuDmpLoadMotionDriverFirmware();
		if (status != XST_SUCCESS) {
			dmpReady = BOOL_FALSE;
			myprintf("mpu.c: Error loading firmware of DMP.\r\n");

			//Check whether IIC Device is busy
			if (status == XST_DEVICE_BUSY) {
				exit(0);
			} else {
				return status;
			}
		}
	}

//Enable DMP
	status = imuSetDmpState(1);
	if (status != XST_SUCCESS) {
		myprintf("mpu.c: Could not enable DMP.\r\n");
		return status;
	}

//Set flag
	dmpReady = BOOL_TRUE;

//Return
	return XST_SUCCESS;
}

/*
 * Test Quaternion Computation
 */
void testQuaternionComputation() {
	//Variables
	int i;
	float gyro[NUMBER_OF_AXES] = { 0.0, 0.0, 0.0 };
	float gyro_x[5] = { 0.0, 90.0, 90.0, -180.0, 0.0 };
	float quat[QUATERNION_AMOUNT] = { 1.0, 0.0, 0.0, 0.0 };
	float quat_gyro[QUATERNION_AMOUNT] = { 1.0, 0.0, 0.0, 0.0 };
	float quat_new[QUATERNION_AMOUNT] = { 1.0, 0.0, 0.0, 0.0 };
	float delta_t = 1;

	for (i = 0; i < 5; i++) {
		//Set x value
		gyro[0] = gyro_x[i];

		//Compute Quaternion
		computeQuaternion(gyro, normal_force.value, &delta_t, quat_gyro);

		//Rotate
		multiplyQuaternions(quat, quat_gyro, quat_new);
		memcpy(&quat, &quat_new, QUATERNION_AMOUNT * sizeof(float));

		//Print
		printf("Angle x: %+3.2f, \t Quat: %+1.6f %+1.6f %+1.6f %+1.6f\r\n",
				gyro_x[i], quat[0], quat[1], quat[2], quat[3]);
	}
}
/*
 * Compute Quatenternion
 * Sources: https://www.dropbox.com/s/87pl9lgv4bfubne/Incremental%20Quaternion%20from%203D%20Rotational%20Vector%20Derivation.pdf?dl=0
 * 			http://philstech.blogspot.de/2015/06/complimentary-filter-example-quaternion.html
 * In: gyro data in dgr/s, accel data in g, time difference to last sample
 * Out: quaternion
 */
void computeQuaternion(float gyro[NUMBER_OF_AXES], float accel[NUMBER_OF_AXES],
		float* delta_t, float quat[QUATERNION_AMOUNT]) {
	//Variables
	int i;
	float gyro_rad[NUMBER_OF_AXES] = { 0.0, 0.0, 0.0 };
	float gyro_mag = 0, rot_angle = 0;

//	//Complementary Filter
//	float scale = 0.5;
//	float quat_inv[QUATERNION_AMOUNT];
//	Vector accel_meas = { .value[0] = 0.0, .value[1] = 0.0, .value[2] = 0.0 };
//	Vector accel_inertial =
//			{ .value[0] = 0.0, .value[1] = 0.0, .value[2] = 0.0 };
//	Vector corr = { .value[0] = 0.0, .value[1] = 0.0, .value[2] = 0.0 };
//	Vector gyro_corr = { .value[0] = 0.0, .value[1] = 0.0, .value[2] = 0.0 };
//
//	//Create Gyro and Accel Vector
//	gyro_corr = toVector(gyro);
//	accel_meas = toVector(accel);
//
//	//Rotate Accel Vector from body to world frame using recently quaternion
//	accel_inertial = rotateVector(&accel_meas, recentQuat);
//
//	//"Compare" inertial vector to expected result
//	corr = crossProductFromOrigin(&accel_inertial, &normal_force);
//
//	//Scale correction vector
//	corr = multVectorByScalar(corr, scale);
//
//	//Get Inverse of quaternion
//	quatInverse(quat, quat_inv);
//
//	//Rotate back to Body Frame
//	rotateVector(&corr, quat_inv);
//
//	//Compute corrected gyro vector
//	gyro_corr = addVectors(gyro_corr, corr);
//
//	//Convert Vector back to float array
//	vectorToFloatArray(gyro_corr, gyro);

	//Convert gyro data from dgr/s to rad/s and compute sum of squares
	for (i = 0; i < NUMBER_OF_AXES; i++) {
		gyro_rad[i] = degToRad(gyro[i]);
		gyro_mag += (gyro_rad[i] * gyro_rad[i]);
	}

	//Prevent division by  zero
	if (gyro_mag != 0) {
		//Compute rotational vector magnitude
		gyro_mag = sqrtf(gyro_mag);

		//Compute Unit vector based on body rotation vector
		for (i = 0; i < NUMBER_OF_AXES; i++) {
			gyro_rad[i] = gyro_rad[i] / gyro_mag;
		}
	}

	//Compute Rotation Angle
	rot_angle = gyro_mag * (*delta_t);

	//Construct Quaternion
	quat[0] = cosf(rot_angle / 2);	//w

	//x, y , z
	for (i = 0; i < NUMBER_OF_AXES; i++) {
		quat[i + 1] = gyro_rad[i] * sinf(rot_angle / 2);
	}
}

/*
 * Read register data for specified amount of time and prints to UART
 * In: pointer to memory (not yet allocated), sample time in s, calibration time in s
 */
void collectRegisterData(unsigned int sampleTime, unsigned int calibrationTime) {
	//Variables
	MpuRegisterData data;
	unsigned long cnt = 0, samples = 0;
	int status;
	unsigned char *bufStart, *bufCurr;

	//Compute number of data samples
	samples = sampleTime * FIFO_RATE;

	//Set Pointer for Buffer
	bufStart = (unsigned char*) 0x7000000;
	bufCurr = bufStart;

	//Initialize
	status = initIMU(calibrationTime);
	if (status != XST_SUCCESS) {
		printf("Could not initialize IMU.\r\n");
		return;
	}

	//Get Samples
	while (cnt < samples) {
		if (needToUpdateData() == BOOL_TRUE) {

			//Read Sensor Data and write to memory
			status = readFromRegs(data.gyro, data.accel, data.compass,
					&data.temp, 0, SENSORS_ALL);

			//Check whether IIC Bus is Busy
			if (status == XST_DEVICE_BUSY) {
				break; //TODO Any error handling?
			}

			//Read successful?
			if (status == XST_SUCCESS) {
				//LED Run
				ledRun();

				//Store to buffer
				*bufCurr = (unsigned char) (data.gyro[0] & BYTE0);
				bufCurr++;
				*bufCurr = (unsigned char) ((data.gyro[0] & BYTE1) >> 8);
				bufCurr++;
				*bufCurr = (unsigned char) (data.gyro[1] & BYTE0);
				bufCurr++;
				*bufCurr = (unsigned char) ((data.gyro[1] & BYTE1) >> 8);
				bufCurr++;
				*bufCurr = (unsigned char) (data.gyro[2] & BYTE0);
				bufCurr++;
				*bufCurr = (unsigned char) ((data.gyro[2] & BYTE1) >> 8);
				bufCurr++;
				*bufCurr = (unsigned char) (data.accel[0] & BYTE0);
				bufCurr++;
				*bufCurr = (unsigned char) ((data.accel[0] & BYTE1) >> 8);
				bufCurr++;
				*bufCurr = (unsigned char) (data.accel[1] & BYTE0);
				bufCurr++;
				*bufCurr = (unsigned char) ((data.accel[1] & BYTE1) >> 8);
				bufCurr++;
				*bufCurr = (unsigned char) (data.accel[2] & BYTE0);
				bufCurr++;
				*bufCurr = (unsigned char) ((data.accel[2] & BYTE1) >> 8);
				bufCurr++;
				*bufCurr = (unsigned char) (data.compass[0] & BYTE0);
				bufCurr++;
				*bufCurr = (unsigned char) ((data.compass[0] & BYTE1) >> 8);
				bufCurr++;
				*bufCurr = (unsigned char) (data.compass[1] & BYTE0);
				bufCurr++;
				*bufCurr = (unsigned char) ((data.compass[1] & BYTE1) >> 8);
				bufCurr++;
				*bufCurr = (unsigned char) (data.compass[2] & BYTE0);
				bufCurr++;
				*bufCurr = (unsigned char) ((data.compass[2] & BYTE1) >> 8);
				bufCurr++;
				*bufCurr = (unsigned char) (data.temp & BYTE0);
				bufCurr++;
				*bufCurr = (unsigned char) ((data.temp & BYTE1) >> 8);
				bufCurr++;
				*bufCurr = (unsigned char) ((data.temp & BYTE2) >> 16);
				bufCurr++;
				*bufCurr = (unsigned char) ((data.temp & BYTE3) >> 24);
				bufCurr++;

				//Increase Counter
				cnt++;
			}
		}
	}

	//Disable Timer Interrupts
	disableTmrInt();

	//Initialize XUart
	initXUartPs();

	//Transmit buf
	//printf("XModem Transmission starts.\r\n");
	xmodemTransmit(bufStart, ((cnt - 1) * DATA_NUMBER_OF_BYTES));
	//printf("XModem Transmission finished.\r\n");
}

/*
 * Read from Registers
 * OUT: gyro, accel, comp, timestamp of
 * IN: sensors (INV_XYZ_GYRO for gyro, INV_XYZ_ACCEL for accel, INV_XYZ_COMPASS for compass)
 * Returns 0 if successful
 */
int readFromRegs(short *gyro, short *accel, short* comp, long* temp,
		unsigned long *timestamp, short sensorMask) {
//Variables
	int status;
	//unsigned long ts_gyro = 0, ts_accel = 0, ts_comp = 0, ts_temp = 0;

	//Get Timestamp if required
	if (timestamp > 0) {
		imuGet_ms(timestamp);
	}

//Get Gyro
	if (sensorMask & INV_XYZ_GYRO) {
		status = imuReadGyroReg(gyro, 0);
		if (status != XST_SUCCESS) {
			return status;
		}
	}

//Get Acc
	if (sensorMask & INV_XYZ_ACCEL) {
		status = imuReadAccelReg(accel, 0);
		if (status != XST_SUCCESS) {
			return status;
		}
	}

//Get Compass
	if (sensorMask & INV_XYZ_COMPASS) {
		status = imuReadCompassReg(comp, 0);
		if (status != XST_SUCCESS) {
			return status;
		}
	}

	//Get Temperature
	if (sensorMask & SENSOR_TEMP) {
		status = imuReadTemp(temp, 0);
		if (status != XST_SUCCESS) {
			return status;
		}
	}

	//Return
	return XST_SUCCESS;
}

/*
 * Get IMU Address
 * Out: IMU Address
 * Returns 0 if successful
 */
int getImuAddr(u8* addr) {
	//Variables
	int status;

	//Has IMU address been set already?
	if (imuAddr == 0) {
		//Initialize
		status = initMPU();

		//Check for successful initialization
		if (status != XST_SUCCESS) {
			//Check whether IIC Device is busy
			if (status == XST_DEVICE_BUSY) {
				exit(0);
			} else {
				return status;
			}
		}
	}

	//Put address into global variable and return
	*addr = imuAddr;
	return XST_SUCCESS;
}

/*
 * Configure MPU
 * In: Fifo Mask:
 * INV_X_GYRO, INV_Y_GYRO, INV_Z_GYRO
 * INV_XYZ_GYRO
 * INV_XYZ_ACCEL
 * Returns 0 if successful
 */
int configureMpuFifo(unsigned char fifoMask) {
	//Variables
	int status;

	//Enable MPU FIFO
	status = mpu_configure_fifo(fifoMask);
	if (status != XST_SUCCESS) {
		myprintf("mpu.c: Error configuring FIFO.\n\r");

		//Check whether IIC Device is busy
		if (status == XST_DEVICE_BUSY) {
			exit(0);
		} else {
			return status;
		}
	}

	//Reset FIFO
	status = imuResetFifo();
	if (status != XST_SUCCESS) {
		myprintf("mpu.c: Could not reset FIFO.\n\r");
		return status;
	}

	//Return
	return XST_SUCCESS;
}

/*
 * Initialize IMU
 * Returns 0 if successful
 */
int initMPU() {
//Variables
	int status;

//1. Init IMU (Set Address, etc.)
	status = imuInit(&imuAddr);
	if (status != XST_SUCCESS) {
		myprintf("mpu.c: Error in Setting IMU Address.\n\r");

		//Check whether IIC Device is busy
		if (status == XST_DEVICE_BUSY) {
			exit(0);
		} else {
			return status;
		}
	}

//2. Init IMU
	struct int_param_s param;
	status = imuInitMpu(&param);
	if (status != 0) {
		myprintf("mpu.c: Error initializing IMU\r\n.");

		//Check whether IIC Device is busy
		if (status == XST_DEVICE_BUSY) {
			exit(0);
		} else {
			return status;
		}
	}

//3. Select Sensors
	unsigned char sensors = SENSORS_INV;
	status = imuSetSensors(sensors);
	if (status != 0) {
		myprintf("mpu.c: Error setting sensors.\r\n");

		//Check whether IIC Device is busy
		if (status == XST_DEVICE_BUSY) {
			exit(0);
		} else {
			return status;
		}
	}

//4. Set Sample Rate
	status = imuSetSampleRate(MPU_SAMPLE_RATE);
	if (status != 0) {
		myprintf("mpu.c: Error setting MPU sample rate.\r\n");

		//Check whether IIC Device is busy
		if (status == XST_DEVICE_BUSY) {
			exit(0);
		} else {
			return status;
		}
	}

//5. Set sensitivities
//Gyro
	status = imuSetGyroFsr(GYRO_SENS);
	if (status != 0) {
		myprintf("mpu.c: Error setting gyroscope FRS.\r\n");

		//Check whether IIC Device is busy
		if (status == XST_DEVICE_BUSY) {
			exit(0);
		} else {
			return status;
		}
	}
//Accel
	status = imuSetAccelFsr(ACCEL_SENS);
	if (status != 0) {
		myprintf("mpu.c: Error setting gyroscope FRS.\r\n");

		//Check whether IIC Device is busy
		if (status == XST_DEVICE_BUSY) {
			exit(0);
		} else {
			return status;
		}
	}

	//6. Init Global Variables
	initVar();

//Sleep and Return
	return XST_SUCCESS;
}

/*
 * Initialize Global Variables
 */
void initVar() {
	//Variables
	int i;
	float temp_f[NUMBER_OF_AXES];
	long temp_l[NUMBER_OF_AXES];

	//Init temp variables
	for (i = 0; i < NUMBER_OF_AXES; i++) {
		temp_f[i] = 0.0;
		temp_l[i] = 0;
	}

	//Initialize
	memcpy(glob_gyro_bias, temp_l, NUMBER_OF_AXES * sizeof(long));
	memcpy(glob_accel_bias, temp_l, NUMBER_OF_AXES * sizeof(long));
	glob_accel_bias[GRAVITY_AXIS] = 1.0;
	memcpy(normal_force.value, temp_f, NUMBER_OF_AXES * sizeof(float));
	normal_force.value[GRAVITY_AXIS] = 1.0;
	recent_ts = 0;
	memcpy(recentGyro, temp_f, NUMBER_OF_AXES * sizeof(float));
	memcpy(recentAccel, temp_f, NUMBER_OF_AXES * sizeof(float));
	recentAccel[GRAVITY_AXIS] = 1.0;
	memcpy(recentCompass, temp_f, NUMBER_OF_AXES * sizeof(float));
	memcpy(recentQuat, temp_f, NUMBER_OF_AXES * sizeof(float));
	recentQuat[0] = 1.0;
	recentQuat[3] = 0.0;
	recentTemp = 0.0;
	memcpy(recentAccelInertial.value, temp_f, NUMBER_OF_AXES * sizeof(float));
	memcpy(recentVelocity.value, temp_f, NUMBER_OF_AXES * sizeof(float));
	memcpy(recentPosition.value, temp_f, NUMBER_OF_AXES * sizeof(float));
}
