#include "matrices.h"
#include "Constants.h"
#define _USE_MATH_DEFINES
#include <math.h>

using namespace MatrixConstants;

namespace JMMatrix {
	// #####################################################################
	// ***** ***** generic ***** *****
	// #####################################################################

	// Calculate determinant for 2x2 matrix.
	float CalculateDeterminant(float index0, float index1, float index2, float index3) {
		return index0 * index3 - index1 * index2;
	}





	// #####################################################################
	// ***** ***** mat2 ***** *****
	// #####################################################################
	/*	openGL Matrix is column-major
		M = |0	2|
			|1	3|

		data[4] = {0, 1, 2, 3};
	*/
	mat2::mat2() {
		for (int i = 0; i < sizeof(data)/sizeof(float); i++)
		{
			data[i] = 0;
		}
	}

	// Clean the rounding errors.
	void mat2::Clean() {
		for (int i = 0; i < sizeof(data) / sizeof(float); i++)
		{
			data[i] = roundf(data[i] * kDecimalPlaces) / kDecimalPlaces;
		}
	}

	float mat2::Determinant() const{
		//return (data[0] * data[3] - data[1] * data[2]);
		return CalculateDeterminant(data[0], data[1], data[2], data[3]);
	}

	mat2 Inverse(const mat2& matrix) {
		float determinant = matrix.Determinant();
		mat2 inverse;
		
		inverse.data[0] =  matrix.data[3] / determinant;
		inverse.data[1] = -matrix.data[1] / determinant;

		inverse.data[2] = -matrix.data[2] / determinant;
		inverse.data[3] =  matrix.data[0] / determinant;

		return inverse;
	}

	mat2 Transpose(const mat2& matrix) {
		mat2 transpose;

		transpose.data[0] = matrix.data[0];
		transpose.data[1] = matrix.data[2];

		transpose.data[2] = matrix.data[1];
		transpose.data[3] = matrix.data[3];

		return transpose;
	}

	// Add.
	mat2& mat2::operator+= (const mat2& matrix) {
		for (int i = 0; i < sizeof(data) / sizeof(float); i++)
		{
			data[i] += matrix.data[i];
		}

		return *this;
	}

	// Subtract.
	mat2& mat2::operator-= (const mat2& matrix) {
		for (int i = 0; i < sizeof(data) / sizeof(float); i++)
		{
			data[i] -= matrix.data[i];
		}

		return *this;
	}

	// Scale.
	mat2& mat2::operator*= (const float scalar) {
		for (int i = 0; i < sizeof(data) / sizeof(float); i++)
		{
			data[i] *= scalar;
		}

		return *this;
	}

	const mat2 operator+ (const mat2& matrixLeft, const mat2& matrixRight) {
		mat2 matrix;

		for (int i = 0; i < sizeof(matrix.data) / sizeof(float); i++)
		{
			matrix.data[i] = matrixLeft.data[i] + matrixRight.data[i];
		}

		return matrix;
	}

	const mat2 operator- (const mat2& matrix) {
		mat2 matrixN;

		for (int i = 0; i < sizeof(matrix.data) / sizeof(float); i++)
		{
			matrixN.data[i] = -matrix.data[i];
		}

		return matrixN;
	}

	const mat2 operator- (const mat2& matrixLeft, const mat2& matrixRight) {
		mat2 matrix;

		for (int i = 0; i < sizeof(matrix.data) / sizeof(float); i++)
		{
			matrix.data[i] = matrixLeft.data[i] - matrixRight.data[i];
		}

		return matrix;
	}

	const mat2 operator* (float scalar, const mat2& matrix) {
		mat2 matrixTemp;

		for (int i = 0; i < sizeof(matrixTemp.data) / sizeof(float); i++)
		{
			matrixTemp.data[i] = matrix.data[i] * scalar;
		}

		return matrixTemp;
	}

	const mat2 operator* (const mat2& matrix, const float scalar) {
		return operator*(scalar, matrix);
	}

	const mat2 operator* (const mat2& matrixLeft, const mat2& matrixRight) {
		mat2 matrixTemp;
		/*
		M = |0	2|	N = |0 2|
			|1	3|		|1 3|

		M * N = |0*0 + 2*1    0*2 + 2*3|
				|1*0 + 3*1    1*2 + 3*3|
		*/
		matrixTemp.data[0] = matrixLeft.data[0] * matrixRight.data[0] + matrixLeft.data[2] * matrixRight.data[1];
		matrixTemp.data[1] = matrixLeft.data[1] * matrixRight.data[0] + matrixLeft.data[3] * matrixRight.data[1];

		matrixTemp.data[2] = matrixLeft.data[0] * matrixRight.data[2] + matrixLeft.data[2] * matrixRight.data[3];
		matrixTemp.data[3] = matrixLeft.data[1] * matrixRight.data[2] + matrixLeft.data[3] * matrixRight.data[3];

		return matrixTemp;
	}

	const vec2 operator* (const mat2& matrix, const vec2& vector) {
		return vec2(
			matrix.data[0] * vector.x + matrix.data[2] * vector.y, 
			matrix.data[1] * vector.x + matrix.data[3] * vector.y
		);
	}

	const bool operator== (const mat2& matrixLeft, const mat2& matrixRight) {
		for (int i = 0; i < sizeof(matrixLeft.data) / sizeof(float); i++)
		{
			if (fabsf(matrixLeft.data[i] - matrixRight.data[i]) > kMaxDifference)
			{
				return false;
			}
		}

		return true;
	}

	const bool operator!= (const mat2& matrixLeft, const mat2& matrixRight) {
		return !operator==(matrixLeft, matrixRight);
	}

