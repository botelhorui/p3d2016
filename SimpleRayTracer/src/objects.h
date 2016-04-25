#pragma once
#include <vector>
#include <iostream>
#include "vec.h"
#include <algorithm>

class Hit;
class Ray;

class BBox
{
public:
	vec3 min;
	vec3 max;
	bool inside(vec3 v)
	{
		return min.x < v.x && v.x < max.x &&
			min.y < v.y && v.y < max.y &&
			min.z < v.z && v.z < max.z;
	}
};


class Material {
public:
	vec3 color;
	double Kd;
	double Ks;
	double Shine;
	double T;
	double ior;
};

class Object
{
public:
	virtual ~Object() {}
	Material mat;
	BBox bbox;
	Object(Material mat) :mat(mat) {};
	// output: hit
	// updates hit if the ray intersects closer
	virtual void calcIntersection(const Ray& ray, Hit& hit) = 0;
	virtual bool isBounded() = 0;
};

class Plane : public Object {
public:
	vec3 normal;
	double offset;

	Plane(vec3 v0, vec3 v1, vec3 v2, Material mat) : Object(mat)
	{
		normal = normalize(cross(v1 - v0, v2 - v0));
		offset = -dot(v0, normal);
	}
	void calcIntersection(const Ray& ray, Hit& hit) override;
	virtual bool isBounded() { return false; }
};

class Sphere : public Object {
public:
	vec3 center;
	double radius;
	Sphere(vec3 pos, double radius, Material mat) : Object(mat), center(pos), radius(radius)
	{
		bbox.min = center - radius - EPSILON;
		bbox.max = center + radius + EPSILON;
	}
	void calcIntersection(const Ray& ray, Hit& hit) override;
	virtual bool isBounded() { return true; }
};

class Triangle : public Object {
public:
	vec3 p1, p2, p3;
	vec3 normal;
	double d;

	Triangle(vec3 v0, vec3 v1, vec3 v2, Material mat) :
		Object(mat), p1(v0), p2(v1), p3(v2)
	{
		normal = normalize(cross(v1 - v0, v2 - v0));
		// for each point P of the plane, P.N is constant
		d = -dot(v0, normal);
		// find min/max for each coordinate x,y,z
		double minx = min3(p1.x, p2.x, p3.x);
		double miny = min3(p1.y, p2.y, p3.y);
		double minz = min3(p1.z, p2.z, p3.z);
		double maxx = max3(p1.x, p2.x, p3.x);
		double maxy = max3(p1.y, p2.y, p3.y);
		double maxz = max3(p1.z, p2.z, p3.z);
		bbox.min = vec3(minx, miny, minz)-EPSILON;
		bbox.max = vec3(maxx, maxy, maxz)+EPSILON;
	}

	void calcIntersection(const Ray& ray, Hit& hit) override;
	virtual bool isBounded() { return true; }
};