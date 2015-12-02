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
#include "xtime_l.h"

/*
 * Defines
 */
#define DATA_NO_DMP_RUNS 	10
#define DATA_WITH_DMP_RUNS 	10
#define PWM_RUNS 			10

/*
 * Main
 */
int main() {
	//Variables
	int i = 0, cnt = 0, status;
	short int sensors = SENSORS_ALL;

	//Init Platform
	init_platform();

	//Print Output Separator
	myprintf("...............................\r\n");

//	//Quaternion Drift
//	myprintf(".........Drift...........\n\r");
//	printQuatDrift(1);

//Calibrate
	myprintf(".........Calibration...........\n\r");
	status = calibrateGyrAcc();
	if (status != XST_SUCCESS) {
		myprintf("Calibration failed.\r\n");
	}

	//Enable dynamic gyro calibration
	status = dmpGyroCalibration(1);
	if (status != XST_SUCCESS) {
		myprintf("Could not enable DMP dynamic gyro calibration.\r\n");
	}

	//Print Quaternions to Serial Port
	for (cnt = 0; cnt <= 1000;) {
		status = printQuatForDisplay();
		if (status == XST_SUCCESS) {
			//cnt++;
		}
		//usleep(100);
		for (i = 0; i <= 10000; i++) {
			;
		}
	}

////Get Data without DMP
//	myprintf(".........Without DMP...........\n\r");
//	for (cnt = 0; cnt <= DATA_NO_DMP_RUNS; cnt++) {
//		//Print
//		status = printDataNoDMP(&sensors);
//
//		//Decrease count if not successful
//		if (status != XST_SUCCESS) {
//			cnt--;
//		}
//
//		//Wait
//		for (i = 0; i <= 10000; i++) {
//			;
//		}
//	}

////Get Data with DMP
//	myprintf(".........With DMP...........\n\r");
//	for (cnt = 0; cnt <= DATA_NO_DMP_RUNS; cnt++) {
//		//Print
//		status = printDataWithDMP();
//
//		//Decrease count if not successful
//		if (status != XST_SUCCESS) {
//			cnt--;
//		} else {
//			usleep(1);
//		}
//
//		//Wait
//		for (i = 0; i <= 10000; i++) {
//			;
//		}
//	}
//
//	//Sleep
//	sleep(1);
//
////Calibrate
//	myprintf(".........Calibration...........\n\r");
//	status = calibrateGyrAcc();
//	if (status != XST_SUCCESS) {
//		myprintf("Calibration failed.\r\n");
//	}
//
//	//Get Data with DMP
//	myprintf(".........With DMP...........\n\r");
//	for (cnt = 0; cnt <= DATA_NO_DMP_RUNS; cnt++) {
//		//Print
//		status = printDataWithDMP();
//
//		//Decrease count if not successful
//		if (status != XST_SUCCESS) {
//			cnt--;
//		}
//
//		//Wait
//		for (i = 0; i <= 10000; i++) {
//			;
//		}
//
//		//Print forever
//		cnt--;
//	}

//PWM
//pwm(PWM_RUNS);

//Stay in here
	while (1) {
		;
	}

//Finish
	cleanup_platform();
	return 0;
}
