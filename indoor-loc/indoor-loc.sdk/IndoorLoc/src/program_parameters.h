/*
 * 	program_parameters.h: Parameters to influence program flow
 *	Author: Stephanie Kunkel
 */

#ifndef PROGRAM_PARAMETERS_H_
#define PROGRAM_PARAMETERS_H_

//Defines
//#define USE_DMP						//Comment OUT to use DMP. Comment to use MPU Register Values only.
//#define DEBUG 						//Comment OUT for DEBUG prints, COMMENT for print only for IMU viewer

//Parameters
#define CAL_TIME				0 			//s
#define CAL_SAMPLES				1000
#define IMU_SAMPLE_FREQ			200			//Hz //For real-time computation, make sure FIFO Frequency is not higher than 200Hz
#define IMUVIEWER_FREQ			10 			//Hz
#define FPGA_FREQ				100000000	//FCLK in Hz

//XModem
#define DATA_NUMBER_OF_BYTES	48
#define BYTE0					0x000000FF
#define BYTE1					0x0000FF00
#define BYTE2					0x00FF0000
#define BYTE3					0xFF000000

//Robot
#define NUMBER_OF_JOINTS		6

//UWB
#define UWB_DIST_SENS			(sizeof(int16_t))

//Math
#define NUMBER_OF_AXES			3					//x, y, z
#define QUATERNION_AMOUNT		(NUMBER_OF_AXES + 1)//w, x, y, z rotational angles

//Struct for Mpu Register Data
typedef struct {
	int16_t gyro[NUMBER_OF_AXES];
	int16_t accel[NUMBER_OF_AXES];
	int16_t compass[NUMBER_OF_AXES];
	int32_t temp;
//	short fill;
} MpuRegisterData;

//Struct for Sensor and Robot Data
typedef struct {
	MpuRegisterData mpuData;
	uint32_t pwmValues[NUMBER_OF_JOINTS];
	int16_t  uwb_dist_cm;
} SensorPwmData;

#endif /* PROGRAM_PARAMETERS_H_ */
