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


//Functions
void storeInBuff(RobotMpuData* data);
int transmitBuf();
void resetBuff();

#endif /* DATABUFFER_H_ */
