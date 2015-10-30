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

#include <stdio.h>
#include <stdlib.h>
#include "platform.h"
#include "pwmsw.h"
#include "imu.h"
#include <unistd.h>

int main() {
	//Variables
	int status;

	//Init Platform
	init_platform();

	//IMU
	u8 imuAddr;
	unsigned char* data = (unsigned char*) malloc(8);
	*data = '0';
	u16 data_16;

	//1. Init (Set Address, etc.)
	status = imuInit(&imuAddr);
	if (status != XST_SUCCESS) {
		xil_printf("run.c: Error in IMU Initialization.\n\r");
		return 0;
	}

	//2. Power up
	status = imuI2cWrite(imuAddr, MPU9150_PWR_MGMT_1, sizeof(data), data);
	if (status != XST_SUCCESS) {
		xil_printf("run.c: Error in IMU Power Up.\n\r");
		return 0;
	}

	//3. Read Device ID
//	status = imuI2cRead(imuAddr, MPU9150_WHO_AM_I, 8, data);
//	if (status != XST_SUCCESS) {
//		xil_printf("run.c: Error in reading IMU device ID.\n\r");
//		return 0;
//	} else {
//		xil_printf("IMU Device ID: 0x%x\r\n", *data);
//	}

//4. Read Data
	int i;
	for (i = 0; i <= 50; i++) {
		//Gyro
		status = readDoubleReg(MPU9150_GYRO_XOUT_H, MPU9150_GYRO_XOUT_L,
				&data_16);
		if (status != XST_SUCCESS) {
			xil_printf("run.c: Error in reading IMU device ID.\n\r");
			return 0;
		} else {
			xil_printf("IMU Gyro X: 0x%x\r\n", data_16);
		}

		//Acc
		status = readDoubleReg(MPU9150_ACCEL_XOUT_H, MPU9150_ACCEL_XOUT_L,
				&data_16);
		if (status != XST_SUCCESS) {
			xil_printf("run.c: Error in reading IMU device ID.\n\r");
			return 0;
		} else {
			xil_printf("IMU Acc X: 0x%x\r\n", data_16);
		}

		//Temp
		status = readDoubleReg(MPU9150_TEMP_OUT_H, MPU9150_TEMP_OUT_L,
				&data_16);
		if (status != XST_SUCCESS) {
			xil_printf("run.c: Error in reading IMU device ID.\n\r");
			return 0;
		} else {
			xil_printf("IMU Temp: 0x%x\r\n", data_16);
		}
		sleep(1);
	}

	//PWM
	pwm();

	/*while (1){
	 print("Hello World\n\r");
	 }*/

	cleanup_platform();
	return 0;
}
