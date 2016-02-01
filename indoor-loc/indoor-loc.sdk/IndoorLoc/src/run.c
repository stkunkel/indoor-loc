/*
 * run.c: Main Program
 * Author: Stephanie Kunkel
 */

/*
 * Includes
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "platform.h"
#include "print_utils.h"
#include "imu/mpu.h"
#include "imu/mpu_int.h"
#include "robot/pwmsw.h"
#include "zedboard/gpio_utils.h"
#include "bool.h"

/*
 * Defines
 */
#define DATA_NO_DMP_RUNS 	10
#define DATA_WITH_DMP_RUNS 	10
#define QUAT_DISPLAY_RUNS	10
#define QUAT_DRIFT_MIN		1

/*
 * Function Prototypes
 */
int printQuaternionDriftAfterXMin(unsigned int time_min);
int printDataForAnalysis(short sensors, short data, unsigned int numberOfRuns);
int printForImuViewer(short int printMask, unsigned int numberOfRuns);
int printDataWithoutDMP(short int sensors, unsigned int numberOfRuns);
int printDataUsingDMP(short sensors, char initialCalibration,
		char dmpCalibration, unsigned int numberOfRuns);

/*
 * Main
 */
int main() {
	//Variables
	int status = XST_SUCCESS;

	//Init Platform
	init_platform();

	//Print Output Separator
	myprintf(".........Program Start...........\n\r");

	//Print Data without DMP
//	status = printDataWithoutDMP(SENSORS_ALL, DATA_NO_DMP_RUNS);

//Print Data with DMP without initial or DMP gyro calibration
//	status = printDataUsingDMP(SENSORS_ALL, 0, 0, DATA_WITH_DMP_RUNS);

//Print Data with DMP with initial calibration but no DMP gyro calibration
//	status = printDataUsingDMP(SENSORS_ALL, 1, 0, DATA_WITH_DMP_RUNS);

//Print Data with DMP with and DMP gyro calibration
//	status = printDataUsingDMP(SENSORS_ALL, 1, 1, DATA_WITH_DMP_RUNS);

//Print Quaternions and Position to Serial Port
	status = printForImuViewer(PRINT_ALL, QUAT_DISPLAY_RUNS);

	//Quaternion Drift
	//status = printQuaternionDriftAfterXMin(QUAT_DRIFT_MIN);

	//PWM Test
	//status = pwmTest();

	//Test position update functionality
//	testPositionUpdate();

	//Test Matrix Inverse
//	testMatrixInverse();

	//Test Quaternions for IMU Viewer
//	quaternionTest();

	//Test LED
//	testToggleLed();
//	testLedRun();

	//Stay in here
	while (1) {
		;
	}

	//Finish
	cleanup_platform();
	return 0;
}

/*
 * Prints Quaternion Drift
 * In: time in min
 * Returns 0 when done
 */
int printQuaternionDriftAfterXMin(unsigned int time_min) {
	//Print
	myprintf(".........Drift...........\n\r");
	printQuatDrift(time_min);

	//Return
	return XST_SUCCESS;
}

int printDataForAnalysis(short sensors, short printMask,
		unsigned int numberOfRuns) {
	//Variables
	int cnt = 0, printcnt = 0, status;

	//Init
	myprintf(".........Configure MPU and DMP...........\n\r");
	status = configureDMP(FEATURES_CAL, DMP_FIFO_RATE);
	if (status != XST_SUCCESS) {
		return status;
	}

	//Calibrate
	myprintf(".........Calibrate...........\n\r");
	status = calibrateGyrAcc(CAL_SAMPLES);
	if (status != XST_SUCCESS) {
		return status;
	}

	//Enable Interrupts
	status = setupMPUInt();
	if (status != XST_SUCCESS) {
		return status;
	}

	//Init GPIO
	initGpio();

	//Print Quaternions to Serial Port
	myprintf(".........Print for IMU Viewer...........\n\r");
	for (cnt = 0; cnt <= numberOfRuns; cnt++) {
		//Reset Status
		status = XST_SUCCESS;

		//Wait for Interrupt Interrupt
		if (imuDataAvailable()) {
			//Update Data
			status = updateData();

			//Check whether data should be printed
			printcnt++;
			if (status == XST_SUCCESS) {
				//LED Run
				ledRun();

				if ((printcnt % (DMP_FIFO_RATE / IMUVIEWER_FREQ)) == 0) {
					//Reset printcnt
					printcnt = 0;

					//Print Data

					//Print Quat, Vel, and Pos
					status = printforDisplay(&printMask, " | ");

					//Print new line
					if (status == XST_SUCCESS) {
						printf("\n\r");
					} else {
						cnt--;
					}
				}
			} else {
				cnt--;
			}
		}

		//Make sure only successful prints count
		if (numberOfRuns < 1) {
			cnt--;
		}
	}

	//Return
	return status;
}

/*
 * Prints Quaternions and/or Position for IMU Viewer
 * In: number of runs (if 0 --> print forever)
 * Returns 0 if successful
 */
