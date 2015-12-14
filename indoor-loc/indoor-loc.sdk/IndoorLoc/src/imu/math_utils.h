/*
 * math_utils.h
 * Author: Stephanie Kunkel
 */

#ifndef MATH_UTILS_H_
#define MATH_UTILS_H_

//Includes
#include "math.h"

//Defines
#define NUMBER_OF_AXES			3					//x, y, z
#define QUATERNION_AMOUNT		(NUMBER_OF_AXES + 1)//w, x, y, z rotational angles

//Typedefs
typedef struct {
	float value[NUMBER_OF_AXES]; //rows
} Vector;

typedef struct {
	float value[NUMBER_OF_AXES * NUMBER_OF_AXES]; //rows * columns
} Matrix;

//Functions
void discreteIntegration(float scalar, Vector* vector, Vector* recent, Vector* result);
void toRotationMatrix(float quat[QUATERNION_AMOUNT], Matrix* rotationMatrix);
void eulerGetSigma(float quat[QUATERNION_AMOUNT], float* sigma);
void eulerGetTheta(float quat[QUATERNION_AMOUNT], float* theta);
void eulerGetPsi(float quat[QUATERNION_AMOUNT], float* psi);
long floatToQ16(float x);
Vector toVector(float* array);
void toFloatArray(Vector vector, float* array);
Vector multVectorByScalar(Vector vector, float scalar);
Vector addVectors(Vector vector1, Vector vector2);
Vector substractVectors(Vector minuend, Vector substrahend);
Matrix multMatrixByScalar(Matrix matrix, float scalar);
Vector multMatrixAndVector(Matrix matrix, Vector vector);
#endif /* MATH_UTILS_H_ */
