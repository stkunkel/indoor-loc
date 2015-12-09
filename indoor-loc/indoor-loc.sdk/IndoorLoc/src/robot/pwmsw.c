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
#include <math.h>
#include "../platform.h"
#include "../print_utils.h"
#include "pwm.h"
#include "pwmsw.h"

/*
 * Global Variables
 */
u32 stepAmount = PWM_STEPS;
u32 valInit = PWM_VAL_INIT;

/*
 * Arrays for faster access
 */
static const u32 pwmValRegister[6] = { PWM0_VAL_REG, PWM1_VAL_REG, PWM2_VAL_REG,
PWM3_VAL_REG, PWM4_VAL_REG, PWM5_VAL_REG };
static const u32 pwmStepsRegister[6] = { PWM0_STEPS_REG, PWM1_STEPS_REG,
PWM2_STEPS_REG, PWM3_STEPS_REG, PWM4_STEPS_REG, PWM5_STEPS_REG };
static const char* jointName[6] = { "Base", "Shoulder", "Elbow", "Wrist",
		"Thumb", "Finger" };
static const Joint jointVal[6] = { base, shoulder, elbow, wrist, thumb, finger };

/*
 * Function Prototypes
 */
int moveToAbsAngle(Joint joint, float dgr);
float getAbsAngle(Joint joint);
float valToAngle(Joint joint, u32 value);
u32 angleToValue(Joint joint, float dgr);
int moveTo(Joint joint, u32 value);
short getPosMvDir(Joint joint);
u32 getValReg(Joint joint);
short getIndex(Joint joint);
int reset();
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
	u32 val = PWM_VAL_INIT;
	int status = PWM_SUCCESS;
	int reg = 0;

	//Reset
	status = reset();
	if (status != PWM_SUCCESS) {
		return status;
	}

	//Debug
//	myprintf(("Degree -> Val:\r\n"));
//	for (reg = -720; reg <= 720; reg += 60) {
//		myprintf("J  : %d / %d\r\n", reg,
//				angleToValue(jointVal[4], (float) reg));
//		//myprintf("W/F: %d / %d\r\n", reg,
//		//		angleToValue(jointVal[5], (float) reg));
//	}

//	myprintf(("Val -> Degree:\r\n"));
//	for (reg = 0; reg <= 3100; reg += 100) {
//		myprintf("J  : %d / %f\r\n", reg, valToAngle(jointVal[4], reg));
//		myprintf("W/F: %d / %f\r\n", reg, valToAngle(jointVal[5], reg));
//	}
//	return 0;

//Get Reg Values
	for (reg = 0; reg < NUMBER_OF_JOINTS; reg++) {
		//Get current value
		val = readPwmReg(pwmValRegister[reg]);

		//Print current value
		myprintf("Steps of %s: %d\r\n", jointName[reg],
				readPwmReg(pwmStepsRegister[reg]));
		myprintf("Value of %s: %d (%fdgrs)\r\n", jointName[reg], val,
				radToDeg(valToAngle(jointVal[reg], val)));

		//Move 90dgrs
		if (jointVal[reg] == wrist || jointVal[reg] == finger) {
			val += 1000;
		} else {
			val -= 1000;
		}

		status = moveTo(jointVal[reg], val);

		//Get and print current value
		val = readPwmReg(pwmValRegister[reg]);
		myprintf("Value of %s: %d (%fdgrs)\r\n", jointName[reg], val,
				radToDeg(valToAngle(jointVal[reg], val)));

		//Move back 90dgrs
		if (jointVal[reg] == wrist || jointVal[reg] == finger) {
			val -= 1000;
		} else {
			val += 1000;
		}
		status = moveTo(jointVal[reg], val);

		//Get and print current value
		val = readPwmReg(pwmValRegister[reg]);
		myprintf("Value of %s: %d (%fdgrs)\r\n", jointName[reg], val,
				radToDeg(valToAngle(jointVal[reg], val)));

		//Reset
		writePwmReg(pwmValRegister[reg], 1500);
	}

	//finish
	return status;
}

/*
 * Move to Absolute Angle
 */
int moveToAbsAngle(Joint joint, float dgr) {
	//Variables
	int status = PWM_SUCCESS;
	u32 val;

	//Get final steps
	val = angleToValue(joint, dgr);

	//Move
	status = moveTo(joint, val);

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
	return valToAngle(joint, steps);
}

/*
 * Convert Degrees to Radians
 * In: Degrees
 * Returns Radians
 */
float degToRad(float deg) {
	return deg / 180.0 * M_PI;
}

/*
 * Convert Radians to Degrees
 * In: Radians
 * Returns Degrees
 */
float radToDeg(float rad) {
	return rad * 180.0 / M_PI;
}

/*
 * Convert Absolute Step Value to Absolute Angle
 * In: Joint, step value
 * Returns Angle in Degrees
 */
