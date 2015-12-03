/*
 * pwmsw.c: Setting PWM pulses
 * Author: Stephanie Kunkel
 */

/*
 * Includes
 */
#include <stdio.h>
#include <xil_io.h>
#include <unistd.h>
#include "../platform.h"
#include "../print_utils.h"
#include "pwm.h"
#include "pwmsw.h"

/*
 * Global Variables
 */
u32 stepAmount = 0;

/*
 * Function Prototypes
 */
int moveToAbsAngle(Joint joint, float dgr, unsigned int time_ms);
float getAbsAngle(Joint joint);
int setAbsAngle(Joint joint, float dgr);
float stepsToAngle(u32 steps);
u32 angleToSteps(float dgr);
u32 getValReg(Joint joint);
u32 readPwmReg(u32 reg);
int writePwmReg(u32 reg, u32 value);

/*
 * Functions
 */

/*
 * PWM Test
 * Returns 0 if successful
 */
int pwmTest() {
	//variables
	unsigned int a, i;
	u32 val;

	//Init PWM

	//finish
	return PWM_SUCCESS;
}

/*
 * Move to Absolute Angle
 */
int moveToAbsAngle(Joint joint, float dgr, unsigned int time_ms) {
	//Variables
	int status = PWM_SUCCESS;
	u32 val_goal;
	u32 val_cur;
	u32 sleeptime;

	//Get current steps
	val_cur = getValReg(joint);

	//Get final steps
	val_goal = angleToSteps(dgr);

	//Compute sleep time btw. steps
	sleeptime = (u32) (US_PER_DGR * 180 / stepAmount);

	//Go step by step
	if (val_goal > val_cur) {
		while (val_goal > val_cur) {
			//Decrease value
			val_cur--;
			status = writePwmReg(getValReg(joint), val_cur);

			//Return if unsuccessful
			if (status != PWM_SUCCESS) {
				break;
			}

			//Sleep
			usleep(sleeptime);
		}
	} else {
		while (val_goal < val_cur) {
			//Increase value
			val_cur++;
			status = writePwmReg(getValReg(joint), val_cur);

			//Return if unsuccessful
			if (status != PWM_SUCCESS) {
				break;
			}

			//Sleep
			usleep(sleeptime);
		}
	}

	//Return
	return status;
}

/*
 * Get Absolute Angle
 * In: Joint
 * Returns angle
 */
float getAbsAngle(Joint joint) {
	//Variables
	u32 steps;

	//Get Steps
	steps = readPwmReg(getValReg(joint));

	//Convert to angle and return
	return stepsToAngle(steps);
}

/*
 * Set Absolute Angle
 * In: Joint, angle in degrees
 * Returns 0 if successful
 */
int setAbsAngle(Joint joint, float dgr) {
	//Variables
	u32 steps;
	int status;

	//Compute steps from angle (0 --> 0dgr, steps --> 180dgr)
	steps = angleToSteps(dgr);

	//Write to Register
	status = writePwmReg(getValReg(joint), steps);

	//Return
	return status;
}

/*
 * Convert Steps to Absolute Angle
 * In: Steps
 * Returns Angle in Degrees
 */
float stepsToAngle(u32 steps) {
	return steps / stepAmount * 180.0;
}

/*
 * Convert Absolute Angle to Steps
 * In: Angle in Degrees
 * Returns Steps
 */
u32 angleToSteps(float dgr) {
	return (u32) (dgr / 180 * stepAmount);
}

/*
 * Get value register for Joint
 * In: Joint
 * Returns register
 */
u32 getValReg(Joint joint) {
	switch (joint) {
	case base:
		return BASE_REG;
	case shoulder:
		return SHOULDER_REG;
	case elbow:
		return ELBOW_REG;
	case wrist:
		return WRIST_REG;
	case thumb:
		return THUMB_REG;
	case finger:
		return FINGER_REG;
	default:
		return -1;
	}
}

/*
 * Initialize PWM Module
 * In: Amount of steps
 * Returns 0 if successful
 */
int initPWM(u32 steps) {
	//Variables
	u32 val = (u32) steps/2;

	//Set steps
	writePwmReg(PWM_S00_AXI_SLV_REG6_OFFSET, steps);
	writePwmReg(PWM_S00_AXI_SLV_REG7_OFFSET, steps);
	writePwmReg(PWM_S00_AXI_SLV_REG8_OFFSET, steps);
	writePwmReg(PWM_S00_AXI_SLV_REG9_OFFSET, steps);
	writePwmReg(PWM_S00_AXI_SLV_REG10_OFFSET, steps);
	writePwmReg(PWM_S00_AXI_SLV_REG11_OFFSET, steps);

	//Set initial val --> 90 dgrs
	writePwmReg(PWM_S00_AXI_SLV_REG0_OFFSET, val);
	writePwmReg(PWM_S00_AXI_SLV_REG1_OFFSET, val);
	writePwmReg(PWM_S00_AXI_SLV_REG2_OFFSET, val);
	writePwmReg(PWM_S00_AXI_SLV_REG3_OFFSET, val);
	writePwmReg(PWM_S00_AXI_SLV_REG4_OFFSET, val);
	writePwmReg(PWM_S00_AXI_SLV_REG5_OFFSET, val);

	//Remember steps
	stepAmount = steps;

	//Return
	return PWM_SUCCESS;
}

/*
 * Read from Register of PWM Module
 * In: Register
 * Returns content of register
 */
u32 readPwmReg(u32 reg) {
	return PWM_mReadReg(PWMADDRESS, reg);
}

/*
 * Write to Register of PWM Module
 * In: Register, Value
 * Returns 0 if successful
 */
int writePwmReg(u32 reg, u32 value) {
	//Write value to register
	PWM_mWriteReg(PWMADDRESS, reg, value);

	//Check if value written correctly
	if (readPwmReg(reg) == value) {
		return PWM_SUCCESS;
	} else {
		return PWM_REG_CONT_NO_MATCH;
	}
}