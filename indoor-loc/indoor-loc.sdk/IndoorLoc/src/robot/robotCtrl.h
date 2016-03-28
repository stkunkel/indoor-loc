/*
 * robotCtrl.h: Robot control and movement data collection
 * Author: Stephanie Kunkel
 */

#ifndef ROBOTCTRL_H_
#define ROBOTCTRL_H_

//Includes
#include "xstatus.h"
#include "../program_parameters.h"
#include "../dataBuffer.h"
#include "pwmsw.h"
#include "../imu/mpu.h"
#include "../zedboard/print_utils.h"
#include "../zedboard/gpio_utils.h"

//Parameters
#define HSS_422_TIME_FOR_90_DGRS		500//400//333 //ms
#define STEPS_BEFORE_NEW_ANGLE			5


//Functions
int collectRobotMvmtData(unsigned int sampleTime, unsigned int calibrationTime,
		bool collect);

#endif /* ROBOTCTRL_H_ */
