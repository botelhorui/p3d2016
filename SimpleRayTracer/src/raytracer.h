#pragma once
#include <vector>
#include <iostream>
#include "vec.h"

/*							Mode:	2		3
balls_low.nff",				//0		
balls_medium.nff",			//1		1.6
balls_high.nff",			//2		126		
mount_low.nff",				//3		0.5
mount_high.nff",			//4		81.8	20
mount_very_high.nff"		//5		1405	400
balls_low_large.nff",		//6
"scenes/test1.nff",			//7
"scenes/test1_small.nff",	//8
*/
#define SCENE_FILE 8
#define MAX_DEPTH 6
#define MAX_DIVISIONS 2
#define MONTE_CARLO_THRESHOLD 0.3

/*
Draw Mode:
	0 - point by point;
	1 - line by line;
	2 - full frame,
	3 - full threaded
	4 - full threaded monte carlo
	5 - full threaded DoF
	6 - full threaded DoF + monte carlo
*/
#define DRAW_MODE 3
#define DEPTH_OF_FIELD_SAMPLES 50


class Material {
public:
	vec3 color;
	double Kd;
	double Ks;
	double Shine;
	double T;
	double ior;
};

class Hit {
public:
	vec3 pos;
	vec3 normal;
	bool into_inside = false;
	Material mat;
	double dist = -1;
	bool found = false;

	bool isCloser(double newDist) const {
		return (dist == -1 || newDist < dist);
	}

	void updateMinHit(vec3& newPos, vec3& newNormal, bool newIntoInside, Material & newMat, double& newDist) {
		if (newDist > 0 && (!found || newDist < dist))
		{
			pos = newPos;
			normal = newNormal;
			into_inside = newIntoInside;
			mat = newMat;
			dist = newDist;
			found = true;
		}
	}
};


class Ray {
public:
	vec3 origin;
	vec3 dir;
	int depth;
	double ior;

	Ray() {

	}

	Ray(vec3& origin, vec3 dir, int depth, double ior) : origin(origin), dir(dir), depth(depth), ior(ior) {

	}

	void offset(double x) {
		origin += dir * x;
	}
};

class Light {
public:
	vec3 pos;
	vec3 a, b;
	vec3 color;
	Light(vec3 pos, vec3 color, vec3 a, vec3 b) : pos(pos), color(color), a(a), b(b) {}
};

class Camera {
public:
	vec3 from;
	vec3 at;
	vec3 up;
	double focalDist;
	double viewDist;
	double apperture;
	double angle;
	double hither;
	int res_x;
	int res_y;
};

class Plane {
public:
	vec3 normal;
	double offset;
	Material mat;

	Plane(vec3 v0, vec3 v1, vec3 v2, Material mat) : mat(mat)
	{
		normal = normalize(cross(v1 - v0, v2 - v0));
		offset = -dot(v0, normal);
	}

	void calcIntersection(const Ray& ray, Hit& hit);
};

class Sphere {
public:
	vec3 center;
	double radius;
	Material mat;

	Sphere(vec3 pos, double radius, Material mat) : center(pos), radius(radius), mat(mat)
	{
	}

	void calcIntersection(const Ray& ray, Hit& hit);
};

class Triangle {
public:
	vec3 p1, p2, p3;
	vec3 normal;
	double d;
	Material mat;

	Triangle(vec3 v0, vec3 v1, vec3 v2, Material mat) :
		mat(mat), p1(v0), p2(v1), p3(v2)
	{
		normal = normalize(cross(v1 - v0, v2 - v0));
		// for each point P of the plane, P.N is constant
		d = -dot(v0, normal);
	}

	void calcIntersection(const Ray& ray, Hit& hit);
};

class Grid {
public:
	void addPlane(Plane plane);
	void addSphere(Sphere sphere);
	void addTriangle(Triangle triangle);
	void initializeGrid(int objects);

private:
	int width;
	int height;
	int depth;
	int objectsCount;
	vec3 minimum;
	vec3 maximum;
	vec3 cells;
	vec3 cellDimension;

	std::vector<Plane> planes;
	std::vector<Sphere> spheres;
	std::vector<Triangle> triangles;

	void calculateMinimumCoordinates();
	void calculateMaximumCoordinates();
	void calculateCells();

	// Tests
	std::vector<vec3> boundingValues {
		vec3(4.2, 7.3, 0.51),
		vec3(3.5, 5.8, 0.50),
		vec3(9.2, 9.4, 0.52),
		vec3(7.4, 1.8, 7.4),
		vec3(1.7, 2.4, 4.6),
	};
};

class Scene {
public:
	Camera camera;
	Grid grid;
	vec3 backgroundColor;
	std::vector<Light> lights;
	std::vector<Plane> planes;
	std::vector<Sphere> spheres;
	std::vector<Triangle> triangles;
	// code to use in case we only want one for to interate over all objects
	//typedef double(*calcIntersection)(const Ray&, vec3&, vec3&, bool&);
	//std::vector<calcIntersection> calls;

	int load_nff(std::string path);
	Ray calculate_primary_ray(int x, int y);

	vec3 calc_reflect_color(Ray ray, Hit& hit);
	vec3 calc_local_color(Ray& ray, Hit& hit);
	void calc_shadow_intersections(Ray& ray, Hit& hit);
	vec3 calc_refract_color(Ray ray, Hit& hit);
	void calc_intersection(Ray& ray, Hit& hit);
	vec3 ray_trace(Ray ray);
	vec3 ray_trace_dof(double x, double y);
	vec3 ray_trace_monte_carlo(int x, int y);
	vec3 ray_trace_monte_carlo_dof(int x, int y);

	void Scene::calc_shadow_ray(Hit& hit, Light& light_pos, Ray& ray_out);

private:
	Ray calculate_primary_ray_monte_carlo(float x, float y, float deltaX, float deltaY);
	vec3 calculatePrimaryRaysMonteCarlo(int divisions, float x, float y);
	vec3 calculateSecondaryRaysMonteCarlo(int divisions, float x, float y, vec3 colorPixelsPrevious[4][4], int index);
	vec3 calculatePrimaryRaysMonteCarlo_DoF(int divisions, float x, float y);
	vec3 calculateSecondaryRaysMonteCarlo_DoF(int divisions, float x, float y, vec3 colorPixelsPrevious[4][4], int index);
};
