#include <string>
#include <vector>
#include <limits>
#include <glm/vec3.hpp>
#include <glm/glm.hpp>
#include "object.h"

#ifndef SCENE
#define SCENE

#define EPSILON (float)1e-5

class Light {
public:
	glm::vec3 pos;
	glm::vec3 color;
};

class Camera {
public:
	glm::vec3 from;
	glm::vec3 at;
	glm::vec3 up;
	float angle;
	float hither;
	int resX;
	int resY;
};

class Scene {
public:
	Camera camera;
	glm::vec3 backgroundColor;
	std::vector<Light> lights;
	// we have one vector for each type to increase speed
	std::vector<Plane> planes;
	std::vector<Sphere> spheres;
	std::vector<Triangle> triangles;
	std::vector<Object*> objects;

	int load_nff(std::string path);

	Ray calculatePrimaryRay(int x, int y);

	glm::vec3 getColor(glm::vec3 viewDir, Material material, glm::vec3 point, glm::vec3 normal);

	bool isShadow(Ray &ray, Light& l);

	bool getIntersection(const Ray& ray, Material& material, float& minDist, glm::vec3& minIntersection, glm::vec3&minNormal, bool& intoInside);


	glm::vec3 rayTracing(Ray ray, int depth, float RefrIndex);
};
#endif
