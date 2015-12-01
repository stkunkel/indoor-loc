/******************************************************************************
 *
 * Copyright (C) 2009 - 2014 Xilinx, Inc.  All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * Use of the Software is limited solely to applications:
 * (a) running on a Xilinx device, or
 * (b) that interact with a Xilinx device through a bus or interconnect.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
 * OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * Except as contained in this notice, the name of the Xilinx shall not be used
 * in advertising or otherwise to promote the sale, use or other dealings in
 * this Software without prior written authorization from Xilinx.
 *
 ******************************************************************************/

/*
 * run.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

/*
 * Includes
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "platform.h"
#include "print_utils.h"
#include "mpu.h"
#include "pwmsw.h"
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

	//Quaternion Drift
	myprintf(".........Drift...........\n\r");
	printQuatDrift(10);

//////Calibrate
////	myprintf(".........Calibration...........\n\r");
////	status = calibrateGyrAcc();
////	if (status != XST_SUCCESS) {
////		myprintf("Calibration failed.\r\n");
////	}
//
////	//Enable dynamic gyro calibration
////	status = dmpGyroCalibration(1);
////	if (status != XST_SUCCESS) {
////		myprintf("Could not enable DMP dynamic gyro calibration.\r\n");
////	}
//
//	//Print Quaternions to Serial Port
//	for (cnt = 0; cnt <= 1000;) {
//		status = printQuatForDisplay();
//		if (status == XST_SUCCESS) {
//			//cnt++;
//		}
//		//usleep(100);
//		for (i = 0; i <= 10000; i++) {
//			;
//		}
//	}

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
