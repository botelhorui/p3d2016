#pragma once
#include <istream>
#include <cassert>

#define EPSILON (double)1e-4
class vec3
{
	double calcLength(double x, double y, double z) const
	{
		return sqrt(x * x + y * y + z * z);
	}

public:
	double length, x, y, z;

	vec3() : vec3(0, 0, 0)
	{
	}

	vec3(double v) : vec3(v, v, v)
	{
	}

	vec3(double x, double y, double z) : x(x), y(y), z(z), length(calcLength(x, y, z))
	{
	}

	vec3& operator+=(const vec3& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	double const& operator[](int const& i) const
	{
		assert(i < 3 && "Vector index out of bounds");
		if (i == 0)return x;
		if (i == 1)return y;
		return z;
	}
};

inline vec3 normalize(vec3 v)
{
	double l = v.length;
	return vec3(v.x / l, v.y / l, v.z / l);
}

inline vec3 cross(vec3 x, vec3 y)
{
	return vec3(
		x.y * y.z - y.y * x.z,
		x.z * y.x - y.z * x.x,
		x.x * y.y - y.x * x.y);
}

inline double dot(vec3 v0, vec3 v1)
{
	return v0.x * v1.x +
		v0.y * v1.y +
		v0.z * v1.z;
}


inline vec3 operator-(vec3 const& v0, vec3 const& v1)
{
	return vec3(v0.x - v1.x, v0.y - v1.y, v0.z - v1.z);
}

inline vec3 operator+(vec3 const& v0, vec3 const& v1)
{
	return vec3(v0.x + v1.x, v0.y + v1.y, v0.z + v1.z);
}

inline vec3 operator*(double const& f, vec3 const& v)
{
	return vec3(f * v.x, f * v.y, f * v.z);
}

inline vec3 operator*(vec3 const& v, double const& f)
{
	return vec3(f * v.x, f * v.y, f * v.z);
}

inline vec3 operator/(double const& f, vec3 const& v)
{
	return vec3(v.x / f, v.y/f, v.z/f);
}

inline vec3 operator/(vec3 const& v, double const& f)
{
	return vec3(v.x / f, v.y / f, v.z / f);
}

inline vec3 operator-(vec3 const& v)
{
	return vec3(-v.x, -v.y, -v.z);
}

inline std::istream& operator>>(std::istream& i, vec3& v)
{
	return i >> v.x >> v.y >> v.z;
}

inline double min3(double x, double y, double z)
{
	double min = x;
	if (y < min) min = y;
	if (z < min) min = z;
	return min;
}

inline double max3(double x, double y, double z)
{
	double max = x;
	if (y > max) max = y;
	if (z > max) max = z;
	return max;
}

inline double random()
{
	return (double)rand() / RAND_MAX;
}