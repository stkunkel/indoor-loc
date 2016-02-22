/*
 * robotCtrl.c: Robot control and movement data collection
 * Author: Stephanie Kunkel
 */

/*
 * Includes
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
	RobotMpuData data;
	uint32_t cnt = 0, samples = 0, anglecnt = 0, angleid = 0;
	int status;
	unsigned char *bufStart, *bufCurr;
	Joint joint = wrist;
	float angle[4], currAngle, dir;

	//Set angles
	angle[0] = 0.0;
	angle[1] = 90.0;
	angle[2] = 90.0;
	angle[3] = 0.0;
	currAngle = angle[0];
	dir = 1.0;

	//Compute number of data samples
	samples = sampleTime * FIFO_RATE;

	//Set Pointer for Buffer
	bufStart = (unsigned char*) 0x7000000;
	bufCurr = bufStart + sizeof(cnt);

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

				//Store PWM Values in buffer
				*bufCurr = (unsigned char) (data.pwmValues[0] & BYTE0);
				bufCurr++;
				*bufCurr = (unsigned char) ((data.pwmValues[0] & BYTE1) >> 8);
				bufCurr++;
				*bufCurr = (unsigned char) ((data.pwmValues[0] & BYTE2) >> 16);
				bufCurr++;
				*bufCurr = (unsigned char) ((data.pwmValues[0] & BYTE3) >> 24);
				bufCurr++;
				*bufCurr = (unsigned char) (data.pwmValues[1] & BYTE0);
				bufCurr++;
				*bufCurr = (unsigned char) ((data.pwmValues[1] & BYTE1) >> 8);
				bufCurr++;
				*bufCurr = (unsigned char) ((data.pwmValues[1] & BYTE2) >> 16);
				bufCurr++;
				*bufCurr = (unsigned char) ((data.pwmValues[1] & BYTE3) >> 24);
				bufCurr++;
				*bufCurr = (unsigned char) (data.pwmValues[1] & BYTE0);
				bufCurr++;
				*bufCurr = (unsigned char) ((data.pwmValues[2] & BYTE1) >> 8);
				bufCurr++;
				*bufCurr = (unsigned char) ((data.pwmValues[2] & BYTE2) >> 16);
				bufCurr++;
				*bufCurr = (unsigned char) ((data.pwmValues[2] & BYTE3) >> 24);
				bufCurr++;
				*bufCurr = (unsigned char) (data.pwmValues[3] & BYTE0);
				bufCurr++;
				*bufCurr = (unsigned char) ((data.pwmValues[3] & BYTE1) >> 8);
				bufCurr++;
				*bufCurr = (unsigned char) ((data.pwmValues[3] & BYTE2) >> 16);
				bufCurr++;
				*bufCurr = (unsigned char) ((data.pwmValues[3] & BYTE3) >> 24);
				bufCurr++;
				*bufCurr = (unsigned char) (data.pwmValues[4] & BYTE0);
				bufCurr++;
				*bufCurr = (unsigned char) ((data.pwmValues[4] & BYTE1) >> 8);
				bufCurr++;
				*bufCurr = (unsigned char) ((data.pwmValues[4] & BYTE2) >> 16);
				bufCurr++;
				*bufCurr = (unsigned char) ((data.pwmValues[4] & BYTE3) >> 24);
				bufCurr++;
				*bufCurr = (unsigned char) (data.pwmValues[5] & BYTE0);
				bufCurr++;
				*bufCurr = (unsigned char) ((data.pwmValues[5] & BYTE1) >> 8);
				bufCurr++;
				*bufCurr = (unsigned char) ((data.pwmValues[5] & BYTE2) >> 16);
				bufCurr++;
				*bufCurr = (unsigned char) ((data.pwmValues[5] & BYTE3) >> 24);
				bufCurr++;

				//Store MPU Data in buffer
				*bufCurr = (unsigned char) (data.mpuData.gyro[0] & BYTE0);
				bufCurr++;
				*bufCurr =
						(unsigned char) ((data.mpuData.gyro[0] & BYTE1) >> 8);
				bufCurr++;
				*bufCurr = (unsigned char) (data.mpuData.gyro[1] & BYTE0);
				bufCurr++;
				*bufCurr =
						(unsigned char) ((data.mpuData.gyro[1] & BYTE1) >> 8);
				bufCurr++;
				*bufCurr = (unsigned char) (data.mpuData.gyro[2] & BYTE0);
				bufCurr++;
				*bufCurr =
						(unsigned char) ((data.mpuData.gyro[2] & BYTE1) >> 8);
				bufCurr++;
				*bufCurr = (unsigned char) (data.mpuData.accel[0] & BYTE0);
				bufCurr++;
				*bufCurr =
						(unsigned char) ((data.mpuData.accel[0] & BYTE1) >> 8);
				bufCurr++;
				*bufCurr = (unsigned char) (data.mpuData.accel[1] & BYTE0);
				bufCurr++;
				*bufCurr =
						(unsigned char) ((data.mpuData.accel[1] & BYTE1) >> 8);
				bufCurr++;
				*bufCurr = (unsigned char) (data.mpuData.accel[2] & BYTE0);
				bufCurr++;
				*bufCurr =
						(unsigned char) ((data.mpuData.accel[2] & BYTE1) >> 8);
				bufCurr++;
				*bufCurr = (unsigned char) (data.mpuData.compass[0] & BYTE0);
				bufCurr++;
				*bufCurr = (unsigned char) ((data.mpuData.compass[0] & BYTE1)
						>> 8);
				bufCurr++;
				*bufCurr = (unsigned char) (data.mpuData.compass[1] & BYTE0);
				bufCurr++;
				*bufCurr = (unsigned char) ((data.mpuData.compass[1] & BYTE1)
						>> 8);
				bufCurr++;
				*bufCurr = (unsigned char) (data.mpuData.compass[2] & BYTE0);
				bufCurr++;
				*bufCurr = (unsigned char) ((data.mpuData.compass[2] & BYTE1)
						>> 8);
				bufCurr++;
				*bufCurr = (unsigned char) (data.mpuData.temp & BYTE0);
				bufCurr++;
				*bufCurr = (unsigned char) ((data.mpuData.temp & BYTE1) >> 8);
				bufCurr++;
				*bufCurr = (unsigned char) ((data.mpuData.temp & BYTE2) >> 16);
				bufCurr++;
				*bufCurr = (unsigned char) ((data.mpuData.temp & BYTE3) >> 24);
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

	//Write number of samples into buffer
	bufCurr = bufStart;
	*bufCurr = (unsigned char) (cnt & BYTE0);
	bufCurr++;
	*bufCurr = (unsigned char) ((cnt & BYTE1) >> 8);
	bufCurr++;
	*bufCurr = (unsigned char) ((cnt & BYTE2) >> 16);
	bufCurr++;
	*bufCurr = (unsigned char) ((cnt & BYTE3) >> 24);

	//Initialize XUart
	initXUartPs();

	//Transmit buf
	//printf("XModem Transmission starts.\r\n");
	xmodemTransmit(bufStart, (sizeof(cnt) + cnt * DATA_NUMBER_OF_BYTES));
	//printf("XModem Transmission finished.\r\n");

//Return
	return PWM_SUCCESS;
}