	std::ostream& operator<<(std::ostream& output, const mat2& matrix) {
	/*	
	M = |0	2|
		|1	3|

	data[4] = {0, 1, 2, 3};
	*/
		// output << "(" << vector.x << ", " << vector.y << ")";
		output << "|" << matrix.data[0] << "\t " << matrix.data[2] << "|\n";
		output << "|" << matrix.data[1] << "\t " << matrix.data[3] << "|\n";

		return output;
	}





	// #####################################################################
	// ***** ***** mat3 ***** *****
	// #####################################################################
	/*	openGL Matrix is column-major
	M = |0	3  6|
		|1	4  7|
		|2	5  8|

	data[9] = {0, 1, 2, 3, 4, 5, 6, 7, 8};
	*/
	mat3::mat3() {
		for (int i = 0; i < sizeof(data) / sizeof(float); i++)
		{
			data[i] = 0;
		}
	}

	// Clean the rounding errors.
	void mat3::Clean() {
		for (int i = 0; i < sizeof(data) / sizeof(float); i++)
		{
			data[i] = roundf(data[i] * kDecimalPlaces) / kDecimalPlaces;
		}
	}

	float mat3::Determinant() const {
/*
		mat2 mat21, mat22, mat23;
		float data21[4] = { data[4], data[5], data[7], data[8] };
		float data22[4] = { data[1], data[2], data[7], data[8] };
		float data23[4] = { data[1], data[2], data[4], data[5] };

		for (int i = 0; i < sizeof(mat21.data) / sizeof(float); i++)
		{
			mat21.data[i] = data21[i];
			mat22.data[i] = data22[i];
			mat23.data[i] = data23[i];
		}

		return (data[0] * mat21.Determinant() - data[3] * mat22.Determinant() + data[6] * mat23.Determinant());
*/
/*
		return (
			  data[0] * (data[4] * data[8] - data[5] * data[7]) 
			- data[3] * (data[1] * data[8] - data[2] * data[7])
			+ data[6] * (data[1] * data[5] - data[2] * data[4])
		);
*/
		return (
			 data[0] * CalculateDeterminant(data[4], data[5], data[7], data[8])
			-data[3] * CalculateDeterminant(data[1], data[2], data[7], data[8])
			+data[6] * CalculateDeterminant(data[1], data[2], data[4], data[5])
		);

	}

	mat3 Inverse(const mat3& matrix) {
		float determinant = matrix.Determinant();
		mat3 transpose = Transpose(matrix);
		mat3 inverse;
		/*
		| 0  3  6 |
		| 1  4  7 |
		| 2  5  8 |

		2x2 determinants:
		inverse[i] -> determinant -> calculateDeterminant(a, b, c, d)
		0 -> 4 * 8 - 5 * 7  ->  4, 5, 7, 8
		1 -> 3 * 8 - 5 * 6  ->  3, 5, 6, 8
		2 -> 3 * 7 - 4 * 6  ->  3, 4, 6, 7

		3 -> 1 * 8 - 2 * 7  ->  1, 2, 7, 8
		4 -> 0 * 8 - 2 * 6  ->  0, 2, 6, 8
		5 -> 0 * 7 - 1 * 6  ->  0, 1, 6, 7

		6 -> 1 * 5 - 2 * 4  ->  1, 2, 4, 5
		7 -> 0 * 5 - 2 * 3  ->  0, 2, 3, 5
		8 -> 0 * 4 - 1 * 3  ->  0, 1, 3, 4
		*/
		inverse.data[0] =  CalculateDeterminant(transpose.data[4], transpose.data[5], transpose.data[7], transpose.data[8]) / determinant;
		inverse.data[1] = -CalculateDeterminant(transpose.data[3], transpose.data[5], transpose.data[6], transpose.data[8]) / determinant;
		inverse.data[2] =  CalculateDeterminant(transpose.data[3], transpose.data[4], transpose.data[6], transpose.data[7]) / determinant;

		inverse.data[3] = -CalculateDeterminant(transpose.data[1], transpose.data[2], transpose.data[7], transpose.data[8]) / determinant;
		inverse.data[4] =  CalculateDeterminant(transpose.data[0], transpose.data[2], transpose.data[6], transpose.data[8]) / determinant;
		inverse.data[5] = -CalculateDeterminant(transpose.data[0], transpose.data[1], transpose.data[6], transpose.data[7]) / determinant;

		inverse.data[6] =  CalculateDeterminant(transpose.data[1], transpose.data[2], transpose.data[4], transpose.data[5]) / determinant;
		inverse.data[7] = -CalculateDeterminant(transpose.data[0], transpose.data[2], transpose.data[3], transpose.data[5]) / determinant;
		inverse.data[8] =  CalculateDeterminant(transpose.data[0], transpose.data[1], transpose.data[3], transpose.data[4]) / determinant;

		return inverse;
	}

	mat3 Transpose(const mat3& matrix) {
		mat3 transpose;

		transpose.data[0] = matrix.data[0];
		transpose.data[1] = matrix.data[3];
		transpose.data[2] = matrix.data[6];

		transpose.data[3] = matrix.data[1];
		transpose.data[4] = matrix.data[4];
		transpose.data[5] = matrix.data[7];

		transpose.data[6] = matrix.data[2];
		transpose.data[7] = matrix.data[5];
		transpose.data[8] = matrix.data[8];

		return transpose;
	}

	// Add.
	mat3& mat3::operator+= (const mat3& matrix) {
		for (int i = 0; i < sizeof(data) / sizeof(float); i++)
		{
			data[i] += matrix.data[i];
		}

		return *this;
	}

	// Subtract.
	mat3& mat3::operator-= (const mat3& matrix) {
		for (int i = 0; i < sizeof(data) / sizeof(float); i++)
		{
			data[i] -= matrix.data[i];
		}

		return *this;
	}

