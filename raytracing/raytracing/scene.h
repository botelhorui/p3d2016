#include <string>
#include <vector>
#include <limits>
#include <glm/vec3.hpp>
#include <glm/glm.hpp>
#include "object.h"

#ifndef SCENE
#define SCENE
using namespace std;
using namespace glm;

#define EPSILON 0.001f

class Light {
public:
	vec3 pos;
	vec3 color;
};

class Camera {
public:
	vec3 from;
	vec3 at;
	vec3 up;
	float angle;
	float hither;
	int resX;
	int resY;
};

class Scene {
public:
	Camera camera;
	vec3 backgroundColor;
	vector<Light> lights;
	// we have one vector for each type to increase speed
	vector<Plane> planes;
	vector<Sphere> spheres;
	vector<Triangle> triangles;
	vector<Object*> objects;

	int load_nff(string path);

	Ray calculatePrimaryRay(int x, int y);

	vec3 getColor(vec3 viewDir, Material material, vec3 point, vec3 normal);

	bool isShadow(Ray &ray, Light& l);

	bool getIntersection(const Ray& ray, Material& material, float& minDist, vec3& minIntersection, vec3&minNormal);


	vec3 rayTracing(Ray ray, int depth, float RefrIndex);
};
#endif
