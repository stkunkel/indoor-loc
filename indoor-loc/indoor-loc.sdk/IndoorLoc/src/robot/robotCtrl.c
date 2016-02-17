/*
 * robotCtrl.c: Robot control and movement data collection
 * Author: Stephanie Kunkel
 */
#include "robotCtrl.h"

/*
 * Collect Robot Movement Data
 * In: sample time (s), calibration time (s), bool whether to collect data and send it via UART or print directly to UART
 * Returns 0 if successful
 */
int collectRobotMvmtData(unsigned int sampleTime, unsigned int calibrationTime,
		bool collect) {
	//Variables
	MpuRegisterData data;
	unsigned long cnt = 0, samples = 0;
	int status;
	unsigned char *bufStart, *bufCurr;
	Joint joint = base;
	float angle[2], currAngle, dir;

	//Set angles
	angle[0] = 0.0;
	angle[1] = 90.0;
	currAngle = angle[0];
	dir = 1.0;

	//Compute number of data samples
	samples = sampleTime * FIFO_RATE;

	//Set Pointer for Buffer
	bufStart = (unsigned char*) 0x7000000;
	bufCurr = bufStart;

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

		if (cnt % 2 == 0) {
			setAngle(joint, angle[0]);
		} else {
			setAngle(joint, angle[1]);
		}

		//Get Sensor Data
		while (needToUpdateData() == BOOL_FALSE) {
			usleep(1);
		}

		//Read Sensor Data and write to memory
		status = readFromRegs(data.gyro, data.accel, data.compass, &data.temp,
				0, SENSORS_ALL);

		//Read successful?
		if (status == XST_SUCCESS) {
			//LED Run
			ledRun();

			//Collect Data in required
			if (collect == BOOL_TRUE) {

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
		}
	}

//Reset Robot
	reset();

//Disable Timer Interrupts
	disableTmrInt();

//Initialize XUart
	initXUartPs();

//Transmit buf
//printf("XModem Transmission starts.\r\n");
	xmodemTransmit(bufStart, ((cnt - 1) * DATA_NUMBER_OF_BYTES));
//printf("XModem Transmission finished.\r\n");

//Return
	return PWM_SUCCESS;
}
