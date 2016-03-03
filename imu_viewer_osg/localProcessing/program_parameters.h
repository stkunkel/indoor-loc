/*
 * parameters.h
 * Author: Stephanie Kunkel
 */

#ifndef PARAMETERS_H_
#define PARAMETERS_H_

//Defines
//#define LOCAL
#define INFILEDESC	"filtered.txt"
#define OUTFILEDESC	"out.txt"

//Measured Normal Force
#define ACCEL_CONV_VAL	16384
#define NORM_MEAS_X	(1554.566667/ACCEL_CONV_VAL)	//(raw / ACCEL_CONV_VAL)
#define NORM_MEAS_Y	(-308.366667/ACCEL_CONV_VAL)	//
#define NORM_MEAS_Z	(7838.900000/ACCEL_CONV_VAL)	//

//Print Frequency
#define IMUVIEWER_FREQ			10 			//Hz

//Return Types
#define XST_SUCCESS	0
#define XST_FAILURE	1

#endif /* PARAMETERS_H_ */