	// Scale.
	mat3& mat3::operator*= (const float scalar) {
		for (int i = 0; i < sizeof(data) / sizeof(float); i++)
		{
			data[i] *= scalar;
		}

		return *this;
	}

	const mat3 operator+ (const mat3& matrixLeft, const mat3& matrixRight) {
		mat3 matrix;

		for (int i = 0; i < sizeof(matrix.data) / sizeof(float); i++)
		{
			matrix.data[i] = matrixLeft.data[i] + matrixRight.data[i];
		}

		return matrix;
	}

	const mat3 operator- (const mat3& matrix) {
		mat3 matrixN;

		for (int i = 0; i < sizeof(matrix.data)/sizeof(float); i++)
		{
			matrixN.data[i] = -matrix.data[i];
		}

		return matrixN;
	}

	const mat3 operator- (const mat3& matrixLeft, const mat3& matrixRight) {
		mat3 matrix;

		for (int i = 0; i < sizeof(matrix.data) / sizeof(float); i++)
		{
			matrix.data[i] = matrixLeft.data[i] - matrixRight.data[i];
		}

		return matrix;
	}

	const mat3 operator* (float scalar, const mat3& matrix) {
		mat3 matrixTemp;

		for (int i = 0; i < sizeof(matrixTemp.data) / sizeof(float); i++)
		{
			matrixTemp.data[i] = matrix.data[i] * scalar;
		}

		return matrixTemp;
	}

	const mat3 operator* (const mat3& matrix, const float scalar) {
		return operator*(scalar, matrix);
	}

	const mat3 operator* (const mat3& matrixLeft, const mat3& matrixRight) {
		mat3 matrixTemp;
		/*
		M = |0	3  6|	N = |0	3  6|
			|1	4  7|		|1	4  7|
			|2	5  8|		|2	5  8|

		M * N = |0*0 + 3*1 + 6*2    0*3 + 3*4 + 6*5    0*6 + 3*7 + 6*8|
				|1*0 + 4*1 + 7*2    1*3 + 4*4 + 7*5    1*6 + 4*7 + 7*8|
				|2*0 + 5*1 + 8*2    2*3 + 5*4 + 8*5    2*6 + 5*7 + 8*8|
		*/
		matrixTemp.data[0] = matrixLeft.data[0] * matrixRight.data[0] + matrixLeft.data[3] * matrixRight.data[1] + matrixLeft.data[6] * matrixRight.data[2];
		matrixTemp.data[1] = matrixLeft.data[1] * matrixRight.data[0] + matrixLeft.data[4] * matrixRight.data[1] + matrixLeft.data[7] * matrixRight.data[2];
		matrixTemp.data[2] = matrixLeft.data[2] * matrixRight.data[0] + matrixLeft.data[5] * matrixRight.data[1] + matrixLeft.data[8] * matrixRight.data[2];

		matrixTemp.data[3] = matrixLeft.data[0] * matrixRight.data[3] + matrixLeft.data[3] * matrixRight.data[4] + matrixLeft.data[6] * matrixRight.data[5];
		matrixTemp.data[4] = matrixLeft.data[1] * matrixRight.data[3] + matrixLeft.data[4] * matrixRight.data[4] + matrixLeft.data[7] * matrixRight.data[5];
		matrixTemp.data[5] = matrixLeft.data[2] * matrixRight.data[3] + matrixLeft.data[5] * matrixRight.data[4] + matrixLeft.data[8] * matrixRight.data[5];

		matrixTemp.data[6] = matrixLeft.data[0] * matrixRight.data[6] + matrixLeft.data[3] * matrixRight.data[7] + matrixLeft.data[6] * matrixRight.data[8];
		matrixTemp.data[7] = matrixLeft.data[1] * matrixRight.data[6] + matrixLeft.data[4] * matrixRight.data[7] + matrixLeft.data[7] * matrixRight.data[8];
		matrixTemp.data[8] = matrixLeft.data[2] * matrixRight.data[6] + matrixLeft.data[5] * matrixRight.data[7] + matrixLeft.data[8] * matrixRight.data[8];

		return matrixTemp;
	}

	const vec3 operator* (const mat3& matrix, const vec3& vector) {
		return vec3(
			matrix.data[0] * vector.x + matrix.data[3] * vector.y + matrix.data[6] * vector.z,
			matrix.data[1] * vector.x + matrix.data[4] * vector.y + matrix.data[7] * vector.z,
			matrix.data[2] * vector.x + matrix.data[5] * vector.y + matrix.data[8] * vector.z
		);
	}

	const bool operator== (const mat3& matrixLeft, const mat3& matrixRight) {
		for (int i = 0; i < sizeof(matrixLeft.data) / sizeof(float); i++)
		{
			if (fabsf(matrixLeft.data[i] - matrixRight.data[i]) > kMaxDifference)
			{
				return false;
			}
		}

		return true;
	}

	const bool operator!= (const mat3& matrixLeft, const mat3& matrixRight) {
		return !operator==(matrixLeft, matrixRight);
	}

	std::ostream& operator<<(std::ostream& output, const mat3& matrix) {
		/*
		M = |0	3  6|
			|1	4  7|
			|2	5  8|

		data[9] = {0, 1, 2, 3, 4, 5, 6, 7, 8};
		*/
		// output << "(" << vector.x << ", " << vector.y << ")";
		output << "|" << matrix.data[0] << "\t " << matrix.data[3] << "\t " << matrix.data[6] << "|\n";
		output << "|" << matrix.data[1] << "\t " << matrix.data[4] << "\t " << matrix.data[7] << "|\n";
		output << "|" << matrix.data[2] << "\t " << matrix.data[5] << "\t " << matrix.data[8] << "|\n";

		return output;
	}





