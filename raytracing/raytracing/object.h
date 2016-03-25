#include <glm/vec3.hpp>

#ifndef OBJECT
#define OBJECT

class Ray {
public:
	glm::vec3 o;
	glm::vec3 d;
};

class Material {
public:
	glm::vec3 color;
	float Kd;
	float Ks;
	float Shine;
	float T;
	float ior;
};

class Object {
public:
	Material material;
	Object(Material mat) : material(mat) {}
	virtual float intersectDistance(const Ray& ray, const float& minDist, glm::vec3& intersection, glm::vec3& normal, bool& intoInside) const {
		return -1;
	};
};
class Plane : public Object {
public:
	glm::vec3 n;
	float d;
	Plane(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, Material mat) : Object(mat) {
		glm::vec3 s0, s1;
		s0 = v1 - v0;
		s1 = v2 - v0;
		n = glm::normalize(glm::cross(s0, s1));
		d = -glm::dot(v0, n);
	}
	float intersectDistance(const Ray& ray, const float& minDist, glm::vec3& intersection, glm::vec3& normal, bool& intoInside) const;
};

class Sphere : public Object {
public:
	glm::vec3 pos;
	float radius;
	Sphere(glm::vec3 pos, float r, Material mat) : pos(pos), radius(r), Object(mat) {
	}

	float intersectDistance(const Ray& ray, const float& minDist, glm::vec3& intersection, glm::vec3& normal, bool& intoInside) const;
};

class Triangle : public Object {
public:
	glm::vec3 V0;
	glm::vec3 V1;
	glm::vec3 V2;
	glm::vec3 normal;
	// 
	float d;
	Triangle(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, Material mat) : V0(v0), V1(v1), V2(v2), Object(mat) {
		normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));
		// for each point P of the plane, P.N is constant
		d = -glm::dot(v0, normal);
	}
	float intersectDistance(const Ray& ray, const float& minDist, glm::vec3& intersection, glm::vec3& normal, bool& intoInside) const;
};
#endif
