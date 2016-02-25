/*
 * robotCtrl.c: Robot control and movement data collection
 * Author: Stephanie Kunkel
 */

/*
 * Includes
 */
#include "robotCtrl.h"

/*
 * Test MPU Gyro Sensitivity Calculation
 */
void testMpuGyroSensCalc() {
	//Variable
	int status;
	float gyro_sens[NUMBER_OF_AXES];

	//Determine Sensitivity
	status = determineMpuGyroSensitivity(5000, gyro_sens);

	//Print
	if (status != PWM_SUCCESS) {
		printf("robotCtrl.c: Could not determine gyro sensitivity.\r\n");
	} else {
		printf("Gyro Sens: N/A %f %f\r\n", gyro_sens[1], gyro_sens[2]);
	}

}

/*
 * Determine MPU Gyroscope Sensitivity (y- and z-axis only!)
 * In: Numer of samples
 * Out: gyro biases (difference between 90dgr and measured angle for 90° movement for each axis except x)
 * Returns 0 if successful
 */
int determineMpuGyroSensitivity(unsigned int samples,
		float gyro_sens[NUMBER_OF_AXES]) {
	//Variables
	int status;
	unsigned int s, q, j, i = 0;
	float angle[2];
	Joint joint[NUMBER_OF_AXES] = { thumb, wrist, base };
	float quat_abs[QUATERNION_AMOUNT], quat_rel[QUATERNION_AMOUNT],
			quat_new[QUATERNION_AMOUNT];
	float gyro_conv[NUMBER_OF_AXES], accel_conv[NUMBER_OF_AXES];
	RobotMpuData data;
	unsigned long timestamp = 0, recent_ts = 0;
	float time_diff;
	float max = 0.0;

	//Set Angles
	angle[0] = 0.0;
	angle[1] = 90.0;

	//Initialize quaternions and Euler angles
	quat_abs[0] = 1.0;
	quat_rel[0] = 1.0;
	for (q = 1; q < QUATERNION_AMOUNT; q++) {
		quat_abs[q + 1] = 0.0;
		quat_rel[q + 1] = 0.0;
	}

	//Initialize IMU
	status = initIMU(samples / FIFO_RATE);
	if (status != XST_SUCCESS) {
		myprintf("Could not initialize IMU.\r\n");
		return PWM_FAILURE;
	}

	//For each joint
	for (j = 0; j < NUMBER_OF_AXES; j++) {

		//Skip x axis
		if (joint[j] == thumb || joint[j] == wrist) { //TODO
			continue;
		}

		//Reset Robot
		status = reset();
		if (status != PWM_SUCCESS) {
			return PWM_FAILURE;
		}
		sleep(2);

		//Reset i
		i = 0;

		//Reset Quaternion and max values
		quat_abs[0] = 1.0;
		max = 0.0;
		for (q = 1; q < QUATERNION_AMOUNT; q++) {
			quat_abs[q] = 0.0;
		}

		//Take samples
		for (s = 0; s < samples; s++) {

			//Wait for Timer Interrupt
			while (needToUpdateData() == BOOL_FALSE) {
				usleep(1);
			}

			//Set Angle
			if (s % ((FIFO_RATE * HSS_422_TIME_FOR_90_DGRS) / 1000) == 0) {
				setAngle(joint[j], angle[i % 2]);
				i++;

				if (i % 2 == 0) {
					max = quat_abs[j + 1];
				}

//				if (i == 2){
//					break;
//				}
			}
//			if (i < 1) {
//				setAngle(joint[j], angle[1]);
//				i++;
//				samples = s + ((FIFO_RATE * HSS_422_TIME_FOR_90_DGRS) / 1000);
//			}

			//Get PWM Data
			getPwmRegValues(data.pwmValues);

			//Read Sensor Data and write to memory
			status = readFromRegs(data.mpuData.gyro, data.mpuData.accel,
					data.mpuData.compass, &data.mpuData.temp, &timestamp,
					SENSORS_ALL);

			//Read successful?
			if (status == XST_SUCCESS) {
				//LED Run
				ledRun();

				//Handle first function call
				if (recent_ts == 0) {
					//Set recent timestamp (time diff is in s, so it has to be converted to ms)
					recent_ts = timestamp - (long) (1000 * time_diff);
				}

				//Update time difference
				time_diff = (timestamp - recent_ts) / 1000.0;

				//Convert Gyro
				convertGyroData(data.mpuData.gyro, gyro_conv);

				//Convert Accel
				convertAccData(data.mpuData.accel, accel_conv);

				//Compute Quaternion of relative rotation
				computeQuaternion(gyro_conv, accel_conv, &time_diff, quat_rel);

				//Compute new absolute rotation
				multiplyQuaternions(quat_abs, quat_rel, quat_new);

//				//Remember biggest Rotation
//				if (quat_abs[j + 1] > max) {
//					max = quat_abs[j + 1];
//				}

				//Store new quaternion as absolute rotation
				memcpy(&quat_abs, &quat_new, QUATERNION_AMOUNT * sizeof(float));
			} else {
				s--;
			}
		}

		//Compute sensitivity TODO
		gyro_sens[j] = max / 0.7071;

		//Reset
		reset();
	}

//Return
	return PWM_SUCCESS;
}

