/*
 * math_utils.c: Math utilities for Euler angles, number conversions, etc.
 * Author: Stephanie Kunkel
 */

#include "math_utils.h"

/*
 * Compute Position
 * x = x_old + (v_old * delta_t) + (0.5 * a * (delta_t)^2)
 * In: pointers to acceleration a and old velocity v_old; time difference delta_t
 * Out: pointer to new velocity v
 */
void computePosition(Vector* x_old, Vector* v_old, Vector* a, float delta_t, Vector* x) {
	*x = addVectors(*x_old, addVectors(multVectorByScalar(*v_old, delta_t), multVectorByScalar(*a, 0.5*delta_t*delta_t)));
}

/*
 * Compute Velocity
 * v = v_old + (a * delta_t)
 * In: pointers to acceleration a and old velocity v_old; time difference delta_t
 * Out: pointer to new velocity v
 */
void computeVelocity(Vector* v_old, Vector* a, float delta_t, Vector* v) {
	*v = addVectors(*v_old, multVectorByScalar(*a, delta_t));
}

/*
 * Get Roation Matrix from Quaternion
 * In: Quaternion
 * Out: Rotation Matrix
 */
Matrix toRotationMatrix(float quat[QUATERNION_AMOUNT]) {
	//Variables
	Matrix rotationMatrix;

	//Helper computations
	float w_w = quat[0] * quat[0];
	float x_x = quat[1] * quat[1];
	float y_y = quat[2] * quat[2];
	float z_z = quat[3] * quat[3];
	float w_x = quat[0] * quat[1];
	float w_y = quat[0] * quat[2];
	float w_z = quat[0] * quat[3];
	float x_y = quat[1] * quat[2];
	float x_z = quat[1] * quat[3];
	float y_z = quat[2] * quat[3];

	//Set values
	rotationMatrix.value[0] = w_w + x_x - y_y - z_z;
	rotationMatrix.value[1] = 2 * x_y - 2 * w_z;
	rotationMatrix.value[2] = 2 * x_z + 2 * w_y;
	rotationMatrix.value[3] = 2 * x_y + 2 * w_z;
	rotationMatrix.value[4] = w_w - x_x + y_y - z_z;
	rotationMatrix.value[5] = 2 * y_z + 2 * w_x;
	rotationMatrix.value[6] = 2 * x_z - 2 * w_y;
	rotationMatrix.value[7] = 2 * y_z - 2 * w_x;
	rotationMatrix.value[8] = w_w - x_x - y_y + z_z;

	return rotationMatrix;
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
 * Convert float array to Vector
 * In: Pointer to float array
 * Returns Vector
 */
Vector toVector(float* array) {
	//Variables
	Vector vector;

	//Create Vector
	memcpy(&(vector.value[0]), array, NUMBER_OF_AXES * sizeof(float));

	//Return
	return vector;
}

/*
 * Convert Vector to float array
 * In: Pointer to Vector
 * Out: Float array
 */
void toFloatArray(Vector vector, float* array) {
	//Variables
	int i;

	//Create Vector
	for (i = 0; i < NUMBER_OF_AXES; i++) {
		array[i] = vector.value[i];
	}
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
	for (i = 0; i < NUMBER_OF_AXES; i ++) { //row index
		result.value[i] = 0.0;
		for (j = 0; j < NUMBER_OF_AXES; j++) { //column index
			result.value[i] += matrix.value[i * NUMBER_OF_AXES + j]
					* vector.value[i];
		}
	}

	//Return
	return result;
}
