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
 * Arrays for faster access
 */
static const u32 pwmValRegister[6] = { PWM0_VAL_REG, PWM1_VAL_REG, PWM2_VAL_REG,
PWM3_VAL_REG, PWM4_VAL_REG, PWM5_VAL_REG };
static const u32 pwmStepsRegister[6] = { PWM0_STEPS_REG, PWM1_STEPS_REG,
PWM2_STEPS_REG, PWM3_STEPS_REG, PWM4_STEPS_REG, PWM5_STEPS_REG };
static const char* jointName[6] = { "Base", "Shoulder", "Elbow", "Wrist",
		"Thumb", "Finger" };
static const short sign[6] = { -1, -1, -1, 1, -1, 1 };

/*
 * Function Prototypes
 */
int moveToAbsAngle(Joint joint, float dgr, unsigned int time_ms);
float getAbsAngle(Joint joint);
int setAbsAngle(Joint joint, float dgr);
float stepsToAngle(u32 steps);
u32 angleToSteps(float dgr);
u32 getValReg(Joint joint);
int initPWM(u32 steps);
u32 readPwmReg(u32 reg);
int writePwmReg(u32 reg, u32 value);
u32 getIndex(Joint joint);

/*
 * Functions
 */

/*
 * PWM Test
 * Returns 0 if successful
 */
int pwmTest() {
	//variables
	u32 steps = 20000;
	u32 val = 1500;
	int status = PWM_SUCCESS;
	int reg = 0, i = 0;

//	//Init PWM
//	status = initPWM(steps);
//	if (status != PWM_SUCCESS) {
//		return status;
//	}

//Get Reg Values
	for (reg = 0; reg < 6; reg++) {
		writePwmReg(pwmValRegister[reg], 1500);

		//Get current value
		val = readPwmReg(pwmValRegister[reg]);

		//Print current value
		myprintf("Steps of %s: %d\r\n", jointName[reg],
				readPwmReg(pwmStepsRegister[reg]));
		myprintf("Value of %s: %d\r\n", jointName[reg], val);

		//Set val to 0.5ms
		for (i = 0; i < 1000; i++) {
			val += sign[reg];
			writePwmReg(pwmValRegister[reg], val);
			usleep(4000);
		}

		//Set val back to 1.5ms
		for (i = 0; i < 1000; i++) {
			val -= sign[reg];
			writePwmReg(pwmValRegister[reg], val);
			usleep(4000);
		}

		//Get and print current value
		val = readPwmReg(pwmValRegister[reg]);
		myprintf("New Value of %s: %d\r\n", jointName[reg], val);
	}

	//finish
	return status;
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
int initPWM(u32 steps) { //TODO: Keep angle same
	//Variables
	u32 val = (u32) steps / 2;

	//Set initial val --> 90 dgrs
	writePwmReg(PWM_S00_AXI_SLV_REG0_OFFSET, val);
	writePwmReg(PWM_S00_AXI_SLV_REG1_OFFSET, val);
	writePwmReg(PWM_S00_AXI_SLV_REG2_OFFSET, val);
	writePwmReg(PWM_S00_AXI_SLV_REG3_OFFSET, val);
	writePwmReg(PWM_S00_AXI_SLV_REG4_OFFSET, val);
	writePwmReg(PWM_S00_AXI_SLV_REG5_OFFSET, val);

	//Set steps
	writePwmReg(PWM_S00_AXI_SLV_REG6_OFFSET, steps);
	writePwmReg(PWM_S00_AXI_SLV_REG7_OFFSET, steps);
	writePwmReg(PWM_S00_AXI_SLV_REG8_OFFSET, steps);
	writePwmReg(PWM_S00_AXI_SLV_REG9_OFFSET, steps);
	writePwmReg(PWM_S00_AXI_SLV_REG10_OFFSET, steps);
	writePwmReg(PWM_S00_AXI_SLV_REG11_OFFSET, steps);

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

/*
 * Get index of Joint
 * In: Joint
 * Returns internal index
 */
u32 getIndex(Joint joint) {
	switch (joint) {
	case base:
		return BASE_INDEX;
	case shoulder:
		return SHOULDER_INDEX;
	case elbow:
		return ELBOW_INDEX;
	case wrist:
		return WRIST_INDEX;
	case thumb:
		return THUMB_INDEX;
	case finger:
		return FINGER_INDEX;
	default:
		return -1;
	}
}
