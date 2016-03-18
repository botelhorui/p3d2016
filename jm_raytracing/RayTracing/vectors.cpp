#include "vectors.h"
#include "Constants.h"
#include <math.h>

using namespace VectorConstants;

namespace JMVector {
	// ***** ***** vec2 ***** *****
	// Initialize vector.
	vec2::vec2() {
		x = 0;
		y = 0;
	}

	vec2::vec2(const float pX, const float pY) {
		x = pX;
		y = pY;
	}

	// Clean the rounding errors.
	void vec2::Clean() {
		/*
		float val = 37.777779;
		float rounded_down  = floorf(val * 100) / 100; // Result: 37.77
		float nearest_value = roundf(val * 100) / 100; // Result: 37.78
		float rounded_up    = ceilf (val * 100) / 100; // Result: 37.78
		*/
		//using namespace VectorConstants;

		x = roundf(x * kDecimalPlaces) / kDecimalPlaces;
		y = roundf(y * kDecimalPlaces) / kDecimalPlaces;
	}

	// Length, magnitude.
	float vec2::Length() {
		return sqrtf(powf(x, 2) + powf(y, 2));
	}

	// Normalized vector (vector / length).
	const vec2 vec2::Normalized() {
		float length = Length();
		float pX = x / length;
		float pY = y / length;

		//vec2 tempVector(pX, pY);
		return vec2(pX, pY);
	}

	// Add.
	vec2& vec2::operator+= (const vec2& vector) {
		x += vector.x;
		y += vector.y;

		return *this;
	}

	const vec2 operator+ (const vec2& vectorLeft, const vec2& vectorRight) {
		float x = vectorLeft.x + vectorRight.x;
		float y = vectorLeft.y + vectorRight.y;
		vec2 tempVector(x, y);

		return tempVector;
	}

	// Sign.
	const vec2 operator- (const vec2& vector) {
		return vec2(-vector.x, -vector.y);
	}


	// Subtract.
	vec2& vec2::operator-= (const vec2& vector) {
		x -= vector.x;
		y -= vector.y;

		return *this;
	}

	const vec2 operator- (const vec2& vectorLeft, const vec2& vectorRight) {
		float x = vectorLeft.x - vectorRight.x;
		float y = vectorLeft.y - vectorRight.y;
		vec2 tempVector(x, y);

		return tempVector;
	}

	// Scale.
	vec2& vec2::operator*= (const float scale) {
		x *= scale;
		y *= scale;

		return *this;
	}

	const vec2 operator* (float scale, const vec2& vector) {
		float x = vector.x * scale;
		float y = vector.y * scale;
		//vec2 tempVector(x, y);

		return vec2(x, y);
	}

	const vec2 operator* (const vec2& vector, const float scale) {
		float x = vector.x * scale;
		float y = vector.y * scale;
		//vec2 tempVector(x, y);

		return vec2(x, y);
	}

	// Check equality.
	const bool operator== (const vec2& vectorLeft, const vec2& vectorRight) {
		//using namespace VectorConstants;

		if (((vectorLeft.x - vectorRight.x) < kMaxDifference && (vectorLeft.x - vectorRight.x) > -kMaxDifference) &&
			((vectorLeft.y - vectorRight.y) < kMaxDifference && (vectorLeft.y - vectorRight.y) > -kMaxDifference))
		{
			return true;
		}

		return false;
	}

	// Check difference.
	const bool operator!= (const vec2& vectorLeft, const vec2& vectorRight) {
		return !operator==(vectorLeft, vectorRight);
	}

	// Dot product.
	//a · b = ax × bx + ay × by
	const float vec2::Dot(const vec2& vector) {
		return (x * vector.x + y * vector.y);
	}

	// Output the vector.
	std::ostream& operator<<(std::ostream& output, const vec2& vector) {
		output << "(" << vector.x << ", " << vector.y << ")";

		return output;
	}
	// ***** ***** vec2 ***** *****

	// ***** ***** vec3 ***** *****
	vec3::vec3() {
		x = 0;
		y = 0;
		z = 0;
	}

	vec3::vec3(const float pX, const float pY, const float pZ) {
		x = pX;
		y = pY;
		z = pZ;
	}

	// Clean the rounding errors.
	void vec3::Clean() {
		/*
		float val = 37.777779;
		float rounded_down  = floorf(val * 100) / 100; // Result: 37.77
		float nearest_value = roundf(val * 100) / 100; // Result: 37.78
		float rounded_up    = ceilf (val * 100) / 100; // Result: 37.78
		*/
		//using namespace VectorConstants;

		x = roundf(x * kDecimalPlaces) / kDecimalPlaces;
		y = roundf(y * kDecimalPlaces) / kDecimalPlaces;
		z = roundf(z * kDecimalPlaces) / kDecimalPlaces;
	}

	// Length, magnitude.
	float vec3::Length() {
		return sqrtf(powf(x, 2) + powf(y, 2) + powf(z, 2));
	}

	// Normalized vector (vector / length).
	const vec3 vec3::Normalized() {
		float length = Length();
		float pX = x / length;
		float pY = y / length;
		float pZ = z / length;

		return vec3(pX, pY, pZ);
	}

