#pragma once
#include <vector>
#include <iostream>
#include "vec.h"
#include <algorithm>
#include "objects.h"



// TODO later
// https://stackoverflow.com/questions/201593/is-there-a-simple-way-to-convert-c-enum-to-string/201770#201770
enum DrawMode
{
	NORMAL,
	MONTE_CARLO,
	DOF,
	DOF_MONTE_CARLO,
	GRID,
};
/*
Draw Mode:
3 - full threaded
4 - full threaded monte carlo
5 - full threaded DoF
6 - full threaded DoF + monte carlo
7 - full threaded grid acceleration
*/
extern DrawMode DRAW_MODE;

// Ray tracing
extern int MAX_DEPTH;
// Monte Carlo
extern int MAX_DIVISIONS;
extern double MONTE_CARLO_THRESHOLD;
// Depth of field
extern int DEPTH_OF_FIELD_SAMPLES;
// Soft shadows
extern const bool SOFT_SHADOWS_ON;
enum soft_shadow_type { AREA, SPHERE};
extern const soft_shadow_type SOFT_SHADOW_TYPE;
extern const int SOFT_SHADOWS_SAMPLES;
// Area Soft Shadow
extern const vec3 LIGHT_AREA_U;
extern const vec3 LIGHT_AREA_V;
extern const double LIGHT_AREA_SIDE;
// Sphere Soft Shadow
extern double LIGHT_RADIUS;
//GRID acceleration
extern int GRID_WIDTH_MULTIPLIER;
extern long INTERSECTION_TESTS_COUNT;


class Scene;

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
	vec3 area_x = LIGHT_AREA_U;
	vec3 area_y = LIGHT_AREA_V;
	vec3 color;
	Light(vec3 pos, vec3 color, vec3 a, vec3 b) : pos(pos), color(color), area_x(a), area_y(b) {}
};

class Camera {
public:
	vec3 from;
	vec3 at;
	vec3 up;
	double focalDist=3;
	double viewDist=0.5;
	double apperture=0.25;
	double angle;
	double hither;
	int res_x;
	int res_y;
};

class Grid {
public:	
	void initializeGrid();
	Scene* scene;
	std::vector<std::vector<Object*>> cells;
	BBox bbox;
	int nx, ny, nz; // number of cells x, y, z
	double wx, wy, wz;
	vec3 min_coordinates();
	vec3 max_coordinates();
	void calc_hit(Ray& ray, Hit& hit);
};

class Scene {
public:
	Camera camera;
	Grid grid;
	vec3 backgroundColor;
	std::vector<Object*> objects;
	std::vector<Plane*> planes;
	std::vector<Sphere*> spheres;
	std::vector<Triangle*> triangles;
	std::vector<Light> lights;

	int load_nff(std::string path);
	Ray calculate_primary_ray(int x, int y);
	// Colors
	vec3 calc_reflect_color(Ray ray, Hit& hit);
	vec3 calc_local_color(Ray& ray, Hit& hit);
	vec3 calc_refract_color(Ray ray, Hit& hit);	
	
	vec3 ray_trace(int x, int y);
	vec3 ray_trace_dof(double x, double y);
	vec3 ray_trace_monte_carlo(int x, int y);
	vec3 ray_trace_monte_carlo_dof(int x, int y);

	void Scene::calc_shadow_ray(Hit& hit, Light& light_pos, Ray& ray_out);
	void free();
private:
	void calc_intersection(Ray& ray, Hit& hit);
	vec3 ray_trace(Ray ray);
	Ray calculate_primary_ray_monte_carlo(double x, double y, double deltaX, double deltaY);
	vec3 calculatePrimaryRaysMonteCarlo(int divisions, double x, double y);
	vec3 calculateSecondaryRaysMonteCarlo(int divisions, double x, double y, vec3 colorPixelsPrevious[4][4], int index);
	vec3 calculatePrimaryRaysMonteCarlo_DoF(int divisions, double x, double y);
	vec3 calculateSecondaryRaysMonteCarlo_DoF(int divisions, double x, double y, vec3 colorPixelsPrevious[4][4], int index);
};