	// #####################################################################
	// ***** ***** mat4 ***** *****
	// #####################################################################
	/*	openGL Matrix is column-major
	M = |0   4   8   12|
		|1   5   9   13|
		|2   6   10  14|
		|3   7   11  15|

	data[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
	*/
	mat4::mat4() {
		for (int i = 0; i < sizeof(data)/sizeof(float); i++)
		{
			data[i] = 0;
		}
	}

	// Clean the rounding errors.
	void mat4::Clean() {
		for (int i = 0; i < sizeof(data) / sizeof(float); i++)
		{
			data[i] = roundf(data[i] * kDecimalPlaces) / kDecimalPlaces;
		}
	}

	mat4 Transpose(const mat4& matrix) {
		mat4 transpose;

		transpose.data[0] = matrix.data[0];
		transpose.data[1] = matrix.data[4];
		transpose.data[2] = matrix.data[8];
		transpose.data[3] = matrix.data[12];
		
		transpose.data[4] = matrix.data[1];
		transpose.data[5] = matrix.data[5];
		transpose.data[6] = matrix.data[9];
		transpose.data[7] = matrix.data[13];

		transpose.data[8] = matrix.data[2];
		transpose.data[9] = matrix.data[6];
		transpose.data[10] = matrix.data[10];
		transpose.data[11] = matrix.data[14];

		transpose.data[12] = matrix.data[3];
		transpose.data[13] = matrix.data[7];
		transpose.data[14] = matrix.data[11];
		transpose.data[15] = matrix.data[15];

		return transpose;
	}

	const mat4 operator+ (const mat4& matrixLeft, const mat4& matrixRight) {
		mat4 matrix;

		for (int i = 0; i < sizeof(matrix.data) / sizeof(float); i++)
		{
			matrix.data[i] = matrixLeft.data[i] + matrixRight.data[i];
		}

		return matrix;
	}

	const mat4 operator- (const mat4& matrix) {
		mat4 matrixN;

		for (int i = 0; i < sizeof(matrix.data) / sizeof(float); i++)
		{
			matrixN.data[i] = -matrix.data[i];
		}

		return matrixN;
	}

	const mat4 operator- (const mat4& matrixLeft, const mat4& matrixRight) {
		mat4 matrix;

		for (int i = 0; i < sizeof(matrix.data) / sizeof(float); i++)
		{
			matrix.data[i] = matrixLeft.data[i] - matrixRight.data[i];
		}

		return matrix;
	}

	const mat4 operator* (float scalar, const mat4& matrix) {
		mat4 matrixTemp;

		for (int i = 0; i < sizeof(matrixTemp.data) / sizeof(float); i++)
		{
			matrixTemp.data[i] = matrix.data[i] * scalar;
		}

		return matrixTemp;
	}

	const mat4 operator* (const mat4& matrix, const float scalar) {
		return operator*(scalar, matrix);
	}

