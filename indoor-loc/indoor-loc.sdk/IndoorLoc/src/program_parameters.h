/*
 * 	program_parameters.h: Parameters to influence program flow
 *	Author: Stephanie Kunkel
 */

#ifndef PROGRAM_PARAMETERS_H_
#define PROGRAM_PARAMETERS_H_

//Defines
//#define USE_DMP						//Uncomment to use DMP. Comment to use MPU Register Values only.
#define DEBUG 						//UNComment for DEBUG prints, COMMENT for print only for IMU viewer
//#define DISABLE_POS_COMP				//UNcomment to disable quaternion and position computation (not required if just raw data is to be outputtet)

//Parameters
#define CAL_TIME				60 //s
#define CAL_SAMPLES				1000
#define IMUVIEWER_FREQ			10 			//Hz
#define FPGA_FREQ				100000000	//FCLK in Hz

#endif /* PROGRAM_PARAMETERS_H_ */
