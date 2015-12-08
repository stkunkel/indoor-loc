/*
 * math_utils.c: Math utilities for Euler angles, number conversions, etc.
 * Author: Stephanie Kunkel
 */

#include "math_utils.h"

/*
 * Discrete Integration: result = recent + scalar * vector
 * In: scalar, vector, recent
 * Out: result
 */
void discreteIntegration(float scalar, Vector* vector, Vector* recent, Vector* result){
	*result = addVectors(*recent, multVectorByScalar(*vector, scalar));
}

/*
 * Get Roation Matrix from Quaternion
 * In: Quaternion
 * Out: Rotation Matrix
 */
void toRotationMatrix(float quat[QUATERNION_AMOUNT], Matrix* rotationMatrix) {
	rotationMatrix->value[0] = quat[0] * quat[0] + quat[1] * quat[1]
			- quat[2] * quat[2] - quat[3] * quat[3];
	rotationMatrix->value[1] = 2 * quat[1] * quat[2] - 2 * quat[0] * quat[3];
	rotationMatrix->value[2] = 2 * quat[1] * quat[3] + 2 * quat[0] * quat[2];
	rotationMatrix->value[3] = 2 * quat[1] * quat[2] + 2 * quat[0] * quat[3];
	rotationMatrix->value[4] = quat[0] * quat[0] - quat[1] * quat[1]
			+ quat[2] * quat[2] - quat[3] * quat[3];
	rotationMatrix->value[5] = 2 * quat[2] * quat[3] + 2 * quat[0] * quat[1];
	rotationMatrix->value[6] = 2 * quat[1] * quat[3] - 2 * quat[0] * quat[2];
	rotationMatrix->value[7] = 2 * quat[2] * quat[3] - 2 * quat[0] * quat[1];
	rotationMatrix->value[8] = quat[0] * quat[0] - quat[1] * quat[1]
			- quat[2] * quat[2] + quat[3] * quat[3];
}

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

/*
 * Multiply Vector by Scalar
 * In: Vector, Scalar
 * Returns Vector
 */
Vector multVectorByScalar(Vector vector, float scalar) {
	//Variables
	int i;
	Vector result;

	//Multiply each element by scalar
	for (i = 0; i < NUMBER_OF_AXES; i++) {
		result.value[i] = vector.value[i] * scalar;
	}

	//Return
	return result;
}

/*
 * Vector Addition
 * In: two Vectors
 * Returns Vector
 */
Vector addVectors(Vector vector1, Vector vector2) {
	//Variables
	int i;
	Vector result;

	//Add each element
	for (i = 0; i < NUMBER_OF_AXES; i++) {
		result.value[i] = vector1.value[i] + vector2.value[i];
	}

	//Return
	return result;
}

/*
 * Vector Subtraction
 * In: two Vectors
 * Returns Vector result
 */
Vector substractVectors(Vector minuend, Vector substrahend) {
	//Variables
	int i;
	Vector result;

	//Add each element
	for (i = 0; i < NUMBER_OF_AXES; i++) {
		result.value[i] = minuend.value[i] - substrahend.value[i];
	}

	//Return
	return result;
}

/*
 * Multiplication: Scalar * Matrix
 * In: Matrix, Scalar
 * Returns Matrix
 */
Matrix multMatrixByScalar(Matrix matrix, float scalar) {
	//Variables
	int i;
	Matrix result;

	//Multiply each element by scalar
	for (i = 0; i < NUMBER_OF_AXES * NUMBER_OF_AXES; i++) {
		result.value[i] = matrix.value[i] * scalar;
	}

	//Return
	return result;
}

/*
 * Multiplication: Matrix * Vector
 * In: Matrix, Vector
 * Returns Vector
 */
Vector multMatrixAndVector(Matrix matrix, Vector vector) {
	//Variables
	int i, j;
	Vector result;

	//Multiply each element by scalar
	for (i = 0; i < NUMBER_OF_AXES; i += NUMBER_OF_AXES) { //row index
		result.value[i] = 0.0;
		for (j = 0; j < NUMBER_OF_AXES; j++) { //column index
			result.value[i] += matrix.value[i * NUMBER_OF_AXES + j]
					* vector.value[i];
		}
	}

	//Return
	return result;
}