	const mat4 operator* (const mat4& matrixLeft, const mat4& matrixRight) {
		mat4 matrixTemp;
		/*
		M = |0   4   8   12|	N = |0   4   8   12|
			|1   5   9   13|		|1   5   9   13|
			|2   6   10  14|		|2   6   10  14|
			|3   7   11  15|		|3   7   11  15|

		M * N = |0*0 + 4*1 + 8*2  + 12*3    0*4 + 4*5 + 8*6  + 12*7    0*8 + 4*9 + 8*10  + 12*11    0*12 + 4*13 + 8*14  + 12*15|
				|1*0 + 5*1 + 9*2  + 13*3    1*4 + 5*5 + 9*6  + 13*7    1*8 + 5*9 + 9*10  + 13*11    1*12 + 5*13 + 9*14  + 13*15|
				|2*0 + 6*1 + 10*2 + 14*3    2*4 + 6*5 + 10*6 + 14*7    2*8 + 6*9 + 10*10 + 14*11    2*12 + 6*13 + 10*14 + 14*15|
				|3*0 + 7*1 + 11*2 + 15*3    3*4 + 7*5 + 11*6 + 15*7    3*8 + 7*9 + 11*10 + 15*11    3*12 + 7*13 + 11*14 + 15*15|
		*/
		matrixTemp.data[0]  = matrixLeft.data[0] * matrixRight.data[0]  + matrixLeft.data[4] * matrixRight.data[1]  + matrixLeft.data[8]  * matrixRight.data[2]  + matrixLeft.data[12] * matrixRight.data[3];
		matrixTemp.data[1]  = matrixLeft.data[1] * matrixRight.data[0]  + matrixLeft.data[5] * matrixRight.data[1]  + matrixLeft.data[9]  * matrixRight.data[2]  + matrixLeft.data[13] * matrixRight.data[3];
		matrixTemp.data[2]  = matrixLeft.data[2] * matrixRight.data[0]  + matrixLeft.data[6] * matrixRight.data[1]  + matrixLeft.data[10] * matrixRight.data[2]  + matrixLeft.data[14] * matrixRight.data[3];
		matrixTemp.data[3]  = matrixLeft.data[3] * matrixRight.data[0]  + matrixLeft.data[7] * matrixRight.data[1]  + matrixLeft.data[11] * matrixRight.data[2]  + matrixLeft.data[15] * matrixRight.data[3];

		matrixTemp.data[4]  = matrixLeft.data[0] * matrixRight.data[4]  + matrixLeft.data[4] * matrixRight.data[5]  + matrixLeft.data[8]  * matrixRight.data[6]  + matrixLeft.data[12] * matrixRight.data[7];
		matrixTemp.data[5]  = matrixLeft.data[1] * matrixRight.data[4]  + matrixLeft.data[5] * matrixRight.data[5]  + matrixLeft.data[9]  * matrixRight.data[6]  + matrixLeft.data[13] * matrixRight.data[7];
		matrixTemp.data[6]  = matrixLeft.data[2] * matrixRight.data[4]  + matrixLeft.data[6] * matrixRight.data[5]  + matrixLeft.data[10] * matrixRight.data[6]  + matrixLeft.data[14] * matrixRight.data[7];
		matrixTemp.data[7]  = matrixLeft.data[3] * matrixRight.data[4]  + matrixLeft.data[7] * matrixRight.data[5]  + matrixLeft.data[11] * matrixRight.data[6]  + matrixLeft.data[15] * matrixRight.data[7];

		matrixTemp.data[8]  = matrixLeft.data[0] * matrixRight.data[8]  + matrixLeft.data[4] * matrixRight.data[9]  + matrixLeft.data[8]  * matrixRight.data[10] + matrixLeft.data[12] * matrixRight.data[11];
		matrixTemp.data[9]  = matrixLeft.data[1] * matrixRight.data[8]  + matrixLeft.data[5] * matrixRight.data[9]  + matrixLeft.data[9]  * matrixRight.data[10] + matrixLeft.data[13] * matrixRight.data[11];
		matrixTemp.data[10] = matrixLeft.data[2] * matrixRight.data[8]  + matrixLeft.data[6] * matrixRight.data[9]  + matrixLeft.data[10] * matrixRight.data[10] + matrixLeft.data[14] * matrixRight.data[11];
		matrixTemp.data[11] = matrixLeft.data[3] * matrixRight.data[8]  + matrixLeft.data[7] * matrixRight.data[9]  + matrixLeft.data[11] * matrixRight.data[10] + matrixLeft.data[15] * matrixRight.data[11];

		matrixTemp.data[12] = matrixLeft.data[0] * matrixRight.data[12] + matrixLeft.data[4] * matrixRight.data[13] + matrixLeft.data[8]  * matrixRight.data[14] + matrixLeft.data[12] * matrixRight.data[15];
		matrixTemp.data[13] = matrixLeft.data[1] * matrixRight.data[12] + matrixLeft.data[5] * matrixRight.data[13] + matrixLeft.data[9]  * matrixRight.data[14] + matrixLeft.data[13] * matrixRight.data[15];
		matrixTemp.data[14] = matrixLeft.data[2] * matrixRight.data[12] + matrixLeft.data[6] * matrixRight.data[13] + matrixLeft.data[10] * matrixRight.data[14] + matrixLeft.data[14] * matrixRight.data[15];
		matrixTemp.data[15] = matrixLeft.data[3] * matrixRight.data[12] + matrixLeft.data[7] * matrixRight.data[13] + matrixLeft.data[11] * matrixRight.data[14] + matrixLeft.data[15] * matrixRight.data[15];

		return matrixTemp;
	}

	const vec4 operator* (const mat4& matrix, const vec4& vector) {
		return vec4(
			matrix.data[0] * vector.x + matrix.data[4] * vector.y + matrix.data[8] * vector.z + matrix.data[12] * vector.w,
			matrix.data[1] * vector.x + matrix.data[5] * vector.y + matrix.data[9] * vector.z + matrix.data[13] * vector.w,
			matrix.data[2] * vector.x + matrix.data[6] * vector.y + matrix.data[10] * vector.z + matrix.data[14] * vector.w,
			matrix.data[3] * vector.x + matrix.data[7] * vector.y + matrix.data[11] * vector.z + matrix.data[15] * vector.w
			);
	}

	const bool operator== (const mat4& matrixLeft, const mat4& matrixRight) {
		for (int i = 0; i < sizeof(matrixLeft.data) / sizeof(float); i++)
		{
			if (fabsf(matrixLeft.data[i] - matrixRight.data[i]) > kMaxDifference)
			{
				return false;
			}
		}

		return true;
	}

	const bool operator!= (const mat4& matrixLeft, const mat4& matrixRight) {
		return !operator==(matrixLeft, matrixRight);
	}

	std::ostream& operator<<(std::ostream& output, const mat4& matrix) {
		/*
		M = |0   4   8   12|
			|1   5   9   13|
			|2   6   10  14|
			|3   7   11  15|

		data[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
		*/
		output << "|" << matrix.data[0] << "\t " << matrix.data[4] << "\t " << matrix.data[8]  << "\t " << matrix.data[12] << "|\n";
		output << "|" << matrix.data[1] << "\t " << matrix.data[5] << "\t " << matrix.data[9]  << "\t " << matrix.data[13] << "|\n";
		output << "|" << matrix.data[2] << "\t " << matrix.data[6] << "\t " << matrix.data[10] << "\t " << matrix.data[14] << "|\n";
		output << "|" << matrix.data[3] << "\t " << matrix.data[7] << "\t " << matrix.data[11] << "\t " << matrix.data[15] << "|\n";

		return output;
	}





	// #####################################################################
	// ***** ***** Matrix Factory ***** *****
	// #####################################################################
	// mat3
	const mat3 MatrixFactory::createZeroMat3() {
		mat3 matrix;

		for (int i = 0; i < sizeof(matrix.data) / sizeof(float); i++)
		{
			matrix.data[i] = 0;
		}

		return matrix;
	}

	const mat3 MatrixFactory::createIdentityMat3() {
		mat3 identity;

		for (int i = 0; i < sizeof(identity.data) / sizeof(float); i++)
		{
			if (i == 0 || i == 4 || i == 8)
			{
				identity.data[i] = 1;
			}
			else
			{
				identity.data[i] = 0;
			}
		}

		return identity;
	}

