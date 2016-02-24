/*
 * dataBuffer.c: Robot control and movement data collection
 * Author: Stephanie Kunkel
 */

#ifndef DATABUFFER_H_
#define DATABUFFER_H_

//Includes
#include "../imu/mpu.h"
//#include "pwmsw.h"
#include "../zedboard/print_utils.h"
#include "../zedboard/timer_int.h"

//Defines
#define BUF_ADDR			0x7000000
#define NUMBER_OF_JOINTS	6

//Struct for Data Transmission
typedef struct {
	uint32_t pwmValues[NUMBER_OF_JOINTS];
	MpuRegisterData mpuData;
} RobotMpuData;

//Functions
void storeInBuff(RobotMpuData* data);
int transmitBuf();
void resetBuff();

#endif /* DATABUFFER_H_ */
