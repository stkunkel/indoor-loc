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
 * Get Inverse
 * In: Matrix
 * Returns inverse of matrix
 */
Matrix getInverse(Matrix matrix) { //TODO check
	//Variables
	Matrix result;
	float temp[NUMBER_OF_AXES][NUMBER_OF_AXES];
	float temp_inv[NUMBER_OF_AXES][NUMBER_OF_AXES];

	//Convert matrix to float array for internal calculation
	matrixToFloatArray(matrix, temp);

	//get inverse
	cofactor(temp, NUMBER_OF_AXES, temp_inv);

	//convert back to matrix
	result = toMatrix(temp_inv);

	//Return
	return result;
}

/*
 * Finding co-factor of a matrix
 * http://www.sanfoundry.com/c-program-find-inverse-matrix/
 * In: num, f
 * Out: cofactor
 */
void cofactor(float num[NUMBER_OF_AXES][NUMBER_OF_AXES], float f,
		float result[NUMBER_OF_AXES][NUMBER_OF_AXES]) {	//TODO check
	//Variables
	float b[NUMBER_OF_AXES][NUMBER_OF_AXES],
			fac[NUMBER_OF_AXES][NUMBER_OF_AXES];
	int p, q, m, n, i, j;

	//Calculation
	for (q = 0; q < f; q++) {
		for (p = 0; p < f; p++) {
			m = 0;
			n = 0;
			for (i = 0; i < f; i++) {
				for (j = 0; j < f; j++) {
					if (i != q && j != p) {
						b[m][n] = num[i][j];
						if (n < (f - 2))
							n++;
						else {
							n = 0;
							m++;
						}
					}
				}
			}
			fac[q][p] = pow(-1, q + p) * determinant(b, f - 1);
		}
	}
	transpose(num, fac, f, result);
}
/*
 *	Finding transpose of matrix
 *	http://www.sanfoundry.com/c-program-find-inverse-matrix/
 *	In: num, fac, r
 *	Out: transpose
 *
 */
void transpose(float num[NUMBER_OF_AXES][NUMBER_OF_AXES],
		float fac[NUMBER_OF_AXES][NUMBER_OF_AXES], float r,
		float result[NUMBER_OF_AXES][NUMBER_OF_AXES]) {	//TODO check
	//Variables
	int i, j;
	float b[NUMBER_OF_AXES][NUMBER_OF_AXES], d;

	//Computation
	for (i = 0; i < r; i++) {
		for (j = 0; j < r; j++) {
			b[i][j] = fac[j][i];
		}
	}
	d = determinant(num, r);

	if (d == 0) {
		memcpy(result, b, NUMBER_OF_AXES * NUMBER_OF_AXES * sizeof(float));
	} else {
		for (i = 0; i < r; i++) {
			for (j = 0; j < r; j++) {
				result[i][j] = b[i][j] / d;
			}
		}
	}

	return;
}

/*
 * For calculating Determinant of the Matrix
 * http://www.sanfoundry.com/c-program-find-inverse-matrix/
 * In: a, k
 * Returns determinant
 */
float determinant(float a[NUMBER_OF_AXES][NUMBER_OF_AXES], float k) {//TODO check
	//Variables
	float s = 1, det = 0, b[NUMBER_OF_AXES][NUMBER_OF_AXES];
	int i, j, m, n, c;

	//Computation
	if (k == 1) {
		return (a[0][0]);
	} else {
		det = 0;
		for (c = 0; c < k; c++) {
			m = 0;
			n = 0;
			for (i = 0; i < k; i++) {
				for (j = 0; j < k; j++) {
					b[i][j] = 0;
					if (i != 0 && j != c) {
						b[m][n] = a[i][j];
						if (n < (k - 2))
							n++;
						else {
							n = 0;
							m++;
						}
					}
				}
			}
			det = det + s * (a[0][c] * determinant(b, k - 1));
			s = -1 * s;
		}
	}

	//Return
	return (det);
}

/*
 * Matrix Inverse Test
 */
void testMatrixInverse() {
	//Variables
	Matrix a, a_inv, a_inv_inv;
	int i, status = 0;

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
				status++;
			}
		}
	}

	//Print status
	myprintf("Matrix Inverse Test Result: %d\r\n", status);

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

//Get Cofactors
	cofactors = getCofactorMatrix(*m);

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

//Multiply cofactors with determinant
	*result = multMatrixByScalar(cofactors, 1.0 / d);

//Return
	return 0;

}

/*
 * Get Cofactors of matrix
 * In: Matrix
 * Returns: Matrix of cofactors
 */
Matrix getCofactorMatrix(Matrix m) {
//Variables
	Matrix result;

//Get Matrix of minors
	result = getMinors(m);

//Apply Chess Board
	applyChessBoard(&result);

//Return
	return result;
}

/*
 * Apply "Chess board" to matrix
 * (Change sign of every second value -> used to get cofactors from minors)
 * In/Out: Matrix
 */
void applyChessBoard(Matrix* m) {
//Variables
	int i;

//Go through matrix
	for (i = 0; i < NUMBER_OF_AXES * NUMBER_OF_AXES; i++) {
		//odd index -> change sign
		if (i % 2 != 0) {
			//odd index -> change sign
			m->value[i] = -m->value[i];
		}
	}
}

/*
 * Get Matrix of "Minors"
 * In: Matrix
 * Returns matrix of minors
 */
Matrix getMinors(Matrix m) {
//Variables
	unsigned int a, b, i, j, subindex, sub_num_of_axes = NUMBER_OF_AXES - 1;
	Matrix result;
	float subMatrix[sub_num_of_axes * sub_num_of_axes];

//Go through rows of resulting matrix
	for (a = 0; a < NUMBER_OF_AXES; a++) {

		//Go through columns of resulting matrix
		for (b = 0; b < NUMBER_OF_AXES; b++) {
			//Reset sub-index
			subindex = 0;

			//Go through rows
			for (i = 0; i < NUMBER_OF_AXES; i++) {
				//Only take rows different from a
				if (i != a) {

					//Go through columns
					for (j = 0; j < NUMBER_OF_AXES; j++) {

						//Only take columns different from b
						if (j != b) {

							//Set sub-matrices
							subMatrix[subindex] =
									m.value[i * NUMBER_OF_AXES + j];

							//Increase sub-index
							subindex++;
						}
					}
				}
			}

			//Compute minor
			result.value[a * NUMBER_OF_AXES + b] = getDeterminantRecursive(
					subMatrix, sub_num_of_axes);
		}
	}

//Return
	return result;
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
 * Get Roation Matrix from Quaternion
 * In: Quaternion
 * Out: Rotation Matrix for left-handed coordinate system
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
	rotationMatrix.value[5] = 2 * y_z - 2 * w_x; //right handed: +
	rotationMatrix.value[6] = 2 * x_z - 2 * w_y;
	rotationMatrix.value[7] = 2 * y_z + 2 * w_x; //right-handed: -
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
 * Comparison of Floats
 * In: two floats
 * Returns 1 if floats are (nearly) equal and 0 if not
 */
char equal_f(float f1, float f2) {
	if (fabs(f1 - f2) < 0.00001) {
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