	const mat3 MatrixFactory::createMat3FromMat4(const mat4& matrix) {
		mat3 tempMatrix;

		tempMatrix.data[0] = matrix.data[0];
		tempMatrix.data[1] = matrix.data[1];
		tempMatrix.data[2] = matrix.data[2];

		tempMatrix.data[3] = matrix.data[4];
		tempMatrix.data[4] = matrix.data[5];
		tempMatrix.data[5] = matrix.data[6];

		tempMatrix.data[6] = matrix.data[8];
		tempMatrix.data[7] = matrix.data[9];
		tempMatrix.data[8] = matrix.data[10];

		return tempMatrix;
	}

	const mat3 MatrixFactory::createDualMat3(const vec3& vector) {
		mat3 dual;

		dual.data[0] = 0;
		dual.data[1] = vector.z;
		dual.data[2] = -vector.y;

		dual.data[3] = -vector.z;
		dual.data[4] = 0;
		dual.data[5] = vector.x;

		dual.data[6] = vector.y;
		dual.data[7] = -vector.x;
		dual.data[8] = 0;

		return dual;
	}

	const mat3 MatrixFactory::createRotationMat3(const float angle, const vec3& axis) {
		// R = I + (sen(angle)) * A + (1 - cos(angle)) * A * A
		mat3 rotation, dual;

		dual = MatrixFactory::createDualMat3(axis);
		rotation = MatrixFactory::createIdentityMat3() + (sinf(angle) * dual) + ((1 - cosf(angle)) * (dual * dual));

		return rotation;
	}

	const mat3 MatrixFactory::createScaleMat3(const float scale) {
		return (MatrixFactory::createIdentityMat3() * scale);
	}
	
	const mat3 MatrixFactory::createScaleMat3(const vec3& vector) {
		mat3 scale;

		scale.data[0] = vector.x;
		scale.data[4] = vector.y;
		scale.data[8] = vector.z;

		return scale;
	}

	const mat3 MatrixFactory::createTranslationMat3(const vec2& vector) {
		mat3 translation = MatrixFactory::createIdentityMat3();

		translation.data[6] = vector.x;
		translation.data[7] = vector.y;

		return translation;
	}

	// mat4
	const mat4 MatrixFactory::createZeroMat4() {
		mat4 matrix;

		for (int i = 0; i < sizeof(matrix.data) / sizeof(float); i++)
		{
			matrix.data[i] = 0;
		}

		return matrix;
	}

	const mat4 MatrixFactory::createIdentityMat4() {
		mat4 identity;

		for (int i = 0; i < sizeof(identity.data) / sizeof(float); i++)
		{
			if (i == 0 || i == 5 || i == 10 || i == 15)
			{
				identity.data[i] = 1;
			}
			else
			{
				identity.data[i] = 0;
			}
		}

		return identity;
	}

	const mat4 MatrixFactory::createMat4FromMat3(const mat3& matrix) {
		mat4 tempMatrix;

		tempMatrix.data[0]  = matrix.data[0];
		tempMatrix.data[1]  = matrix.data[1];
		tempMatrix.data[2]  = matrix.data[2];

		tempMatrix.data[4]  = matrix.data[3];
		tempMatrix.data[5]  = matrix.data[4];
		tempMatrix.data[6]  = matrix.data[5];

		tempMatrix.data[8]  = matrix.data[6];
		tempMatrix.data[9]  = matrix.data[7];
		tempMatrix.data[10] = matrix.data[8];

		tempMatrix.data[15] = 1.0f;

		return tempMatrix;
	}

	const mat4 MatrixFactory::createRotationMat4(const float angle, const vec3& axis) {
		return createMat4FromMat3(createRotationMat3(angle, axis));
	}

	const mat4 MatrixFactory::createRotationInverseMat4(const mat4& matrix) {
		return Transpose(matrix);
	}

	const mat4 MatrixFactory::createRotationFromViewMat4(const mat4& view) {
		mat4 temp = view;

		temp.data[3] = 0.0f;

		temp.data[7] = 0.0f;

		temp.data[11] = 0.0f;

		temp.data[12] = 0.0f;
		temp.data[13] = 0.0f;
		temp.data[14] = 0.0f;
		temp.data[15] = 1.0f;

		return temp;
	}

	const mat4 MatrixFactory::createScaleMat4(const float scale) {
		mat4 tempMatrix;

		tempMatrix.data[0]  = scale;
		tempMatrix.data[5]  = scale;
		tempMatrix.data[10] = scale;
		tempMatrix.data[15] = 1.0f;

		return tempMatrix;
	}

	const mat4 MatrixFactory::createScaleMat4(const vec3& vector) {
		mat4 tempMatrix;

		tempMatrix.data[0]  = vector.x;
		tempMatrix.data[5]  = vector.y;
		tempMatrix.data[10] = vector.z;
		tempMatrix.data[15] = 1.0f;

		return tempMatrix;
	}

	const mat4 MatrixFactory::createTranslationMat4(const vec3& vector) {
		mat4 translation = MatrixFactory::createIdentityMat4();

		translation.data[12] = vector.x;
		translation.data[13] = vector.y;
		translation.data[14] = vector.z;

		return translation;
	}

	const mat4 MatrixFactory::createTranslationInverseMat4(const mat4& matrix) {
		mat4 inverse = matrix;

		inverse.data[12] = -inverse.data[12];
		inverse.data[13] = -inverse.data[13];
		inverse.data[14] = -inverse.data[14];

		return inverse;
	}

	const mat4 MatrixFactory::createTranslationFromViewMat4(const mat4& view) {
		mat4 temp = MatrixFactory::createIdentityMat4();

		temp.data[12] = view.data[12];
		temp.data[13] = view.data[13];
		temp.data[14] = view.data[14];

		return temp;
	}

