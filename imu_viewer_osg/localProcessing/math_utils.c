/*
 * math_utils.c: Vector, Matrix and position/rotation helper functions
 * Author: Stephanie Kunkel
 */

#include "math_utils.h"

/*
 * Compute Position
 * x = x_old + (v_old * delta_t) + (0.5 * a * (delta_t)^2)
 * In: pointers to acceleration a and old velocity v_old; time difference delta_t
 * Out: pointer to new velocity v
 */
void computePosition(Vector* x_old, Vector* v_old, Vector* a, float delta_t,
		Vector* x) {
	*x = addVectors(*x_old,
			addVectors(multVectorByScalar(*v_old, delta_t),
					multVectorByScalar(*a, 0.5 * delta_t * delta_t)));
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
 * Matrix Inverse Test
 */
void testMatrixInverse() {
	//Variables
	Matrix a, a_inv, a_inv_inv;
	int i, status = 0, result[4] = { 0, 0, 0, 0 };

	//TEST 1
	//Inizialize Matrix
	a.value[0] = 3.0;
	a.value[1] = 0.0;
	a.value[2] = 2.0;
	a.value[3] = 2.0;
	a.value[4] = 0.0;
	a.value[5] = -2.0;
	a.value[6] = 0.0;
	a.value[7] = 1.0;
	a.value[8] = 1.0;

	//Get Inverse
	status = getInverseOfMatrix(&a, &a_inv);

	//Get Inverse of inverse
	status = getInverseOfMatrix(&a_inv, &a_inv_inv);

	if (status == 0) {
		//Compare
		for (i = 0; i < (NUMBER_OF_AXES * NUMBER_OF_AXES); i++) {
			if (!equal_f(a_inv_inv.value[i], a.value[i])) {
				result[0]++;
			}
		}
	}

	// TEST 2
	//Inizialize Matrix
	a.value[0] = 1.0;
	a.value[1] = 0.0;
	a.value[2] = 0.0;
	a.value[3] = 0.0;
	a.value[4] = 1.0;
	a.value[5] = 0.0;
	a.value[6] = 0.0;
	a.value[7] = 0.0;
	a.value[8] = 1.0;

	//Get Inverse
	status = getInverseOfMatrix(&a, &a_inv);

	//Get Inverse of inverse
	status = getInverseOfMatrix(&a_inv, &a_inv_inv);

	if (status == 0) {
		//Compare
		for (i = 0; i < (NUMBER_OF_AXES * NUMBER_OF_AXES); i++) {
			if (!equal_f(a_inv_inv.value[i], a.value[i])) {
				result[1]++;
			}
		}
	}

	// TEST 3
	//Inizialize Matrix
	a.value[0] = -21.0;
	a.value[1] = 53.0;
	a.value[2] = 0.65;
	a.value[3] = 103.0;
	a.value[4] = 0.000012;
	a.value[5] = -0.123;
	a.value[6] = -105430.0;
	a.value[7] = 534.0;
	a.value[8] = -35.0;

	//Get Inverse
	status = getInverseOfMatrix(&a, &a_inv);

	//Get Inverse of inverse
	status = getInverseOfMatrix(&a_inv, &a_inv_inv);

	if (status == 0) {
		//Compare
		for (i = 0; i < (NUMBER_OF_AXES * NUMBER_OF_AXES); i++) {
			if (!equal_f(a_inv_inv.value[i], a.value[i])) {
				result[2]++;
			}
		}
	}

	// TEST 4
	//Inizialize Matrix
	a.value[0] = -1.0;
	a.value[1] = -2.0;
	a.value[2] = 2.0;
	a.value[3] = 2.0;
	a.value[4] = 1.0;
	a.value[5] = 1.0;
	a.value[6] = 3.0;
	a.value[7] = 4.0;
	a.value[8] = 5.0;

	//Get Inverse
	status = getInverseOfMatrix(&a, &a_inv);

	//Get Inverse of inverse
	status = getInverseOfMatrix(&a_inv, &a_inv_inv);

	if (status == 0) {
		//Compare
		for (i = 0; i < (NUMBER_OF_AXES * NUMBER_OF_AXES); i++) {
			if (!equal_f(a_inv_inv.value[i], a.value[i])) {
				result[3]++;
			}
		}
	}

	//Print status
	myprintf("Matrix Inverse Test Result: %d %d %d %d\r\n", result[0],
			result[1], result[2], result[3]);

}

/*
 * Get Inverse of a Matrix
 * In: Matrix
 * Out: inverse of matrix
 * Returns 1 if there is no inverse
 */
int getInverseOfMatrix(Matrix* m, Matrix* result) {
//Variables
	Matrix cofactors;
	float matrixArray[NUMBER_OF_AXES * NUMBER_OF_AXES];
	float d;

	//Get Adj
	cofactors = getAdj(*m);

//Transpose
	*result = getTranspose(cofactors);

//Copy matrix values into float array
	memcpy(matrixArray, m->value,
			(NUMBER_OF_AXES * NUMBER_OF_AXES) * sizeof(float));

//Get determinant
	d = getDeterminantRecursive(matrixArray, NUMBER_OF_AXES);
	if (d == 0) {
		return 1;
	}

//Multiply cofactors with 1 / determinant
	*result = multMatrixByScalar(cofactors, 1.0 / d);

//Return
	return 0;

}

/*
 * Get Determinant of a Matrix (as float array) -- Laplace Expansion
 * Note: This is a recursive operation.
 * In: Matrix as float array
 * Returns determinant of Matrix
 */
float getDeterminantRecursive(float* m, unsigned int degree) {
//Variables
	unsigned int factor, i, j, subindex;
	float d = 0.0;
	float subMatrix[(degree - 1) * (degree - 1)];

	//Return number if is is a scalar
	if (degree == 2) {
		return m[0] * m[3] - m[2] * m[1];
	}

//Generate sub-matrices and recursively compute determinant
	for (factor = 0; factor < degree; factor++) {
		//Reset sub-index
		subindex = 0;

		//Go through rows
		for (i = 0; i < degree; i++) {
			//Only take rows different from factor
			if (i != 0) {	//factor

				//Go through columns
				for (j = 0; j < degree; j++) {

					//Only take columns different from factor
					if (j != factor) {

						//Set sub-matrices
						subMatrix[subindex] = m[i * degree + j];

						//Increase sub-index
						subindex++;
					}
				}
			}
		}

		//Compute determinant
		if (factor % 2 == 0) { //even factor index --> add
			d += m[factor] * getDeterminantRecursive(subMatrix, degree - 1);
		} else { //odd factor index --> subtract
			d -= m[factor] * getDeterminantRecursive(subMatrix, degree - 1);
		}

	}

//Return
	return d;
}

/*
 * Get adj of Matrix
 * In: Matrix
 * Returns adj. of matrix
 */
Matrix getAdj(Matrix m) {
	//Variables
	Matrix result;
	unsigned int g, h, i, j, subindex;
	float subMatrix[(NUMBER_OF_AXES - 1) * (NUMBER_OF_AXES - 1)];

	//Go through rows
	for (g = 0; g < NUMBER_OF_AXES; g++) {

		//Go through columns
		for (h = 0; h < NUMBER_OF_AXES; h++) {
			//Reset sub-index
			subindex = 0;

			//Go through rows
			for (i = 0; i < NUMBER_OF_AXES; i++) {
				//Only take rows different from factor
				if (i != g) {	//factor

					//Go through columns
					for (j = 0; j < NUMBER_OF_AXES; j++) {

						//Only take columns different from h
						if (j != h) {

							//Set sub-matrices
							subMatrix[subindex] =
									m.value[i * NUMBER_OF_AXES + j];

							//Increase sub-index
							subindex++;
						}
					}
				}
			}

			//Get Adj
			result.value[g * NUMBER_OF_AXES + h] = pow(-1.0, h + h)
					* getDeterminantRecursive(subMatrix, NUMBER_OF_AXES - 1);
		}
	}

	//Return
	return result;
}

/*
 * Get Transpose of Matrix
 * In: Matrix
 * Returns transpose of matrix
 */
Matrix getTranspose(Matrix m) {
//Variables
	int i, j;
	Matrix result;

//Generate Transpose
	for (i = 0; i < NUMBER_OF_AXES; i++) {
		for (j = 0; j < NUMBER_OF_AXES; j++) {
			result.value[j * NUMBER_OF_AXES + i] = m.value[i * NUMBER_OF_AXES
					+ j];
		}
	}

//Return
	return result;
}

/*
 * Multiply two Quaternions
 */
void multiplyQuaternions(float* q1, float* q2, float* result) {
	//Variables
	result[0] = q1[0] * q2[0] - q1[1] * q2[1] - q1[2] * q2[2] - q1[3] * q2[3];
	result[1] = q1[0] * q2[1] + q1[1] * q2[0] + q1[2] * q2[3] - q1[3] * q2[2];
	result[2] = q1[0] * q2[2] - q1[1] * q2[3] + q1[2] * q2[0] + q1[3] * q2[1];
	result[3] = q1[0] * q2[3] + q1[1] * q2[2] - q1[2] * q2[1] + q1[3] * q2[0];
}

/*
 * Test Vector Rotation using Quaternion
 */
void testVectorRotation() {
	Vector vector;
	Vector result;
	float quat[QUATERNION_AMOUNT] = { 1.0, 0.0, 0.0, 0.0 };
	
	//Initialize Vector
	vector.value[0] = 0.0;
	vector.value[1] = 0.0;
	vector.value[2] = 0.0;

	//Same
	result = rotateVector(&vector, quat);
	printf("0dgr: %f %f %f\r\n", result.value[0], result.value[1],
			result.value[2]);

	//90° Around x axis --> (0, 0, 1)
	quat[0] = 0.7071;
	quat[1] = 0.7071;
	quat[2] = 0.0;
	quat[3] = 0.0;
	result = rotateVector(&vector, quat);
	printf("90dgr: %f %f %f\r\n", result.value[0], result.value[1],
			result.value[2]);

	//180° around x axis --> (-1, 0, 0)
	quat[0] = 0.0;
	quat[1] = 1.0;
	quat[2] = 0.0;
	quat[3] = 0.0;
	result = rotateVector(&vector, quat);
	printf("180dgr: %f %f %f\r\n", result.value[0], result.value[1],
			result.value[2]);

}

/*
 * Rotate Vector using Quaternion
 * Sources: http://www.euclideanspace.com/maths/algebra/realNormedAlgebra/quaternions/transforms/
 * 			http://de.mathworks.com/help/aeroblks/quaternionrotation.html
 * In: Pointer to vector and quaternion
 * Returns rotated vector
 */
Vector rotateVector(Vector* vector, float quat[QUATERNION_AMOUNT]) {
	//Variables
	Vector result;

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

	//Computation
	result.value[0] = w_w * vector->value[0] + 2 * w_y * vector->value[2]
			- 2 * w_z * vector->value[1] + x_x * vector->value[0]
			+ 2 * x_y * vector->value[1] + 2 * x_z * vector->value[2]
			- z_z * vector->value[0] - y_y * vector->value[0];
	result.value[1] = 2 * x_y * vector->value[0] + y_y * vector->value[1]
			+ 2 * y_z * vector->value[2] + 2 * w_z * vector->value[0]
			- z_z * vector->value[1] + w_w * vector->value[1]
			- 2 * w_x * vector->value[2] - x_x * vector->value[1];
	result.value[2] = 2 * x_z * vector->value[0] + 2 * y_z * vector->value[1]
			+ z_z * vector->value[2] - 2 * w_y * vector->value[0]
			- y_y * vector->value[2] + 2 * w_x * vector->value[1]
			- x_x * vector->value[2] + w_w * vector->value[2];

	//Return
	return result;
}

/*
 * Get Roation Matrix from Quaternion
 * In: Quaternion
 * Out: Rotation Matrix for right-handed coordinate system
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
	rotationMatrix.value[1] = 2 * x_y + 2 * w_z; //lhcs: -
	rotationMatrix.value[2] = 2 * x_z - 2 * w_y; //lhcs: +
	rotationMatrix.value[3] = 2 * x_y - 2 * w_z; //lhcs: +
	rotationMatrix.value[4] = w_w - x_x + y_y - z_z;
	rotationMatrix.value[5] = 2 * y_z + 2 * w_x; //lhcs: -
	rotationMatrix.value[6] = 2 * x_z + 2 * w_y; //lhcs: -
	rotationMatrix.value[7] = 2 * y_z - 2 * w_x; //lhcs: +
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
 * Quaternion Inverse
 * Source: http://mathworld.wolfram.com/Quaternion.html
 * In: Quaternion
 * Out: Inverse
 */
void quatInverse(float quat[QUATERNION_AMOUNT], float result[QUATERNION_AMOUNT]){
	//Variables
	float conj[QUATERNION_AMOUNT];
	float norm, norm_sq;

	//Get Conjugate
	quatConjugate(quat, conj);

	//Get Norm and square
	norm = quatNorm(quat);
	norm_sq = norm * norm;

	//Compute Inverse
	result[0] = conj[0]/norm_sq;
	result[1] = conj[0]/norm_sq;
	result[2] = conj[0]/norm_sq;
	result[3] = conj[0]/norm_sq;
}

/*
 * Quaternion Norm
 * Source: http://mathworld.wolfram.com/Quaternion.html
 * In: Quaternion
 * Returns Norm
 */
float quatNorm(float quat[QUATERNION_AMOUNT]){
	return (sqrtf((quat[0]*quat[0]) + (quat[1]*quat[1]) + (quat[2]*quat[2]) + (quat[3]*quat[3])));
}

/*
 * Quaternion Conjugate
 * Source: http://mathworld.wolfram.com/Quaternion.html
 * In: Quaternion
 * Out: Conjugate
 */
void quatConjugate(float quat[QUATERNION_AMOUNT], float result[QUATERNION_AMOUNT]){
	result[0] = quat[0];
	result[1] = -quat[1];
	result[2] = -quat[2];
	result[3] = -quat[3];
}

/*
 * Comparison of Floats
 * In: two floats
 * Returns 1 if floats are (nearly) equal and 0 if not
 */
char equal_f(float f1, float f2) {
	if (fabs(f1 - f2) < EPSILON) {
		return 1;
	} else {
		return 0;
	}
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
 * Convert float array to Matrix
 * In: Pointer to float array
 * Returns Matrix
 */
Matrix toMatrix(float array[NUMBER_OF_AXES][NUMBER_OF_AXES]) {
//Variables
	int i, j;
	Matrix matrix;

//Create Matrix
	for (i = 0; i < NUMBER_OF_AXES; i++) {
		for (j = 0; j < NUMBER_OF_AXES; j++) {
			matrix.value[i * NUMBER_OF_AXES + j] = array[i][j];
		}

	}

//Return
	return matrix;
}

/*
 * Convert Matrix to float array
 * In: Pointer to Matrix
 * Out: Float array with two indices
 */
void matrixToFloatArray(Matrix matrix,
		float array[NUMBER_OF_AXES][NUMBER_OF_AXES]) {
//Variables
	int i, j;

//Create Vector
	for (i = 0; i < NUMBER_OF_AXES; i++) {
		for (j = 0; j < NUMBER_OF_AXES; j++) {
			array[i][j] = matrix.value[i * NUMBER_OF_AXES + j];
		}
	}
}

/*
 * Cross-product of two vectors from origin (RHCS)
 * In: Vector a and b
 * Returns cross-product
 */
Vector crossProductFromOrigin(Vector* a, Vector* b) {
	//Variables
	Vector result;

	//Computation
	result.value[0] = a->value[1] * b->value[2] - a->value[2] * b->value[1];
	result.value[1] = a->value[2] * b->value[0] - a->value[0] * b->value[2];
	result.value[2] = a->value[0] * b->value[1] - a->value[1] * b->value[0];

	//Return
	return result;
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
void vectorToFloatArray(Vector vector, float* array) {
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
	for (i = 0; i < NUMBER_OF_AXES; i++) { //row index
		result.value[i] = 0.0;
		for (j = 0; j < NUMBER_OF_AXES; j++) { //column index
			result.value[i] += matrix.value[i * NUMBER_OF_AXES + j]
					* vector.value[j];
		}
	}

//Return
	return result;
}

/*
 * Convert Degrees to Radians
 * In: Degrees
 * Returns Radians
 */
float degToRad(float deg) {
	return (deg / 180.0 * M_PI);
}

/*
 * Convert Radians to Degrees
 * In: Radians
 * Returns Degrees
 */
float radToDeg(float rad) {
	return (rad * 180.0 / M_PI);
}