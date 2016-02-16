/*
 * robotCtrl.c: Robot control and movement data collection
 * Author: Stephanie Kunkel
 */
#include "robotCtrl.h"
/*
 * Collect Robot Movement Data
 */
int collectRobotMvmtData(unsigned int sampleTime, unsigned int calibrationTime,
		bool collect) {
	//Variables
	MpuRegisterData *dataStart, *dataColl, *print;
	unsigned long cnt = 0, samples = 0, printcnt = 0;
	int status;
	Joint joint = wrist;
	float angle[2];
	short gyro[NUMBER_OF_AXES], accel[NUMBER_OF_AXES], compass[NUMBER_OF_AXES];
	long temp;

	//Set angles
	angle[0] = 90.0;
	angle[1] = 0.0;

	//Compute number of data samples
	samples = sampleTime * FIFO_RATE;

	//Allocate Memory
	if (collect == BOOL_TRUE) {
		dataStart = (MpuRegisterData*) 0x7000000; //(MpuRegisterData*) malloc((samples + 200) * sizeof(MpuRegisterData));

		//Set Print Pointer
		dataColl = dataStart;
		print = dataStart;
	}

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

	//Read out some data sets before sampling starts
	//	for (cnt = 0; cnt < CAL_IGNORE_SAMPLES; cnt++) {
	//		if (needToUpdateData() == BOOL_TRUE) {
	//
	//			//Read Sensor Data and write to memory
	//			status = readFromRegs(dataColl->gyro, dataColl->accel,
	//					dataColl->compass, &dataColl->temp, 0, SENSORS_ALL);
	//		}
	//	}

	//Reset cnt
	cnt = 0;

	//Get Samples
	while (cnt < samples) {

		//Move Robot
		if (cnt % 2 == 0) {
			setAngle(joint, angle[0]);
		} else {
			setAngle(joint, angle[1]);
		}

		//Get Sensor Data
		if (needToUpdateData() == BOOL_TRUE) {

			//Read Sensor Data and write to memory
			if (collect == BOOL_TRUE) {
				status = readFromRegs(dataColl->gyro, dataColl->accel,
						dataColl->compass, &dataColl->temp, 0, SENSORS_ALL);
			} else {
				status = readFromRegs(gyro, accel, compass, &temp, 0,
				SENSORS_ALL);
			}

			//Read successful?
			if (status == XST_SUCCESS) {
				//LED Run
				ledRun();

				//Increase count
				cnt++;

				if (collect == BOOL_TRUE) {
					//Go to next data set
					if (cnt < samples) {
						dataColl++;
					}
				} else {
					//Print
					status = printforDisplay(PRINT_FOR_VIEWER, " ");

					//Print new line
					if (status == XST_SUCCESS) {
						//Increase print counter
						printcnt++;

						//Print new line
						printf("\n\r");
					} else {
						cnt--;
					}
				}
			}
		}
	}

	//Print samples
	if (collect == BOOL_TRUE) {
		for (printcnt = 0; printcnt < cnt; printcnt++) {
			//Print
			//		printf("%d ", print->cnt);
			printRaw(print->gyro);
			printf(" ");
			printRaw(print->accel);
			printf(" ");
			printRaw(print->compass);
			printf(" %ld;", print->temp);

			//Increase Pointer Address
			print++;
		}
	}

	//Return
	return PWM_SUCCESS;
}