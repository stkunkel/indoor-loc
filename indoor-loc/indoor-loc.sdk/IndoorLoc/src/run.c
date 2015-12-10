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
#include "robot/pwmsw.h"

/*
 * Defines
 */
#define DATA_NO_DMP_RUNS 	10
#define DATA_WITH_DMP_RUNS 	0
#define QUAT_DISPLAY_RUNS	0
#define QUAT_DRIFT_MIN		1

/*
 * Function Prototypes
 */
int printQuaternionDriftAfterXMin(unsigned int time_min);
int printForImuViewer(char printQuat, char printPos, unsigned int numberOfRuns);
int printDataWithoutDMP(short int sensors, unsigned int numberOfRuns);
int printDataUsingDMP(char initialCalibration, char dmpCalibration,
		unsigned int numberOfRuns);

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
	//status |= printDataWithoutDMP(SENSORS_ALL, DATA_NO_DMP_RUNS);

	//Print Data with DMP without initial or DMP gyro calibration
	//status |= printDataUsingDMP(0, 0, DATA_WITH_DMP_RUNS);

	//Print Data with DMP with initial calibration but no DMP gyro calibration
	//status |= printDataUsingDMP(1, 0, DATA_WITH_DMP_RUNS);

	//Print Data with DMP with and DMP gyro calibration
	//status |= printDataUsingDMP(1, 1, DATA_WITH_DMP_RUNS);

	//Print Quaternions to Serial Port
	//status |= printForImuViewer(1, 0, QUAT_DISPLAY_RUNS);

	//Quaternion Drift
	//status |= printQuaternionDriftAfterXMin(QUAT_DRIFT_MIN);

	//Print Quaternions and Position
	status |= printForImuViewer(1, 1, QUAT_DISPLAY_RUNS);

	//PWM Test
	//status = pwmTest();

	//Done?
	if (status == XST_SUCCESS) {
		myprintf(".........Success...........\n\r");
	} else {
		myprintf(".........Failure...........\n\r");
	}

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

/*
 * Prints Quaternions and/or Position for IMU Viewer
 * In: number of runs (if 0 --> print forever)
 * Returns 0 if successful
 */
int printForImuViewer(char printQuat, char printPos, unsigned int numberOfRuns) {
	//Variables
	int cnt = 0, status;
	u32 intrpt;

	//Init
	myprintf(".........Configure MPU and DMP...........\n\r");
	status = configureDMP(FEATURES_CAL, DMP_FIFO_RATE);
	if (status != XST_SUCCESS) {
		return status;
	}

	//Calibrate
	myprintf(".........Calibrate...........\n\r");
	status = calibrateGyrAcc();
	if (status != XST_SUCCESS) {
		return status;
	}

	//Print Quaternions to Serial Port
	myprintf(".........Print for IMU Viewer...........\n\r");
	for (cnt = 0; cnt <= numberOfRuns; cnt++) {
		//Reset Status
		status = XST_SUCCESS;

		//Wait for Interrupt Interrupt
		if (waitForInterrupt()) {
			//Update Data
			status = updateData();
			if (status == XST_SUCCESS) {

				//Print
				status = printforDisplay(printQuat, printPos);

				//Print new line
				if (status == XST_SUCCESS) {
					printf("\n\r");
				}
			}
		}

		//Make sure only successful prints count
		if (status != XST_SUCCESS || numberOfRuns < 1) {
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
int printDataUsingDMP(char initialCalibration, char dmpCalibration,
		unsigned int numberOfRuns) {
//Variables
	int status, cnt;
	u32 intrpt;

//Calibrate if required
	if (initialCalibration) {
		myprintf(".........Calibration...........\n\r");
		status = calibrateGyrAcc();
		if (status != XST_SUCCESS) {
			return status;
		}
	}

//En- or disable dynamic gyro calibration
	status = dmpGyroCalibration(dmpCalibration);
	if (status != XST_SUCCESS) {
		return status;
	}

//Get Data with DMP
	myprintf(".........With DMP...........\n\r");
	for (cnt = 0; cnt <= numberOfRuns; cnt++) {
		//Wait for Interrupt Interrupt
		if (waitForInterrupt()) {
			//Update Data
			status = updateData();
			if (status == XST_SUCCESS) {

				//Print
				status = printDataWithDMP();
			}
		}

		//Decrease count if not successful
		if (status != XST_SUCCESS || numberOfRuns == 0) {
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
		for (i = 0; i <= 10000; i++) {
			;
		}
	}
	return XST_SUCCESS;
}