/*
 * Move Robot and collect Data
 */

/*
 * Collect Robot Movement Data
 * In: sample time (s), calibration time (s), bool whether to collect data and send it via UART or print directly to UART
 * Returns 0 if successful
 */
int collectRobotMvmtData(unsigned int sampleTime, unsigned int calibrationTime,
		bool collect) {
//Variables
	RobotMpuData data;
	uint32_t cnt = 0, samples = 0, anglecnt = 0, angleid = 0;
	int status;
//	unsigned char *bufStart, *bufCurr;
	Joint joint = base;
	float angle[4], currAngle, dir;

//Set angles
	angle[0] = 0.0;
	angle[1] = 0.0;//90.0;
	angle[2] = 0.0;//90.0;
	angle[3] = 0.0;
	currAngle = angle[0];
	dir = 1.0;

//Compute number of data samples
	samples = sampleTime * FIFO_RATE;

////Set Pointer for Buffer
//	bufStart = (unsigned char*) 0x7000000;
//	bufCurr = bufStart + sizeof(cnt);

//Reset Robot
	status = reset();
	if (status != PWM_SUCCESS) {
		return PWM_FAILURE;
	}

//Initialize IMU
	status = initIMU(calibrationTime);
	if (status != XST_SUCCESS) {
		printf("Could not initialize IMU.\r\n");
		return PWM_FAILURE;
	}

//Reset cnt
	cnt = 0;

//Get Samples
	while (cnt < samples) {

//		//Get Moving Direction
//		if (currAngle >= angle[1]){
//			dir = -1.0;
//		}  else if (currAngle <= angle[0]){
//			dir = 1.0;
//		}
//
//		//Move Robot
//		currAngle += dir;
//		setAngle(joint, currAngle);

//Set Angle
//		if (cnt % 2 == 0) {
//			setAngle(joint, angle[0]);
//		} else {
//			setAngle(joint, angle[1]);
//		}

//Get Sensor Data
		while (needToUpdateData() == BOOL_FALSE) {
			usleep(1);
		}

		//Set Angle
		if (cnt % ((FIFO_RATE * HSS_422_TIME_FOR_90_DGRS) / 1000) == 0) {
			if (angleid >= 2) {
				angleid = 0;
			}
			setAngle(joint, angle[angleid]);
			angleid++;
		}

		//Get PWM Data
		getPwmRegValues(data.pwmValues);

		//Read Sensor Data and write to memory
		status = readFromRegs(data.mpuData.gyro, data.mpuData.accel,
				data.mpuData.compass, &data.mpuData.temp, 0, SENSORS_ALL);

		//Read successful?
		if (status == XST_SUCCESS) {
			//LED Run
			ledRun();

			//Collect Data in required
			if (collect == BOOL_TRUE) {

				//Store Values in buffer
				storeInBuff(&data);

			} else {
				//Update Data
				status = updateData();

				//Check whether data should be printed
				if (status == XST_SUCCESS) {
					if ((cnt % (FIFO_RATE / IMUVIEWER_FREQ)) == 0) {

						//Print
						status = printforDisplay(PRINT_FOR_VIEWER, " ");

						//Print new line
						if (status == XST_SUCCESS) {

							//Print new line
							printf("\n\r");
						}
					}
				}
			}

			//Increase Counter
			cnt++;

//		} else if (status == XST_DEVICE_BUSY){
//			break;
		}
	}

//Reset Robot
	reset();

	//Transmit Buff
	status = transmitBuf();

//Return
	return status;
}
