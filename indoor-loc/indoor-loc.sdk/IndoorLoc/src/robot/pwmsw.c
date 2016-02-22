/*
 * pwmsw.c: Setting PWM pulses
 * Author: Stephanie Kunkel
 */

/*
 * Includes
 */
#include "pwmsw.h"

/*
 * Global Variables
 */
u32 stepAmount = PWM_STEPS;
u32 valInit = PWM_VAL_INIT;

/*
 * Arrays for faster access
 */
static const u32 pwmValRegister[NUMBER_OF_JOINTS] = { PWM0_VAL_REG, PWM1_VAL_REG, PWM2_VAL_REG,
PWM3_VAL_REG, PWM4_VAL_REG, PWM5_VAL_REG };
static const u32 pwmStepsRegister[NUMBER_OF_JOINTS] = { PWM0_STEPS_REG, PWM1_STEPS_REG,
PWM2_STEPS_REG, PWM3_STEPS_REG, PWM4_STEPS_REG, PWM5_STEPS_REG };
static const char* jointName[NUMBER_OF_JOINTS] = { "Base", "Shoulder", "Elbow", "Wrist",
		"Thumb", "Finger" };
static const Joint jointVal[NUMBER_OF_JOINTS] = { base, shoulder, elbow, wrist, thumb, finger };

/*
 * Function Prototypes
 */
float moveToAbsAngle(Joint joint, float dgr);
float getAbsAngle(Joint joint);
float valToAngle(Joint joint, u32 value);
u32 angleToValue(Joint joint, float dgr);
u32 moveTo(Joint joint, u32 value);
int moveTowards(Joint joint, u32 value);
u32 getLowerPwmLimit(Joint joint);
u32 getUpperPwmLimit(Joint joint);
short getPosMvDir(Joint joint);
u32 getValReg(Joint joint);
short getIndexForReg(u32 reg);
short getIndexOfJoint(Joint joint);
int reset();
u32 writePwmReg(u32 reg, u32 value);
u32 readPwmReg(u32 reg);

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

////90°-0°-90°-0°
//	Joint joint = wrist;
//	float a1 = 0.0;
//	float a2 = -90.0;
//	moveToAbsAngle(joint, a1);
//	sleep(1);
//	moveToAbsAngle(joint, a2);
//	sleep(1);
//	moveToAbsAngle(joint, a1);
//	return PWM_SUCCESS;

	//Invalid Values
//	setValue(finger, 2750);
//	setValue(finger, PWM_VAL_INIT);
//	setValue(thumb, 250);
//	setValue(thumb, PWM_VAL_INIT);
//	setValue(wrist, 3000);
//	setValue(wrist, PWM_VAL_INIT);
//	setValue(elbow, 0);
//	setValue(elbow, PWM_VAL_INIT);
//	setValue(shoulder, 3000);
//	setValue(shoulder, PWM_VAL_INIT);
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

		moveTo(jointVal[reg], val);

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
		moveTo(jointVal[reg], val);

		//Get and print current value
		val = readPwmReg(pwmValRegister[reg]);
		myprintf("Value of %s: %d (%fdgrs)\r\n", jointName[reg], val,
				radToDeg(valToAngle(jointVal[reg], val)));

		//Reset
		writePwmReg(pwmValRegister[reg], PWM_VAL_INIT);
	}

	//finish
	return PWM_SUCCESS;
}

/*
 * Move to Absolute Angle
 */
