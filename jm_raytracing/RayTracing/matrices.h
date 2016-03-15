#ifndef JMMATRICES_H
#define JMMATRICES_H

#include <iostream>
#include "vectors.h"

#undef near
#undef far

using namespace JMVector;

namespace JMMatrix {
	class mat2 {
	public:
		float data[4];

		mat2();
		void Clean(); // Clean the rounding errors.
		float Determinant() const;
		friend mat2 Inverse(const mat2& matrix);
		friend mat2 Transpose(const mat2& matrix);

		mat2& operator+= (const mat2& matrix); // Add.
		mat2& operator-= (const mat2& matrix); // Subtract.
		mat2& operator*= (const float scalar); // Scale.

		friend const mat2 operator+ (const mat2& matrixLeft, const mat2& matrixRight); // Add.
		friend const mat2 operator- (const mat2& matrix); // Subtract.
		friend const mat2 operator- (const mat2& matrixLeft, const mat2& matrixRight); // Subtract.
		friend const mat2 operator* (float scalar, const mat2& matrix); // Scale (same as the next function).
		friend const mat2 operator* (const mat2& matrix, const float scalar); // Scale (same as the previous function).
		friend const mat2 operator* (const mat2& matrixLeft, const mat2& matrixRight);
		friend const vec2 operator* (const mat2& matrix, const vec2& vector);
		friend const bool operator== (const mat2& matrixLeft, const mat2& matrixRight); // Check equality.
		friend const bool operator!= (const mat2& matrixLeft, const mat2& matrixRight); // Check difference.

		friend std::ostream& operator<<(std::ostream& output, const mat2& matrix); // Output the vector.

	private:
	}; // Class mat2

	class mat3 {
	public:
		float data[9];

		mat3();
		void Clean(); // Clean the rounding errors.
		float Determinant() const;
		friend mat3 Inverse(const mat3& matrix);
		friend mat3 Transpose(const mat3& matrix);

		mat3& operator+= (const mat3& matrix); // Add.
		mat3& operator-= (const mat3& matrix); // Subtract.
		mat3& operator*= (const float scalar); // Scale.

		friend const mat3 operator+ (const mat3& matrixLeft, const mat3& matrixRight); // Add.
		friend const mat3 operator- (const mat3& matrix); // Subtract.
		friend const mat3 operator- (const mat3& matrixLeft, const mat3& matrixRight); // Subtract.
		friend const mat3 operator* (float scalar, const mat3& matrix); // Scale (same as the next function).
		friend const mat3 operator* (const mat3& matrix, const float scalar); // Scale (same as the previous function).
		friend const mat3 operator* (const mat3& matrixLeft, const mat3& matrixRight);
		friend const vec3 operator* (const mat3& matrix, const vec3& vector);
		friend const bool operator== (const mat3& matrixLeft, const mat3& matrixRight); // Check equality.
		friend const bool operator!= (const mat3& matrixLeft, const mat3& matrixRight); // Check difference.

		friend std::ostream& operator<<(std::ostream& output, const mat3& matrix); // Output the vector.

	private:
	}; // Class mat4

	class mat4 {
	public:
		float data[16];

		mat4();
		void Clean(); // Clean the rounding errors.
/*
		float Determinant() const;
		friend mat4 Inverse(const mat4& matrix);
*/
		friend mat4 Transpose(const mat4& matrix);

		friend const mat4 operator+ (const mat4& matrixLeft, const mat4& matrixRight); // Add.
		friend const mat4 operator- (const mat4& matrix); // Subtract.
		friend const mat4 operator- (const mat4& matrixLeft, const mat4& matrixRight); // Subtract.
		friend const mat4 operator* (float scalar, const mat4& matrix); // Scale (same as the next function).
		friend const mat4 operator* (const mat4& matrix, const float scalar); // Scale (same as the previous function).
		friend const mat4 operator* (const mat4& matrixLeft, const mat4& matrixRight);
		friend const vec4 operator* (const mat4& matrix, const vec4& vector);
		//friend const vec4 operator* (const mat4& matrix, const vec4& vector);
		friend const bool operator== (const mat4& matrixLeft, const mat4& matrixRight); // Check equality.
		friend const bool operator!= (const mat4& matrixLeft, const mat4& matrixRight); // Check difference.

		friend std::ostream& operator<<(std::ostream& output, const mat4& matrix); // Output the vector.

	private:
	}; // Class mat4

	class MatrixFactory {
	public:
		// mat3
		static const mat3 createZeroMat3();
		static const mat3 createIdentityMat3();
		static const mat3 createMat3FromMat4(const mat4& matrix);
		static const mat3 createDualMat3(const vec3& vector);
		static const mat3 createRotationMat3(const float angle, const vec3& axis);
		static const mat3 createScaleMat3(const float scale);
		static const mat3 createScaleMat3(const vec3& vector);
		static const mat3 createTranslationMat3(const vec2& vector);
		// mat4
		static const mat4 createZeroMat4();
		static const mat4 createIdentityMat4();
		static const mat4 createMat4FromMat3(const mat3& matrix);
		static const mat4 createRotationMat4(const float angle, const vec3& axis);
		static const mat4 createRotationInverseMat4(const mat4& matrix);
		static const mat4 createRotationFromViewMat4(const mat4& view);
		static const mat4 createScaleMat4(const float scale);
		static const mat4 createScaleMat4(const vec3& vector);
		static const mat4 createTranslationMat4(const vec3& vector);
		static const mat4 createTranslationInverseMat4(const mat4& matrix);
		static const mat4 createTranslationFromViewMat4(const mat4& view);
		// View Matrix from (eye, center, up) parameters.
		static const mat4 createViewMat4(const vec3& eye, const vec3& center, const vec3& up);
		static const mat4 createViewInverseMat4(const mat4& view);
		// Orthographic Projection Matrix from (left-right, bottom-top, near-far) parameters.
		static const mat4 createProjectionOrthographicMat4(const float left, const float right, const float bottom, const float top, const float near, const float far);
		// Perspective Projection Matrix from (fovy, aspect, nearZ, farZ) parameters.
		static const mat4 createProjectionPerspectiveMat4(const float fovy, const float aspect, const float nearZ, const float farZ);
		static const mat4 createProjectionPerspectiveMat4(const float fovy, const float width, const float height, const float nearZ, const float farZ);
		static const mat4 createInverseProjectionPerspectiveMat4(const mat4& perspective);
		static const mat4 createInverseMat4(const mat4& m);
	private:
	};
}

#endif