	// View Matrix from (eye, center, up) parameters.
	const mat4 MatrixFactory::createViewMat4(const vec3& eye, const vec3& center, const vec3& up) {
		vec3 view = center - eye;
		vec3 v = view * (1.0f / view.Length());

		//vec3 side = MatrixFactory::createDualMat3(v) * up;
		vec3 side = v.Cross(up);
		vec3 s = side * (1.0f / side.Length());

		//vec3 u = MatrixFactory::createDualMat3(s) * v;
		vec3 u = s.Cross(v);

		mat4 viewMatrix;
		viewMatrix.data[0] = s.x;
		viewMatrix.data[1] = u.x;
		viewMatrix.data[2] = -v.x;
		viewMatrix.data[3] = 0.0f;

		viewMatrix.data[4] = s.y;
		viewMatrix.data[5] = u.y;
		viewMatrix.data[6] = -v.y;
		viewMatrix.data[7] = 0.0f;

		viewMatrix.data[8] = s.z;
		viewMatrix.data[9] = u.z;
		viewMatrix.data[10] = -v.z;
		viewMatrix.data[11] = 0.0f;

		viewMatrix.data[12] = -s.Dot(eye);
		viewMatrix.data[13] = -u.Dot(eye);
		viewMatrix.data[14] = v.Dot(eye);
		viewMatrix.data[15] = 1.0f;

		return viewMatrix;
	}

	const mat4 MatrixFactory::createViewInverseMat4(const mat4& view) {
		mat4 viewRotation = MatrixFactory::createRotationFromViewMat4(view);
		mat4 viewRotationInverse = MatrixFactory::createRotationInverseMat4(viewRotation);
		mat4 viewTranslation = MatrixFactory::createTranslationFromViewMat4(view);
		mat4 viewTranslationInverse = MatrixFactory::createTranslationInverseMat4(viewTranslation);
		mat4 viewInverse = viewRotationInverse * viewTranslationInverse;
/** /
		std::cout << " =====> Matrix Factory View Inverse <=====\n";
		std::cout << "View:\n" << view << "\n";
		std::cout << "View Rotation:\n" << viewRotation << "\n";
		std::cout << "View Rotation Inverse:\n" << viewRotationInverse << "\n";
		std::cout << "View Translation:\n" << viewTranslation << "\n";
		std::cout << "View Translation Inverse:\n" << viewTranslationInverse << "\n";
		std::cout << "View Inverse:\n" << viewInverse << "\n";
		std::cout << "\n";
/**/
		return viewInverse;
	}

	// Orthographic Projection Matrix from (left-right, bottom-top, near-far) parameters.
	const mat4 MatrixFactory::createProjectionOrthographicMat4(const float left, const float right, const float bottom, const float top, const float near, const float far) {
		mat4 matrix;

		matrix.data[0] = 2 / (right - left);

		matrix.data[5] = 2 / (top - bottom);

		matrix.data[10] = - (2 / (far - near));

		matrix.data[12] = (left + right) / (left - right);
		matrix.data[13] = (bottom + top) / (bottom - top);
		matrix.data[14] = (near + far) / (near - far);
		matrix.data[15] = 1.0f;

		return matrix;
	}

	// Perspective Projection Matrix from (fovy, aspect, nearZ, farZ) parameters.
	const mat4 MatrixFactory::createProjectionPerspectiveMat4(const float fovy, const float aspect, const float nearZ, const float farZ) {
		//float d = 1 / tanf(fovy * M_PI / 360.0f);
		float d = 1.0f / tanf(fovy * 0.5f);
		mat4 matrix;

		matrix.data[0] = d / aspect;

		matrix.data[5] = d;

		matrix.data[10] = (nearZ + farZ) / (nearZ - farZ);
		matrix.data[11] = -1.0f;

		matrix.data[14] = (2 * farZ * nearZ) / (nearZ - farZ);

		return matrix;
	}

	const mat4 MatrixFactory::createProjectionPerspectiveMat4(const float fovy, const float width, const float height, const float nearZ, const float farZ) {
		return createProjectionPerspectiveMat4(fovy, width / height, nearZ, farZ);
	}

	const mat4 MatrixFactory::createInverseProjectionPerspectiveMat4(const mat4& perspective) {
		mat4 inverse;

		inverse.data[0] = 1.0f / perspective.data[0];

		inverse.data[5] = 1.0f / perspective.data[5];

		inverse.data[11] = 1.0f / perspective.data[14];

		inverse.data[14] = 1.0f / perspective.data[11];
		inverse.data[15] = - (perspective.data[10] / (perspective.data[11] * perspective.data[14]));
/** /
		std::cout << " =====> Matrix Factory Perspective Inverse <=====\n";
		std::cout << "Perspective:\n" << perspective << "\n";
		std::cout << "Perspective Inverse:\n" << inverse << "\n";
		std::cout << "\n";
/**/
		return inverse;
	}

