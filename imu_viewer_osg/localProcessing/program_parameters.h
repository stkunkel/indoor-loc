/*
 * parameters.h
 * Author: Stephanie Kunkel
 */

#ifndef PARAMETERS_H_
#define PARAMETERS_H_

//Defines
#define LOCAL
#define INFILEDESC	"filtered.txt"
#define OUTFILEDESC	"out.txt"

//Measured Normal Force
#define ACCEL_CONV_VAL	16384
#define NORM_MEAS_X	(6080.433333/ACCEL_CONV_VAL)	//(raw / ACCEL_CONV_VAL)
#define NORM_MEAS_Y	(-3348.933333/ACCEL_CONV_VAL)	//
#define NORM_MEAS_Z	(13031.466667/ACCEL_CONV_VAL)	//

//Print Frequency
#define IMUVIEWER_FREQ			10 			//Hz

//Return Types
#define XST_SUCCESS	0
#define XST_FAILURE	1

#endif /* PARAMETERS_H_ */