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
#define NORM_MEAS_X	(120.0/ACCEL_CONV_VAL)	//0.98352050781 //(raw / ACCEL_CONV_VAL)
#define NORM_MEAS_Y	(43.0/ACCEL_CONV_VAL)	//-0.00756835937
#define NORM_MEAS_Z	(15990.4/ACCEL_CONV_VAL)	//-0.0869140625

//Print Frequency
#define IMUVIEWER_FREQ			10 			//Hz

//Return Types
#define XST_SUCCESS	0
#define XST_FAILURE	1

#endif /* PARAMETERS_H_ */