	const mat4 MatrixFactory::createInverseMat4(const mat4& m) {
		float det;
		int i;
		mat4 inv;

		inv.data[0] = m.data[5] * m.data[10] * m.data[15] -
			m.data[5] * m.data[11] * m.data[14] -
			m.data[9] * m.data[6] * m.data[15] +
			m.data[9] * m.data[7] * m.data[14] +
			m.data[13] * m.data[6] * m.data[11] -
			m.data[13] * m.data[7] * m.data[10];

		inv.data[4] = -m.data[4] * m.data[10] * m.data[15] +
			m.data[4] * m.data[11] * m.data[14] +
			m.data[8] * m.data[6] * m.data[15] -
			m.data[8] * m.data[7] * m.data[14] -
			m.data[12] * m.data[6] * m.data[11] +
			m.data[12] * m.data[7] * m.data[10];

		inv.data[8] = m.data[4] * m.data[9] * m.data[15] -
			m.data[4] * m.data[11] * m.data[13] -
			m.data[8] * m.data[5] * m.data[15] +
			m.data[8] * m.data[7] * m.data[13] +
			m.data[12] * m.data[5] * m.data[11] -
			m.data[12] * m.data[7] * m.data[9];

		inv.data[12] = -m.data[4] * m.data[9] * m.data[14] +
			m.data[4] * m.data[10] * m.data[13] +
			m.data[8] * m.data[5] * m.data[14] -
			m.data[8] * m.data[6] * m.data[13] -
			m.data[12] * m.data[5] * m.data[10] +
			m.data[12] * m.data[6] * m.data[9];

		inv.data[1] = -m.data[1] * m.data[10] * m.data[15] +
			m.data[1] * m.data[11] * m.data[14] +
			m.data[9] * m.data[2] * m.data[15] -
			m.data[9] * m.data[3] * m.data[14] -
			m.data[13] * m.data[2] * m.data[11] +
			m.data[13] * m.data[3] * m.data[10];

		inv.data[5] = m.data[0] * m.data[10] * m.data[15] -
			m.data[0] * m.data[11] * m.data[14] -
			m.data[8] * m.data[2] * m.data[15] +
			m.data[8] * m.data[3] * m.data[14] +
			m.data[12] * m.data[2] * m.data[11] -
			m.data[12] * m.data[3] * m.data[10];

		inv.data[9] = -m.data[0] * m.data[9] * m.data[15] +
			m.data[0] * m.data[11] * m.data[13] +
			m.data[8] * m.data[1] * m.data[15] -
			m.data[8] * m.data[3] * m.data[13] -
			m.data[12] * m.data[1] * m.data[11] +
			m.data[12] * m.data[3] * m.data[9];

		inv.data[13] = m.data[0] * m.data[9] * m.data[14] -
			m.data[0] * m.data[10] * m.data[13] -
			m.data[8] * m.data[1] * m.data[14] +
			m.data[8] * m.data[2] * m.data[13] +
			m.data[12] * m.data[1] * m.data[10] -
			m.data[12] * m.data[2] * m.data[9];

		inv.data[2] = m.data[1] * m.data[6] * m.data[15] -
			m.data[1] * m.data[7] * m.data[14] -
			m.data[5] * m.data[2] * m.data[15] +
			m.data[5] * m.data[3] * m.data[14] +
			m.data[13] * m.data[2] * m.data[7] -
			m.data[13] * m.data[3] * m.data[6];

		inv.data[6] = -m.data[0] * m.data[6] * m.data[15] +
			m.data[0] * m.data[7] * m.data[14] +
			m.data[4] * m.data[2] * m.data[15] -
			m.data[4] * m.data[3] * m.data[14] -
			m.data[12] * m.data[2] * m.data[7] +
			m.data[12] * m.data[3] * m.data[6];

		inv.data[10] = m.data[0] * m.data[5] * m.data[15] -
			m.data[0] * m.data[7] * m.data[13] -
			m.data[4] * m.data[1] * m.data[15] +
			m.data[4] * m.data[3] * m.data[13] +
			m.data[12] * m.data[1] * m.data[7] -
			m.data[12] * m.data[3] * m.data[5];

		inv.data[14] = -m.data[0] * m.data[5] * m.data[14] +
			m.data[0] * m.data[6] * m.data[13] +
			m.data[4] * m.data[1] * m.data[14] -
			m.data[4] * m.data[2] * m.data[13] -
			m.data[12] * m.data[1] * m.data[6] +
			m.data[12] * m.data[2] * m.data[5];

		inv.data[3] = -m.data[1] * m.data[6] * m.data[11] +
			m.data[1] * m.data[7] * m.data[10] +
			m.data[5] * m.data[2] * m.data[11] -
			m.data[5] * m.data[3] * m.data[10] -
			m.data[9] * m.data[2] * m.data[7] +
			m.data[9] * m.data[3] * m.data[6];

		inv.data[7] = m.data[0] * m.data[6] * m.data[11] -
			m.data[0] * m.data[7] * m.data[10] -
			m.data[4] * m.data[2] * m.data[11] +
			m.data[4] * m.data[3] * m.data[10] +
			m.data[8] * m.data[2] * m.data[7] -
			m.data[8] * m.data[3] * m.data[6];

		inv.data[11] = -m.data[0] * m.data[5] * m.data[11] +
			m.data[0] * m.data[7] * m.data[9] +
			m.data[4] * m.data[1] * m.data[11] -
			m.data[4] * m.data[3] * m.data[9] -
			m.data[8] * m.data[1] * m.data[7] +
			m.data[8] * m.data[3] * m.data[5];

		inv.data[15] = m.data[0] * m.data[5] * m.data[10] -
			m.data[0] * m.data[6] * m.data[9] -
			m.data[4] * m.data[1] * m.data[10] +
			m.data[4] * m.data[2] * m.data[9] +
			m.data[8] * m.data[1] * m.data[6] -
			m.data[8] * m.data[2] * m.data[5];

		det = m.data[0] * inv.data[0] + m.data[1] * inv.data[4] + m.data[2] * inv.data[8] + m.data[3] * inv.data[12];

		if (fabsf(det) < kMaxDifference)
		{
			det = kMaxDifference;
		}

		det = 1.0f / det;

		for (i = 0; i < 16; i++) {
			inv.data[i] *= det;
		}

		return inv;
	}
}
