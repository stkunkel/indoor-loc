/*
 * pwmsw.c: Setting PWM pulses
 * Author: Stephanie Kunkel
 */

#define PWMADDRESS  0x43c00000
#define PWMSTEPS	10

#include <stdio.h>
#include <xil_io.h>
#include <unistd.h>
#include "../platform.h"
#include "pwm.h"
#include "pwmsw.h"

int pwm(int amount) {
	//variables
		unsigned int a, i;
		u32 val;

		//set steps to PWMSTEPS
		PWM_mWriteReg(PWMADDRESS, PWM_S00_AXI_SLV_REG6_OFFSET, PWMSTEPS);
		PWM_mWriteReg(PWMADDRESS, PWM_S00_AXI_SLV_REG7_OFFSET, PWMSTEPS);
		PWM_mWriteReg(PWMADDRESS, PWM_S00_AXI_SLV_REG8_OFFSET, PWMSTEPS);
		PWM_mWriteReg(PWMADDRESS, PWM_S00_AXI_SLV_REG9_OFFSET, PWMSTEPS);
		PWM_mWriteReg(PWMADDRESS, PWM_S00_AXI_SLV_REG10_OFFSET, PWMSTEPS);
		PWM_mWriteReg(PWMADDRESS, PWM_S00_AXI_SLV_REG11_OFFSET, PWMSTEPS);

		//main loop
		for (a = 0; a <= amount; a++) {
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
			}
		}

		//finish
		return 0;
}