	// Add.
	vec3& vec3::operator+= (const vec3& vector) {
		x += vector.x;
		y += vector.y;
		z += vector.z;

		return *this;
	}

	const vec3 operator+ (const vec3& vectorLeft, const vec3& vectorRight) {
		float x = vectorLeft.x + vectorRight.x;
		float y = vectorLeft.y + vectorRight.y;
		float z = vectorLeft.z + vectorRight.z;

		return vec3(x, y, z);
	}

	// Sign.
	const vec3 operator- (const vec3& vector) {
		return vec3(-vector.x, -vector.y, -vector.z);
	}

	// Subtract.
	vec3& vec3::operator-= (const vec3& vector) {
		x -= vector.x;
		y -= vector.y;
		z -= vector.z;

		return *this;
	}

	const vec3 operator- (const vec3& vectorLeft, const vec3& vectorRight) {
		float x = vectorLeft.x - vectorRight.x;
		float y = vectorLeft.y - vectorRight.y;
		float z = vectorLeft.z - vectorRight.z;

		return vec3(x, y, z);
	}

	// Scale.
	vec3& vec3::operator*= (const float scale) {
		x *= scale;
		y *= scale;
		z *= scale;

		return *this;
	}

	const vec3 operator* (float scale, const vec3& vector) {
		float x = vector.x * scale;
		float y = vector.y * scale;
		float z = vector.z * scale;

		return vec3(x, y, z);
	}

	const vec3 operator* (const vec3& vector, const float scale) {
		float x = vector.x * scale;
		float y = vector.y * scale;
		float z = vector.z * scale;

		return vec3(x, y, z);
	}

	// Check equality.
	const bool operator== (const vec3& vectorLeft, const vec3& vectorRight) {
		if (((vectorLeft.x - vectorRight.x) < kMaxDifference && (vectorLeft.x - vectorRight.x) > -kMaxDifference) &&
			((vectorLeft.y - vectorRight.y) < kMaxDifference && (vectorLeft.y - vectorRight.y) > -kMaxDifference) &&
			((vectorLeft.z - vectorRight.z) < kMaxDifference && (vectorLeft.z - vectorRight.z) > -kMaxDifference))
		{
			return true;
		}
		
		return false;
	}

	// Check difference.
	const bool operator!= (const vec3& vectorLeft, const vec3& vectorRight) {
		return !operator==(vectorLeft, vectorRight);
	}

	// Cross product.
	//cx = aybz - azby
	//cy = azbx - axbz
	//cz = axby - aybx
	const vec3 vec3::Cross(const vec3& vector) {
		float xCross = y * vector.z - z * vector.y;
		float yCross = z * vector.x - x * vector.z;
		float zCross = x * vector.y - y * vector.x;

		return vec3(xCross, yCross, zCross);
	}

	// Dot product.
	//a · b = ax × bx + ay × by + az × bz
	const float vec3::Dot(const vec3& vector) {
		return (x * vector.x + y * vector.y + z * vector.z);
	}

	// Output the vector.
	std::ostream& operator<<(std::ostream& output, const vec3& vector) {
		output << "(" << vector.x << ", " << vector.y << ", " << vector.z << ")";

		return output;
	}
	// ***** ***** vec3 ***** *****

	// ***** ***** vec4 ***** *****
	vec4::vec4() {
		x = 0;
		y = 0;
		z = 0;
		w = 0;
	}

	vec4::vec4(const float pX, const float pY, const float pZ, const float pW) {
		x = pX;
		y = pY;
		z = pZ;
		w = pW;
	}

	float vec4::Length() {
		return sqrtf(powf(x, 2) + powf(y, 2) + powf(z, 2));
	}

	const vec4 vec4::Normalized() {
		float length = Length();
		float pX = x / length;
		float pY = y / length;
		float pZ = z / length;
		float pW = w / length;

		return vec4(pX, pY, pZ, pW);
	}

	const bool operator== (const vec4& vectorLeft, const vec4& vectorRight) {
		if (((vectorLeft.x - vectorRight.x) < kMaxDifference && (vectorLeft.x - vectorRight.x) > -kMaxDifference) &&
			((vectorLeft.y - vectorRight.y) < kMaxDifference && (vectorLeft.y - vectorRight.y) > -kMaxDifference) &&
			((vectorLeft.z - vectorRight.z) < kMaxDifference && (vectorLeft.z - vectorRight.z) > -kMaxDifference) &&
			((vectorLeft.w - vectorRight.w) < kMaxDifference && (vectorLeft.w - vectorRight.w) > -kMaxDifference))
		{
			return true;
		}

		return false;
	}

	std::ostream& operator<<(std::ostream& output, const vec4& vector) {
		output << "(" << vector.x << ", " << vector.y << ", " << vector.z << ", " << vector.w << ")";

		return output;
	}
	// ***** ***** vec4 ***** *****

/*
	vec4();
	const vec4 Normalized();
	friend const bool operator== (const vec4& vectorLeft, const vec4& vectorRight); // Check equality.

	friend std::ostream& operator<<(std::ostream& output, const vec4& vector); // Output the vector.
*/
}
