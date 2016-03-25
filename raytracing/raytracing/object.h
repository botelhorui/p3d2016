#include <glm/vec3.hpp>

using namespace std;
using namespace glm;

#ifndef OBJECT
#define OBJECT

class Ray {
public:
	vec3 o;
	vec3 d;
};

class Material {
public:
	vec3 color;
	float Kd;
	float Ks;
	float Shine;
	float T;
	float index_of_refraction;
};

class Object {
public:
	Material material;
	Object(Material mat) : material(mat) {}
	virtual float intersectDistance(const Ray& ray, const float& minDist, vec3& intersection, vec3& normal) const {
		return -1;
	};
};
class Plane : public Object {
public:
	vec3 n;
	float d;
	Plane(vec3 v0, vec3 v1, vec3 v2, Material mat) : Object(mat) {
		vec3 s0, s1;
		s0 = v1 - v0;
		s1 = v2 - v0;
		n = normalize(cross(s0, s1));
		d = -dot(v0, n);
	}
	float intersectDistance(const Ray& ray, const float& minDist, vec3& intersection, vec3& normal) const;
};

class Sphere : public Object {
public:
	vec3 pos;
	float r;
	Sphere(vec3 pos, float r, Material mat) : pos(pos), r(r), Object(mat) {
	}

	float intersectDistance(const Ray& ray, const float& minDist, vec3& intersection, vec3& normal) const;
};

class Triangle : public Object {
public:
	vec3 V0;
	vec3 V1;
	vec3 V2;
	vec3 normal;
	// 
	float d;
	Triangle(vec3 v0, vec3 v1, vec3 v2, Material mat) : V0(v0), V1(v1), V2(v2), Object(mat) {
		normal = normalize(cross(v1 - v0, v2 - v0));
		// for each point P of the plane, P.N is constant
		d = -dot(v0, normal);
	}
	float intersectDistance(const Ray& ray, const float& minDist, vec3& intersection, vec3& normal) const;
};
#endif
