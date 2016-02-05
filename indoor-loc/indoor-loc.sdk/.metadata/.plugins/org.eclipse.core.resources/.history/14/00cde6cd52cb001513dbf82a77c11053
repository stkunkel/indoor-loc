/*
 * math_utils.h: Vector, Matrix and position/rotation helper functions
 * Author: Stephanie Kunkel
 */

#ifndef MATH_UTILS_H_
#define MATH_UTILS_H_

//Includes
#include "print_utils.h"
#include "math.h"
#include <string.h>
#include "program_parameters.h"

//Defines
#define NUMBER_OF_AXES			3					//x, y, z
#define QUATERNION_AMOUNT		(NUMBER_OF_AXES + 1)//w, x, y, z rotational angles
#define X_AXIS					0
#define Y_AXIS					1
#define Z_AXIS					2
#define QUAT_W					0
#define QUAT_X					1
#define QUAT_Y					2
#define QUAT_Z					3
#define EPSILON					0.0001				//max. difference of two floats to be equal

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
void testMatrixInverse();
float determinant(float a[NUMBER_OF_AXES][NUMBER_OF_AXES], float k);
int getInverseOfMatrix(Matrix* m, Matrix* result);
float getDeterminantRecursive(float* m, unsigned int degree);
Matrix getAdj(Matrix m);
Matrix getTranspose(Matrix m);
void multiplyQuaternions(float* q1, float* q2, float* result);
void testVectorRotation();
Vector rotateVector(Vector* vector, float quat[QUATERNION_AMOUNT]);
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
float degToRad(float deg);
float radToDeg(float rad);
#endif /* MATH_UTILS_H_ */
