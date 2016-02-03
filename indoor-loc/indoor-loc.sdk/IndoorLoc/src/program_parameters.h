/*
 * 	program_parameters.h: Parameters to influence program flow
 *	Author: Stephanie Kunkel
 */

#ifndef PROGRAM_PARAMETERS_H_
#define PROGRAM_PARAMETERS_H_

//Defines
//#define USE_DMP						//Uncomment to use DMP. Comment to use MPU Register Values only.
//#define INITIAL_CALIBRATION			//Uncomment for initial Calibration, comment to skip initial calibration
//#define DEBUG 						//UNComment for DEBUG prints, COMMENT for print only for IMU viewer

//Parameters
#define CAL_SAMPLES				1000		//Number of samples used for calibration TODO: increase to 10000
#define IMUVIEWER_FREQ			10 			//Hz
#define FPGA_FREQ				100000000	//FCLK in Hz

#endif /* PROGRAM_PARAMETERS_H_ */