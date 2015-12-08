/*
 * math_utils.c: Math utilities for Euler angles, number conversions, etc.
 * Author: Stephanie Kunkel
 */

#include "math_utils.h"

/*
 * Euler Angles
 */
void eulerGetSigma(float quat[QUATERNION_AMOUNT], float* sigma) {
	*sigma = atan2((quat[2] * quat[3] + quat[0] * quat[1]),
			0.5 - (quat[1] * quat[1] + quat[2] * quat[2])) * 180 / M_PI;
}

void eulerGetTheta(float quat[QUATERNION_AMOUNT], float* theta) {
	*theta = asin(-2.0 * (quat[1] * quat[3] + quat[0] * quat[2])) * 180 / M_PI;
}

void eulerGetPsi(float quat[QUATERNION_AMOUNT], float* psi) {
	*psi = atan2((quat[1] * quat[2] + quat[0] * quat[3]),
			0.5 - (quat[2] * quat[2] + quat[3] * quat[3])) * 180 / M_PI;
}

/*
 * Convert float to Q16
 */
long floatToQ16(float x) {
	return (long) ((1L << 16) * x);
}

/*
 * Convert Q16 to float
 */
float q16ToFloat(long q16) {
	return (float) q16 / (1L << 16);
}
