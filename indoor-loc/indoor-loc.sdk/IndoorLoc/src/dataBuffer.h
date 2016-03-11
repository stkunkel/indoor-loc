/*
 * dataBuffer.c: Robot control and movement data collection
 * Author: Stephanie Kunkel
 */

#ifndef DATABUFFER_H_
#define DATABUFFER_H_

//Includes
#include "imu/mpu.h"
//#include "robot/pwmsw.h"
#include "zedboard/print_utils.h"
#include "zedboard/timer_int.h"

//Defines
#define BUF_ADDR			0x7000000
#define BUF_MAX				(0x1FF00000 - DATA_NUMBER_OF_BYTES)


//Functions
void storeInBuff(SensorPwmData* data);
int transmitBuf();
void resetBuff();

#endif /* DATABUFFER_H_ */
