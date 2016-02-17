/*
 * mpu_utils.h
 * Author: Stephanie Kunkel
 */

#ifndef IMU_H_
#define IMU_H_

//Includes
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <xil_io.h>
#include "xil_types.h"
#include "xparameters.h"
#include "xgpiops.h"
#include "xstatus.h"
#include "xtime_l.h"
#include "mpu9150/inv_mpu.h"
#include "../zedboard/iic_utils.h"
#include "../zedboard/print_utils.h"
#include "../zedboard/time_utils.h"

//Zynq Parameters
#define GPIO_DEVICE_ID  	XPAR_XGPIOPS_0_DEVICE_ID //from xparameters.h
#define PS_IMU_AD0_PIN 	12 //MIO12
#define PS_IMU_INT_PIN 	13 //MIO13


//IMU Parameters
#define IMU_ADDR_MASK		0b1101000 	//b110100X - X determined by logic level on AD0
#define IMU_LAST_ADDR_BIT	0			//one IMU

//Functions
int imuSetDmpIntMode(unsigned char mode);
int imuSetDmpAccelBias(long *bias);
int imuSetDmpGyroBias(long *bias);
int imuGetFifoCnt(int* cnt);
int imuEnableDmpGyroCal(unsigned char enable);
int imuEnableDmpFeatures(unsigned short mask);
int imuSetFifoRate(unsigned short rate);
int imuSetSensors(unsigned char sensors);
int imuReadIntConfig(unsigned char* config);
int imuConfigureInt(unsigned char* config);
int imuSetIntLatched(unsigned char enable);
int imuSetGyroBias(long *gyro_bias);
int imuSetDmpState(unsigned char enable);
int imuSetGyroFsr(unsigned short fsr);
int imuSetAccelFsr(unsigned char fsr) ;
int imuResetFifo();
int imuSetSampleRate(unsigned short rate);
int imuDmpLoadMotionDriverFirmware();
int imuInitMpu(struct int_param_s *int_param);
int imuDmpReadFifo(short *gyro, short *accel, long *quat,
		unsigned long *timestamp, short *sensors, unsigned char *more);
int imuReadGyroBias(long *gyro_bias);
int imuSetAccelBias(const long *accel_bias);
int imuReadAccelBias(long *accel_bias);
int imuGetIntStatus(short* status);
int imuReadGyroReg(short *data, unsigned long *timestamp);
int imuReadAccelReg(short *data, unsigned long *timestamp);
int imuReadCompassReg(short *data, unsigned long *timestamp);
int imuReadTemp(long *data, unsigned long *timestamp);
int imuI2cRead(unsigned char slave_addr, unsigned char reg_addr, unsigned char length, unsigned char *data);
int imuI2cWrite(unsigned char slave_addr, unsigned char reg_addr, unsigned char length, unsigned char *data);
void imuDelay(unsigned long ms);
int imuInit(u8 *imuAddr);
void imuGet_ms(unsigned long* timestamp);
int imuLog_i(const char *format, ...);
int imuLog_e(const char *format, ...);
int readInt(u32 *DataRead);

#endif /* IMU_H_ */
