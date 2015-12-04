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
float valToRadians(Joint joint, u32 value);
u32 toValue(Joint joint, float dgr);
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
	u32 steps = PWM_STEPS;
	u32 val = PWM_VAL_INIT;
	int status = PWM_SUCCESS;
	int reg = 0, i;

	//Reset
	for (reg = 0; reg < NUMBER_OF_JOINTS; reg++) {
		moveTo(jointVal[reg], 1400);
	}

	//Init PWM
	status = reset();
	if (status != PWM_SUCCESS) {
		return status;
	}

//Get Reg Values
	for (reg = 4; reg < NUMBER_OF_JOINTS; reg++) {
		//Get current value
		val = readPwmReg(pwmValRegister[reg]);

		//Print current value
		myprintf("Steps of %s: %d\r\n", jointName[reg],
				readPwmReg(pwmStepsRegister[reg]));
		myprintf("Value of %s: %d (%fdgrs)\r\n", jointName[reg], val,
				radToDeg(valToRadians(jointVal[reg], val)));

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
				radToDeg(valToRadians(jointVal[reg], val)));

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
				radToDeg(valToRadians(jointVal[reg], val)));

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
	val = toValue(joint, dgr);

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
	return valToRadians(joint, steps);
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
float valToRadians(Joint joint, u32 value) {
	//Variables
	float rad;

	//Scale
	//value = (u32) (value * PWM_STEPS / stepAmount);

	//Compute Angle
	if (joint == wrist || joint == finger) {
		rad = asin((value - 1500.0) / 3000.0);
	} else {
		rad = acos((value / 3000.0 - 0.5)) - M_PI_2;
	}

	//Return
	return rad;
}

/*
 * Convert Absolute Angle to Absolute Step Value
 * In: Joint Angle in Degrees
 * Returns Step value
 */
u32 toValue(Joint joint, float dgr) {
	//Variables
	int factor;
	int limit;
	float remainder;
	u32 value;

	//Make sure dgr is btw. -180 and +180 dgrs
	if (dgr > DGR_POS_LIMIT || dgr < DGR_NEG_LIMIT) {
		if (dgr > DGR_POS_LIMIT) {
			limit = DGR_POS_LIMIT;
		} else {
			limit = DGR_NEG_LIMIT;
		}

		factor = (int) dgr / limit;
		remainder = dgr - factor * limit;
		dgr = remainder - limit;
	}

	//Compute value
	if (joint == wrist || joint == finger) {
		value = (u32) (50.0 * (180.0 + dgr) / 3.0);
	} else {
		value = (u32) (-50.0 * (180.0 - dgr) / 3.0);
	}

	//Scale value
	value = (u32) (value / PWM_STEPS * stepAmount);

	//Return
	return value;
}

/*
 * Move to specified value
 * In: Joint, value
 * Returns 0 if successful
 */
int moveTo(Joint joint, u32 value) {
	//Variables
	u32 currValue, newValue;

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
