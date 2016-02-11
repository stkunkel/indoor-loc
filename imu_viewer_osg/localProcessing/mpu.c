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
void printRaw(short sensor[NUMBER_OF_AXES]);
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
int readFromFifo(short *gyro, short *accel, long *quat,
		unsigned long *timestamp, short *sensors, unsigned char *more);
void computeQuaternion(float gyro[NUMBER_OF_AXES], float accel[NUMBER_OF_AXES],
		float* delta_t, float quat[QUATERNION_AMOUNT]);
int readInData(FILE* file, short *gyro, short *accel, unsigned long *timestamp);


/*
 * Variables
 */
static FILE* infile;
static FILE* outfile;
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
 * Update Sensor Data and Position
 * Returns 0 if successful
 */
int updateData() {
//Variables
	int i, status = XST_SUCCESS;
	short gyro[NUMBER_OF_AXES], accel[NUMBER_OF_AXES], compass[NUMBER_OF_AXES];
	long temperature;
	unsigned long timestamp;
	float gyro_conv[NUMBER_OF_AXES] = { 0.0, 0.0, 0.0 }, accel_conv[NUMBER_OF_AXES] = { 0.0, 0.0, 0.0 },
			compass_conv[NUMBER_OF_AXES] = { 0.0, 0.0, 0.0 }, quat_conv[QUATERNION_AMOUNT] = { 0.0, 0.0, 0.0 },
			quat_new[QUATERNION_AMOUNT] = { 1.0, 0.0, 0.0, 0.0 }, temp_conv = 0.0;
	float time_diff = 1.0 / FIFO_RATE;

	//Read Data from Registers
	status = readInData(infile, gyro, accel, &timestamp);
	if (status != XST_SUCCESS) {
		return XST_FAILURE;
	}

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

	//Compute Quaternion quat_new if not using DMP
	computeQuaternion(gyro_conv, accel_conv, &time_diff, quat_new);

	//Update rotation (quat_conv = recentQuat * quat_new)
	multiplyQuaternions(recentQuat, quat_new, quat_conv);


//Update Position and Velocity
	updatePosition(accel_conv, quat_conv, &recentAccelInertial, &recentVelocity,
			&recentPosition, &time_diff);

//Update Timestamp
	recent_ts = timestamp;

//Update Sensor Data
	memcpy(&recentGyro, &gyro_conv, NUMBER_OF_AXES * sizeof(float));
	memcpy(&recentAccel, &accel_conv, NUMBER_OF_AXES * sizeof(float));
	memcpy(&recentCompass, &compass_conv, NUMBER_OF_AXES * sizeof(float));
	memcpy(&recentQuat, &quat_conv, QUATERNION_AMOUNT * sizeof(float));

//Update Temperature
	recentTemp = temp_conv;

//Print Data to file
	//Print data to file (Gyro)
	for (i = 0; i < NUMBER_OF_AXES; i++){
	  fprintf(outfile, "%f ", recentGyro[i]);
	}
	
	//Print data to file (Accel)
	for (i = 0; i < NUMBER_OF_AXES; i++){
	  fprintf(outfile, "%f ", recentAccel[i]);
	}
	
	//Print data to file (Quat)
	for (i = 0; i < QUATERNION_AMOUNT; i++){
	  fprintf(outfile, "%f ", recentQuat[i]);
	}
	
	//Print data to file (Vel)
	for (i = 0; i < NUMBER_OF_AXES; i++){
	  fprintf(outfile, "%f ", recentVelocity.value[i]);
	}
	
	//Print data to file (Pos)
	for (i = 0; i < NUMBER_OF_AXES; i++){
	  fprintf(outfile, "%f ", recentPosition.value[i]);
	}
	
	fprintf(outfile, "\r\n");
	

//Return
	return status;
}

/*
 * Update Position (also updates timestamps)
 */
void updatePosition(float* accel_conv, float* quat_conv,
		Vector* p_recentAccelInertial, Vector* p_recentVelocity,
		Vector* p_recentPosition, float* time_diff) {
//Variables
	Vector accel_measuremt, normal_force_rot, accel_inertial, velocity,
			newPosition;
	Matrix rotation, rotation_inv;

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
 * Convert Gyroscope Data using Sensitivity
 */
int convertGyroData(short raw[NUMBER_OF_AXES], float converted[NUMBER_OF_AXES]) {
//Variables
	int i;
	float sensitivity;

//get sensitivity
	sensitivity = GYRO_SENSITIVITY;

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
	int i;
	unsigned short sensitivity;

//get sensitivity
	sensitivity = ACCEL_SENSITIVITY;

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
 * Read from Sensor Data From File
 * OUT: gyro, accel, timestamp
 * IN: FILE pointer
 */
int readInData(FILE* file, short *gyro, short *accel, unsigned long *timestamp) {
//Variables
  int success;
  char* line = 0;
  size_t len = 0;
  ssize_t read;

//Read in one Line from File
  if ((read = getline(&line, &len, file)) != -1) {
    //Get Values from File
    success = sscanf(line, "%hd %hd %hd %hd %hd %hd", &gyro[0], &gyro[1], &gyro[2], &accel[0], &accel[1], &accel[2]);
    
    //Check for correct amount of values
    if (success != 6){
      return XST_FAILURE;
    }
    
  } else {
    return XST_FAILURE;
  }


//Compute Timestamp if required
  if (timestamp > 0) {
    // Increase Timestamp
    *timestamp = recent_ts + 1000/FIFO_RATE;
  }

//Return
  return XST_SUCCESS;
}

/*
 * Initialize
 */
int init(){
  //Open Files
  infile = fopen(INFILEDESC, "r");
  outfile = fopen(OUTFILEDESC, "w");

  if (infile == 0 || outfile == 0){
    return XST_FAILURE;
  } else {
    printf("Files opened.\r\n");
  }

  //Init Variables
  initVar();
  printf("Init done.\r\n");

  //Return
  return XST_SUCCESS;
}

/*
 * Finish
 */
int finish(){
  //Close Input File
  if (infile){
    fclose(infile);
    printf("Input File closed.\r\n");
  }
  
  //Close Output File
   if (outfile){
    fclose(outfile);
    printf("Output File closed.\r\n");
  }

  //Return
  return XST_SUCCESS;
}

/*
 * Initialize Global Variables
 */
void initVar() {
	//Variables
	int i = 0;
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
	//memcpy(normal_force.value, temp_f, NUMBER_OF_AXES * sizeof(float));
	normal_force.value[0] = NORM_MEAS_X;
	normal_force.value[1] = NORM_MEAS_Y;
	normal_force.value[2] = NORM_MEAS_Z;
	recent_ts = 0;
	memcpy(recentGyro, temp_f, NUMBER_OF_AXES * sizeof(float));
	memcpy(recentAccel, temp_f, NUMBER_OF_AXES * sizeof(float));
	memcpy(recentCompass, temp_f, NUMBER_OF_AXES * sizeof(float));
	memcpy(recentQuat, temp_f, NUMBER_OF_AXES * sizeof(float));
	recentQuat[0] = 1.0;
	recentQuat[3] = 0.0;
	recentTemp = 0.0;
	memcpy(recentAccelInertial.value, temp_f, NUMBER_OF_AXES * sizeof(float));
	memcpy(recentVelocity.value, temp_f, NUMBER_OF_AXES * sizeof(float));
	memcpy(recentPosition.value, temp_f, NUMBER_OF_AXES * sizeof(float));
}