float moveToAbsAngle(Joint joint, float dgr) {
	//Variables
	u32 val, realVal;
	float angle;

	//Get final steps
	val = angleToValue(joint, dgr);

	//Move
	realVal = moveTo(joint, val);

	//Get Current Angle
	angle = valToAngle(joint, realVal);

	//Return
	return angle;
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
		if (equal_f(dgr, (DGR_NEG_LIMIT / 2.0))
				|| equal_f(dgr, (DGR_POS_LIMIT / 2.0))
				|| (dgr > (DGR_NEG_LIMIT / 2.0) && dgr < (DGR_POS_LIMIT / 2.0))) { // -90.0 <= dgr <= 90.0
			m = 100.0 / 9.0;
			n = 1500; //500
		} else {
			//Get m
			m = 50.0 / 9.0;

			//Get n
			if (dgr < (DGR_NEG_LIMIT / 2)) {
				n = -2000;
			} else { //dgr > 90
				n = 1000;
			}
		}
	} else { //other joints
		if (equal_f(dgr, (DGR_NEG_LIMIT / 2.0))
				|| equal_f(dgr, (DGR_POS_LIMIT / 2.0))
				|| (dgr > (DGR_NEG_LIMIT / 2.0) && dgr < (DGR_POS_LIMIT / 2.0))) { // -90.0 <= dgr <= 90.0
			m = -100.0 / 9.0;
			n = 1500;
		} else {
			//Get m
			m = -50.0 / 9.0;

			//Get n
			if (dgr < (DGR_NEG_LIMIT / 2)) {
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
 * Returns PWM value
 */
u32 moveTo(Joint joint, u32 value) {
	//Variables
	int diff;
	u32 limit;

	//Make sure value is smaller than upper limit
	limit = getUpperPwmLimit(joint);
	if (value > limit) {
		value = limit;
	}

	//Make sure value is bigger than lower limit
	limit = getLowerPwmLimit(joint);
	if (value < limit) {
		value = limit;
	}

	//Move
	do {
		diff = moveTowards(joint, value);
	} while (diff != 0);

	//Return
	return value;
}

/*
 * Set Angle for Joint
 * In: Joint, angle in dgr
 */
float setAngle(Joint joint, float angle) {
	//Variables
	u32 value, realValue;
	float realAngle;

	//Convert angle to value
	value = angleToValue(joint, angle);

	// Set Value
	realValue = setValue((joint), value);

	//Compute Real angle
	realAngle = valToAngle(joint, realValue);

	//Return
	return realAngle;
}

/*
 * Set Value for Joint
 * In: Joint, value
 * Returns current value
 */
u32 setValue(Joint joint, u32 value) {
	//Variables
	u32 limit;
	u32 currVal = 0;

	//Make sure value is smaller than upper limit
	limit = getUpperPwmLimit(joint);
	if (value > limit) {
		value = limit;
	}

	//Make sure value is bigger than lower limit
	limit = getLowerPwmLimit(joint);
	if (value < limit) {
		value = limit;
	}

	//Set Value
	currVal = writePwmReg(getValReg(joint), value);

	//Return Value
	return currVal;
}

/*
 * Move one towards specified value
 * In: Joint, value
 * Returns difference between current and specified value (0 if value has been reached)
 */
int moveTowards(Joint joint, u32 value) {
	//Variables
	u32 currValue;

	//Make sure value is positive
	if (value < PWM_VAL_MIN) {
		value = PWM_VAL_MIN;
	}

	//Make sure value is in range
	if (value > PWM_VAL_MAX) {
		value = PWM_VAL_MAX;
	}

	//Get current value
	currValue = readPwmReg(getValReg(joint));

	//Move in negative direction
	if (currValue < value) {
		//Compute new value
		//2 steps at a time for angles higher than (-)90°
		if (currValue < PWM_VAL_LOW || currValue > PWM_VAL_HIGH) {
			currValue += 2;

			//Correct value it is is out of range now
			if (currValue > PWM_VAL_MAX) {
				currValue = PWM_VAL_MAX;
			} else if (currValue < PWM_VAL_MIN) {
				currValue = PWM_VAL_MIN;
			}
		} else {
			currValue++;
		}
	}

	//Move in positive direction
	else if (currValue > value) {
		//Compute new value
		//2 steps at a time for angles higher/lower than +/-90°
		if (currValue < PWM_VAL_LOW || currValue > PWM_VAL_HIGH) {
			currValue -= 2;

			//Correct value it is is out of range now
			if (currValue > PWM_VAL_MAX) {
				currValue = PWM_VAL_MAX;
			} else if (currValue < PWM_VAL_MIN) {
				currValue = PWM_VAL_MIN;
			}
		} else {
			currValue--;
		}

	}

	//Set new value and get real value
	currValue = setValue(joint, currValue);

	//Return
	return (value - currValue);
}

/*
 * Get Upper PWM Limit
 * Thumb, Finger and Shoulder cannot move more than -90°
 * Base can move up to -180°
 * Elbow and wrist can move up to -270°
 */
u32 getLowerPwmLimit(Joint joint) {
	switch (joint) {
	case thumb:
	case finger:
	case shoulder:
		return PWM_VAL_LOW;
	case base:
		return PWM_VAL_MIN;
	case elbow:
	case wrist:
		return PWM_LIMIT_LOW;
	default:
		return PWM_VAL_INIT;
	}
}

/*
 * Get Upper PWM Limit
 * Thumb, Finger and Shoulder cannot move more than 90°
 * Base can move up to 180°
 * Elbow and wrist can move up to 270°
 */
u32 getUpperPwmLimit(Joint joint) {
	switch (joint) {
	case thumb:
	case finger:
	case shoulder:
		return PWM_VAL_HIGH;
	case base:
		return PWM_VAL_MAX;
	case elbow:
	case wrist:
		return PWM_LIMIT_HIGH;
	default:
		return PWM_VAL_INIT;
	}
}

/*
 * Get positive moving direction for Joint
 * In: Joint
 * Returns sign for positive moving direction  (0 if Joint is unknown)
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
 * Returns register (-1 if Joint is unknown)
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
 * Get index for pwm register
 * In: register
 * Returns internal index (-1 if reg is unknown)
 */
short getIndexForReg(u32 reg) {
	switch (reg) {
	case BASE_REG:
		return BASE_INDEX;
	case SHOULDER_REG:
		return SHOULDER_INDEX;
	case ELBOW_REG:
		return ELBOW_INDEX;
	case WRIST_REG:
		return WRIST_INDEX;
	case THUMB_REG:
		return THUMB_INDEX;
	case FINGER_REG:
		return FINGER_INDEX;
	default:
		return -1;
	}
}

/*
 * Get index of Joint
 * In: Joint
 * Returns internal index (-1 if Joint is unknown)
 */
short getIndexOfJoint(Joint joint) {
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
int reset() {
//Variables
	int i;
	u32 value;

//Move to initial setting
	for (i = 0; i < NUMBER_OF_JOINTS; i++) {
		value = moveTo(jointVal[i], PWM_VAL_INIT);

		if (value != PWM_VAL_INIT) {
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
 * Write to Register of PWM Module
 * In: Register, Value
 * Returns current value
 */
u32 writePwmReg(u32 reg, u32 value) {
	//Variables
	u32 currentValue = 0;

	//Write value to register
	PWM_mWriteReg(PWMADDRESS, reg, value);

	//Read current value
	currentValue = readPwmReg(reg);

	//Return current value
	return currentValue;
}

/*
 * Read all PWM Register Values
 * Out: array of values
 */
void getPwmRegValues (uint32_t values[NUMBER_OF_JOINTS]){
	//Variables
	int i;

	//Get Values
	for (i = 0; i < NUMBER_OF_JOINTS; i++){
		values[i] = readPwmReg(getValReg(pwmValRegister[i]));
	}
}

/*
 * Read from Register of PWM Module
 * In: Register
 * Returns content of register
 */
u32 readPwmReg(u32 reg) {
	return PWM_mReadReg(PWMADDRESS, reg);
}
