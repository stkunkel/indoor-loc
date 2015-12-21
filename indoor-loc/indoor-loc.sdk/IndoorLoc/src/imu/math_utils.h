/*
 * math_utils.h: Vector, Matrix and position/rotation helper functions
 * Author: Stephanie Kunkel
 */

#ifndef MATH_UTILS_H_
#define MATH_UTILS_H_

//Includes
#include "../print_utils.h"
#include "math.h"
#include <string.h>

//Defines
#define NUMBER_OF_AXES			3					//x, y, z
#define QUATERNION_AMOUNT		(NUMBER_OF_AXES + 1)//w, x, y, z rotational angles
#define EPSILON					0.000001			//max. difference of two floats to be equal

//Typedefs
typedef struct {
	float value[NUMBER_OF_AXES]; //rows
} Vector;

typedef struct {
	float value[NUMBER_OF_AXES * NUMBER_OF_AXES]; //rows * columns
} Matrix;

//Functions
void computePosition(Vector* x_old, Vector* v_old, Vector* a, float delta_t,
		Vector* x);
void computeVelocity(Vector* v_old, Vector* a, float delta_t, Vector* v);
Matrix getInverse(Matrix matrix);
void cofactor(float num[NUMBER_OF_AXES][NUMBER_OF_AXES], float f, float result[NUMBER_OF_AXES][NUMBER_OF_AXES]);
void transpose(float num[NUMBER_OF_AXES][NUMBER_OF_AXES],
		float fac[NUMBER_OF_AXES][NUMBER_OF_AXES], float r, float result[NUMBER_OF_AXES][NUMBER_OF_AXES]);
void testMatrixInverse();
float determinant(float a[NUMBER_OF_AXES][NUMBER_OF_AXES], float k);
int getInverseOfMatrix(Matrix* m, Matrix* result);
Matrix getCofactorMatrix(Matrix m);
void applyChessBoard(Matrix* m);
Matrix getMinors(Matrix m);
float getDeterminantRecursive(float* m, unsigned int degree);
Matrix getTranspose(Matrix m);
Matrix toRotationMatrix(float quat[QUATERNION_AMOUNT]);
void eulerGetSigma(float quat[QUATERNION_AMOUNT], float* sigma);
void eulerGetTheta(float quat[QUATERNION_AMOUNT], float* theta);
void eulerGetPsi(float quat[QUATERNION_AMOUNT], float* psi);
char equal_f(float f1, float f2);
long floatToQ16(float x);
Matrix toMatrix(float array[NUMBER_OF_AXES][NUMBER_OF_AXES]);
void matrixToFloatArray(Matrix matrix, float array[NUMBER_OF_AXES][NUMBER_OF_AXES]);
Vector toVector(float* array);
void vectorToFloatArray(Vector vector, float* array);
Vector multVectorByScalar(Vector vector, float scalar);
Vector addVectors(Vector vector1, Vector vector2);
Vector substractVectors(Vector minuend, Vector substrahend);
Matrix multMatrixByScalar(Matrix matrix, float scalar);
Vector multMatrixAndVector(Matrix matrix, Vector vector);
#endif /* MATH_UTILS_H_ */
