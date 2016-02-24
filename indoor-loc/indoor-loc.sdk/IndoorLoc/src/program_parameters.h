/*
 * 	program_parameters.h: Parameters to influence program flow
 *	Author: Stephanie Kunkel
 */

#ifndef PROGRAM_PARAMETERS_H_
#define PROGRAM_PARAMETERS_H_

//Defines
//#define USE_DMP						//Uncomment to use DMP. Comment to use MPU Register Values only.
//#define DEBUG 						//UNComment for DEBUG prints, COMMENT for print only for IMU viewer
//#define DISABLE_POS_COMP				//UNcomment to disable quaternion and position computation (not required if just raw data is to be outputtet)

//Parameters
#define CAL_TIME				10 //s
#define CAL_SAMPLES				1000
#define IMUVIEWER_FREQ			10 			//Hz
#define FPGA_FREQ				100000000	//FCLK in Hz

//XModem
#define DATA_NUMBER_OF_BYTES	46
#define BYTE0					0x000000FF
#define BYTE1					0x0000FF00
#define BYTE2					0x00FF0000
#define BYTE3					0xFF000000

//Robot
#define NUMBER_OF_JOINTS		6

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

//Struct for Data Transmission
typedef struct {
	uint32_t pwmValues[NUMBER_OF_JOINTS];
	MpuRegisterData mpuData;
} RobotMpuData;

#endif /* PROGRAM_PARAMETERS_H_ */
