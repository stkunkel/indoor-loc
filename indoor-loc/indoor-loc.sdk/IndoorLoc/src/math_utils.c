/*
 * math_utils.c
 *
 *  Created on: Nov 12, 2015
 *      Author: rupprich
 */

#include "math_utils.h"

void eulerGetSigma(float quat[QUATERNION_AMOUNT], float* sigma){
	*sigma = atan2((quat[2]*quat[3] + quat[0]*quat[1]), 0.5-(quat[1]*quat[1] + quat[2]*quat[2])) * 180/M_PI;
}

void eulerGetTheta(float quat[QUATERNION_AMOUNT], float* theta){
	*theta = asin(-2.0*(quat[1]*quat[3] + quat[0]*quat[2])) * 180/M_PI;
}

void eulerGetPsi(float quat[QUATERNION_AMOUNT], float* psi){
	*psi = atan2((quat[1]*quat[2] + quat[0]*quat[3]), 0.5-(quat[2]*quat[2] + quat[3]*quat[3])) * 180/M_PI;
}
