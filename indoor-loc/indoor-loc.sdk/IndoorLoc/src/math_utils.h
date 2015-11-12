/*
 * math_utils.h
 *
 *  Created on: Nov 12, 2015
 *      Author: rupprich
 */

#ifndef MATH_UTILS_H_
#define MATH_UTILS_H_

#include "math.h"

#define NUMBER_OF_AXES		3			//x, y, z
#define QUATERNION_AMOUNT	4			//w, x, y, z rotational angles


void eulerGetSigma(float quat[QUATERNION_AMOUNT], float* sigma);
void eulerGetTheta(float quat[QUATERNION_AMOUNT], float* theta);
void eulerGetPsi(float quat[QUATERNION_AMOUNT], float* psi);
#endif /* MATH_UTILS_H_ */