float valToAngle(Joint joint, u32 value) {
	//Variables
	float m;
	int n;

	//Make sure value is bigger than 0
	if (value < PWM_VAL_MIN) {
		value = PWM_VAL_MIN;
	}

	//Make sure value is max. 3000
	if (value > PWM_VAL_MAX) {
		value = PWM_VAL_MAX;
	}

	//Compute m and n
	if (value >= 500 && value <= 1500) {
		if (joint == wrist || joint == finger) {
			m = 9.0 / 100.0;
			n = -135;
		} else {
			m = -9.0 / 100.0;
			n = 135;
		}
	} else {
		if (joint == wrist || joint == finger) {
			//Get m
			m = 9.0 / 50.0;

			//Get n
			if (value < 500) {
				n = -180;
			} else { //value > 1500
				n = -360;
			}
		} else {
			//Get m
			m = -9.0 / 50.0;

			//Get n
			if (value < 500) {
				n = 180;
			} else { //value > 1500
				n = 360;
			}
		}

	}

	//Return
	return (m * value + n);
}

/*
 * Convert Absolute Angle to Absolute Step Value
 * In: Joint Angle in Degrees
 * Returns Step value
 */
u32 angleToValue(Joint joint, float dgr) {
	//Variables
	int factor;
	int limit;
	float remainder;
	float m;
	int n;
	float result;

	//Make sure dgr is btw. -180 and +180 dgrs
	if (dgr > DGR_POS_LIMIT || dgr < DGR_NEG_LIMIT) {
		if (dgr > DGR_POS_LIMIT) {
			//Compute Remainder
			limit = 2 * DGR_POS_LIMIT;
			factor = (int) dgr / limit;
			remainder = dgr - factor * limit;

			//Correct dgr
			if (remainder > DGR_POS_LIMIT) {
				dgr = remainder - limit;
			} else {
				dgr = remainder;
			}

		} else {
			//Compute Remainder
			limit = 2 * DGR_NEG_LIMIT;
			factor = (int) dgr / limit;
			remainder = dgr - factor * limit;

			//Correct dgr
			if (remainder < DGR_NEG_LIMIT) {
				dgr = remainder - limit;
			} else {
				dgr = remainder;
			}
		}

	}

	//Compute m and n for wrist or finger
	if (joint == wrist || joint == finger) {
		if (dgr >= DGR_NEG_LIMIT / 2.0 && dgr <= DGR_POS_LIMIT / 2.0) {
			m = 100.0 / 9.0;
			n = 500;
		} else {
			//Get m
			m = 50.0 / 9.0;

			//Get n
			if (dgr < DGR_NEG_LIMIT / 2) {
				n = -2000;
			} else { //dgr > 90
				n = 1000;
			}
		}
	} else { //other joints
		if (dgr >= DGR_NEG_LIMIT / 2.0 && dgr <= DGR_POS_LIMIT / 2.0) {
			m = -100.0 / 9.0;
			n = 1500;
		} else {
			//Get m
			m = -50.0 / 9.0;

			//Get n
			if (dgr < DGR_NEG_LIMIT / 2) {
				n = 2000;
			} else { //dgr > 90
				n = 1000;
			}
		}
	}

	//Return
	result = round(m * dgr + n);
	return (u32) result;
}

/*
 * Move to specified value
 * In: Joint, value
 * Returns 0 if successful
 */
int moveTo(Joint joint, u32 value) {
	//Variables
	u32 currValue;

	//Make sure value is positive
	if (value < 0) {
		value = 0;
	}

	//Make sure value is in range
	if (value > (u32) (PWM_VAL_MAX * stepAmount / PWM_STEPS)) {
		value = (u32) PWM_VAL_MAX / PWM_STEPS * stepAmount;
	}

	//Get start value
	currValue = readPwmReg(getValReg(joint));

	//Move in negative direction
	while (currValue < value) {
		//Compute new value
		currValue++;

		//Set new value
		writePwmReg(getValReg(joint), currValue);

		//Get new value
		currValue = readPwmReg(getValReg(joint));

		//Make movement smooth
		usleep(SLEEP_BTW_STEPS);
	}

	//Move in positive direction
	while (currValue > value) {
		//Compute new value
		currValue--;

		//Set new value
		writePwmReg(getValReg(joint), currValue);

		//Get new value
		currValue = readPwmReg(getValReg(joint));

		//Make movement smooth
		usleep(SLEEP_BTW_STEPS);
	}

	//Return
	return PWM_SUCCESS;
}

/*
 * Get positive moving direction for Joint
 * In: Joint
 * Returns sign for positive moving direction
 */
short getPosMvDir(Joint joint) {
	switch (joint) {
	case base:
		return BASE_SIGN;
	case shoulder:
		return SHOULDER_SIGN;
	case elbow:
		return ELBOW_SIGN;
	case wrist:
		return WRIST_SIGN;
	case thumb:
		return THUMB_SIGN;
	case finger:
		return FINGER_SIGN;
	default:
		return 0;
	}
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
 * Get index of Joint
 * In: Joint
 * Returns internal index
 */
short getIndex(Joint joint) {
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

/*
 * Reset Robot
 * Returns 0 if successful
 */
int reset() { //TODO: Keep angle same
	//Variables
	int status = PWM_SUCCESS, i;

	//Move to initial setting
	for (i = 0; i < NUMBER_OF_JOINTS; i++) {
		status = moveTo(jointVal[i], PWM_VAL_INIT);

		if (status != PWM_SUCCESS) {
			return PWM_FAILURE;
		}
	}

	//Remember steps and initial value
	stepAmount = PWM_STEPS;
	valInit = PWM_VAL_INIT;

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
