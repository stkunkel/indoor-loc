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
 * helloworld.c: simple test application
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

#define PWMADDRESS  0x43c00000
#define PWMSTEPS	10

#include <stdio.h>
#include <xil_io.h>
#include <unistd.h>
#include "platform.h"
#include "pwm.h"
#include "pwmsw.h"

void xil_print(char *str);

int pwm() {
	//variables
		unsigned int i;
		u32 val;

		//init
		init_platform();

		//set steps to PWMSTEPS
		PWM_mWriteReg(PWMADDRESS, PWM_S00_AXI_SLV_REG6_OFFSET, PWMSTEPS);
		PWM_mWriteReg(PWMADDRESS, PWM_S00_AXI_SLV_REG7_OFFSET, PWMSTEPS);
		PWM_mWriteReg(PWMADDRESS, PWM_S00_AXI_SLV_REG8_OFFSET, PWMSTEPS);
		PWM_mWriteReg(PWMADDRESS, PWM_S00_AXI_SLV_REG9_OFFSET, PWMSTEPS);
		PWM_mWriteReg(PWMADDRESS, PWM_S00_AXI_SLV_REG10_OFFSET, PWMSTEPS);
		PWM_mWriteReg(PWMADDRESS, PWM_S00_AXI_SLV_REG11_OFFSET, PWMSTEPS);

		//main loop
		while (1) {
			//LED1
			PWM_mWriteReg(PWMADDRESS, PWM_S00_AXI_SLV_REG1_OFFSET,
					PWMSTEPS/2);

			//LED2
			PWM_mWriteReg(PWMADDRESS, PWM_S00_AXI_SLV_REG2_OFFSET,
					PWMSTEPS/10);

			//LED3
			PWM_mWriteReg(PWMADDRESS, PWM_S00_AXI_SLV_REG3_OFFSET,
					PWMSTEPS);

			//LED0
			for (i = 0; i <= PWMSTEPS; i = i + PWMSTEPS / 5) {
				PWM_mWriteReg(PWMADDRESS, PWM_S00_AXI_SLV_REG0_OFFSET, i);
//				sleep(1);
//				val = (u32) PWM_mReadReg(PWMADDRESS,
//						PWM_S00_AXI_SLV_REG0_OFFSET);
//				xil_printf("Steps: %d\r\n", val);
			}
		}

		//finish
		cleanup_platform();
		return 0;
}
