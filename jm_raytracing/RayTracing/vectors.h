#ifndef JMVECTORS_H
#define JMVECTORS_H

#include <iostream>

namespace JMVector {
	// Class vec2
	class vec2 {
	public:
		float x, y;

		vec2();
		vec2(const float pX, const float pY);

		void Clean(); // Clean the rounding errors.
		float Length(); // Length, magnitude.
		const vec2 Normalized(); // Normalized vector (vector / length).

		vec2& operator+= (const vec2& vector); // Add.
		vec2& operator-= (const vec2& vector); // Subtract.
		vec2& operator*= (const float scale); // Scale.

		friend const vec2 operator+ (const vec2& vectorLeft, const vec2& vectorRight); // Add.
		friend const vec2 operator- (const vec2& vector); // Sign.
		friend const vec2 operator- (const vec2& vectorLeft, const vec2& vectorRight); // Subtract.
		friend const vec2 operator* (float scale, const vec2& vectorRight); // Scale (same as the next function).
		friend const vec2 operator* (const vec2& vectorLeft, const float scale); // Scale (same as the previous function).
		friend const bool operator== (const vec2& vectorLeft, const vec2& vectorRight); // Check equality.
		friend const bool operator!= (const vec2& vectorLeft, const vec2& vectorRight); // Check difference.

		const float Dot(const vec2& vector); // Dot product.

		friend std::ostream& operator<<(std::ostream& output, const vec2& vector); // Output the vector.
	}; // Class vec2

	// Class vec3
	class vec3 {
	public:
		float x, y, z;

		vec3();
		//vec3() {}
		vec3(const float pX, const float pY, const float pZ);
		//vec3(float pX, float pY, float pZ) : x(pX), y(pY), z(pZ) {}

		void Clean(); // Clean the rounding errors.
		float Length(); // Length, magnitude.
		const vec3 Normalized(); // Normalized vector (= vector / length).

		vec3& operator+= (const vec3& vector); // Add.
		vec3& operator-= (const vec3& vector); // Subtract.
		vec3& operator*= (const float scale); // Scale.

		friend const vec3 operator+ (const vec3& vectorLeft, const vec3& vectorRight); // Add.
		friend const vec3 operator- (const vec3& vector); // Sign.
		friend const vec3 operator- (const vec3& vectorLeft, const vec3& vectorRight); // Subtract.
		friend const vec3 operator* (float scale, const vec3& vectorRight); // Scale (same as the next function).
		friend const vec3 operator* (const vec3& vectorLeft, const float scale); // Scale (same as the previous function).
		friend const bool operator== (const vec3& vectorLeft, const vec3& vectorRight); // Check equality.
		friend const bool operator!= (const vec3& vectorLeft, const vec3& vectorRight); // Check difference.

		const vec3 Cross(const vec3& vector); // Cross product.
		const float Dot(const vec3& vector); // Dot product.

		friend std::ostream& operator<<(std::ostream& output, const vec3& vector); // Output the vector.
	};

	// Class vec4
	class vec4 {
	public:
		float x, y, z, w;

		vec4();
		vec4(const float pX, const float pY, const float pZ, const float pW);

		float Length(); // Length, magnitude.
		const vec4 Normalized();

		friend const bool operator== (const vec4& vectorLeft, const vec4& vectorRight); // Check equality.

		friend std::ostream& operator<<(std::ostream& output, const vec4& vector); // Output the vector.
	};
}

#endif