int printForImuViewer(short int printMask, unsigned int numberOfRuns) {
	//Variables
	int cnt = 0, printcnt = 0, status;

	//Init
	myprintf(".........Configure MPU and DMP...........\n\r");
	status = configureDMP(FEATURES_CAL, DMP_FIFO_RATE);
	if (status != XST_SUCCESS) {
		return status;
	}

	//Calibrate
	myprintf(".........Calibrate...........\n\r");
	status = calibrateGyrAcc(CAL_SAMPLES);
	if (status != XST_SUCCESS) {
		return status;
	}

	//Enable Interrupts
	status = setupMPUInt();
	if (status != XST_SUCCESS) {
		return status;
	}

	//Init GPIO
	initGpio();

	//Adjust Number of Runs
	numberOfRuns *= (DMP_FIFO_RATE / IMUVIEWER_FREQ);

	//Print Quaternions to Serial Port
	myprintf(".........Print for IMU Viewer...........\n\r");
	for (cnt = 0; cnt <= numberOfRuns; cnt++) {
		//Reset Status
		status = XST_SUCCESS;

		//Wait for Interrupt Interrupt
		if (imuDataAvailable()) {
			//Update Data
			status = updateData();

			//Check whether data should be printed
			if (status == XST_SUCCESS) {
				//Increase print counter
				printcnt++;

				//LED Run
				ledRun();

				if ((printcnt % (DMP_FIFO_RATE / IMUVIEWER_FREQ)) == 0) {
					//Reset printcnt
					printcnt = 0;

					//Print
					status = printforDisplay(&printMask, " | ");

					//Print new line
					if (status == XST_SUCCESS) {
						printf("\n\r");
					} else {
						cnt--;
					}
				}
			} else {
				cnt--;
				printcnt--;
			}
		} else {
			cnt--;
		}

		//Make sure only successful prints count
		if (numberOfRuns < 1) {
			cnt--;
		}
	}

//Return
	return status;
}

/*
 * Print Data using DMP
 * In: calibration, number of runs (if 0 --> endless loop)
 */
int printDataUsingDMP(short sensors, char initialCalibration,
		char dmpCalibration, unsigned int numberOfRuns) {
//Variables
	int cnt = 0, printcnt = 0, status;
	short features;

	if (initialCalibration) {
		features = FEATURES_CAL;
	} else {
		features = FEATURES_RAW;
	}

	//Init
	myprintf(".........Configure MPU and DMP...........\n\r");
	status = configureDMP(features, DMP_FIFO_RATE);
	if (status != XST_SUCCESS) {
		return status;
	}

//Calibrate if required
	if (initialCalibration) {
		myprintf(".........Calibration...........\n\r");
		status = calibrateGyrAcc(CAL_SAMPLES);
		if (status != XST_SUCCESS) {
			return status;
		}
	}

//En- or disable dynamic gyro calibration
	status = dmpGyroCalibration(dmpCalibration);
	if (status != XST_SUCCESS) {
		return status;
	}

	//Enable Interrupts
	status = setupMPUInt();
	if (status != XST_SUCCESS) {
		return status;
	}

	//Adjust Number of Runs
	numberOfRuns *= (DMP_FIFO_RATE / IMUVIEWER_FREQ);

//Get Data with DMP
	myprintf(".........With DMP...........\n\r");
	for (cnt = 0; cnt <= numberOfRuns; cnt++) {
		//Reset Status
		status = XST_SUCCESS;

		//Wait for Interrupt Interrupt
		if (imuDataAvailable()) {
			//Update Data
			status = updateData();

			//Check whether data should be printed
			if (status == XST_SUCCESS) {
				//Increase print counter
				printcnt++;

				//Dont print every data set
				if ((printcnt % (DMP_FIFO_RATE / IMUVIEWER_FREQ)) == 0) {
					//Reset printcnt
					printcnt = 0;

					//Print
					printDataWithDMP(&sensors, " | ");

					//Print new line
					printf("\n\r");
				}
			} else {
				cnt--;
				printcnt--;
			}
		} else {
			cnt--;
		}

		//Make sure only successful prints count
		if (numberOfRuns < 1) {
			cnt--;
		}
	}

//Return
	return XST_SUCCESS;
}

/*
 * Print Data without DMP
 * In: sensors, number of runs (if 0 --> endless loop)
 */
int printDataWithoutDMP(short int sensors, unsigned int numberOfRuns) {
//Variables
	int i, cnt, status;

//Get Data without DMP
	myprintf(".........Without DMP...........\n\r");
	for (cnt = 0; cnt <= numberOfRuns; cnt++) {
		//Print
		status = printDataNoDMP(&sensors);

		//Decrease count if not successful
		if (status != XST_SUCCESS || numberOfRuns < 1) {
			cnt--;
		}

		//Wait
//		for (i = 0; i <= 10000; i++) {
//			;
//		}
		usleep(1000);
	}
	return XST_SUCCESS;
}
