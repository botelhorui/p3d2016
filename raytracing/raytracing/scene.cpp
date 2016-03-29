#include <string>
#include <vector>
#include <fstream>
#define _USE_MATH_DEFINES
#include <math.h>

#include <glm/vec3.hpp>
#include <glm/glm.hpp>

#include "scene.h"
#include <iostream>

std::istream &operator>>(std::istream &i, glm::vec3 &v) {
	i >> v.x >> v.y >> v.z;
	return i;
}

std::ostream &operator<<(std::ostream &o, glm::vec3 &v) {
	o << "(" << v.x << "," << v.y << "," << v.z << ")";
	return o;
}

int Scene::load_nff(std::string path) {
	std::ifstream f;
	f.open(path);
	Material lastMaterial;
	while (!f.eof()) {
		std::string type;
		f >> type;
		if (type == "b") {
			f >> backgroundColor.r;
			f >> backgroundColor.g;
			f >> backgroundColor.b;

		}
		else if (type == "v") {
			f >> type;
			assert(type == "from");
			f >> camera.from;
			f >> type;
			std::cout << "camera.from: " << camera.from << std::endl;
			assert(type == "at");
			f >> camera.at;
			f >> type;
			assert(type == "up");
			f >> camera.up;
			f >> type;
			assert(type == "angle");
			f >> camera.angle;
			f >> type;
			assert(type == "hither");
			f >> camera.hither;
			f >> type;
			assert(type == "resolution");
			f >> camera.resX;
			f >> camera.resY;
		}
		else if (type == "l") {
			Light l;
			f >> l.pos;
			std::cout << "l.pos: " << l.pos << std::endl;
			// TODO color is optional
			f >> l.color;
			lights.push_back(l);
		}
		else if (type == "f") {
			f >> lastMaterial.color.r;
			f >> lastMaterial.color.g;
			f >> lastMaterial.color.b;
			std::cout << lastMaterial.color << std::endl;
			f >> lastMaterial.Kd;
			f >> lastMaterial.Ks;
			f >> lastMaterial.Shine;
			f >> lastMaterial.T;
			f >> lastMaterial.ior;
		}
		else if (type == "pl") {
			glm::vec3 v0, v1, v2;
			f >> v0;
			f >> v1;
			f >> v2;
			objects.push_back(new Plane(v0, v1, v2, lastMaterial));
		}
		else if (type == "s") {
			glm::vec3 pos;
			float r;
			f >> pos;
			f >> r;
			objects.push_back(new Sphere(pos, r, lastMaterial));
		}
		else if (type == "") {
			std::cout << "read empty line" << std::endl;
		}
		else if (type == "p") {
			int numVertices;
			f >> numVertices;
			if (numVertices != 3) {
				printf("load_nff: p format expected 3 vertices. got: %d\n", numVertices);
				exit(1);
			}
			glm::vec3 v0, v1, v2;
			f >> v0;
			f >> v1;
			f >> v2;
			objects.push_back(new Triangle(v0, v1, v2, lastMaterial));
		}
		else {
			printf("load_nff: Unrecognizable '%s' ", type.c_str());
			std::cin.get();
			exit(1);
		}
	}
	printf("number of lights: %d\n", lights.size());
	printf("number of planes: %d\n", planes.size());
	printf("number of spheres: %d\n", spheres.size());
	printf("number of triangles: %d\n", triangles.size());
	return 1;
}

Ray Scene::calculatePrimaryRay(int x, int y) {
	// from "2 - Ray_Tracing_Practice.pdf"
	// page 20
	Ray r;
	float df, h, w;
	glm::vec3 xe, ye, ze;
	df = glm::length(camera.from - camera.at);
	float angle = ((float)M_PI / 180.0f)*(camera.angle / 2.0f);
	h = 2 * df * tan(angle);
	w = (camera.resX * h) / camera.resY;
	ze = (camera.from - camera.at) / df;
	xe = glm::normalize(glm::cross(camera.up, ze));
	ye = glm::normalize(glm::cross(ze, xe));
	r.o = camera.from;
	r.d = (-df) * ze;
	r.d += (h * ((1.0f * y) / camera.resY - .5f))* ye;
	r.d += (w * ((1.0f * x) / camera.resX - .5f))* xe;
	r.d = glm::normalize(r.d);
	return r;
}


bool Scene::isShadow(Ray &ray, Light& l) {
	float lrdist = glm::length(l.pos - ray.o);
	float minDist = std::numeric_limits<float>::max();
	glm::vec3 intersection, normal;
	bool intoInside;
	for (auto obj : objects) {
		float dist = obj->intersectDistance(ray, minDist, intersection, normal, intoInside);
		if (dist >= 0 && dist < lrdist) {
			return true;
		}
	}
	return false;
}

bool Scene::getIntersection(const Ray& ray, Material& material, float& minDist, glm::vec3& minIntersection, glm::vec3&minNormal, bool& intoInside) {
	bool found = false;
	minDist = std::numeric_limits<float>::max();
	for (auto obj : objects) {
		glm::vec3 intersection, normal;
		float dist = obj->intersectDistance(ray, minDist, intersection, normal, intoInside);
		if (dist <= 0) continue;
		if (found == false || dist < minDist) {
			minDist = dist;
			minIntersection = intersection;
			minNormal = normal;
			material = obj->material;
			found = true;
		}
	}
	return found;
}

glm::vec3 Scene::rayTracing(Ray ray, int depth, float ior) {
	if (depth == 0) {
		return glm::vec3(0, 0, 0);
	}
	float minDist;
	glm::vec3 intersectionPoint;
	glm::vec3  intersectionNormal;
	bool found = false;
	bool intoInside = false;
	Material material;
	found = getIntersection(ray, material, minDist, intersectionPoint, intersectionNormal, intoInside);
	if (!found) {
		return backgroundColor;
	}

	// intersection found. compute color:
	glm::vec3 viewDir = glm::normalize(ray.o - intersectionPoint);

	// from multiple lights sources
	// localColor	
	glm::vec3 localColor(0, 0, 0);
	for (auto & light : lights) {
		// shadow feelers
		// for each light calculate ray and call rayTracing
		Ray r;
		r.o = intersectionPoint;
		r.d = glm::normalize(light.pos - r.o);
		r.o += r.d * EPSILON;
		if (isShadow(r, light)) {
			continue;
		}
		// else light is not blocked. calculate bling phong color
		// ambient component (it is not defined in the nff file)

		// diffuse component
		glm::vec3 lightDir = glm::normalize(light.pos - intersectionPoint);
		float lambertian = glm::max(0.0f, glm::dot(lightDir, intersectionNormal));

		if (lambertian > 0.0f) {
			glm::vec3 diffuse = material.color * (material.Kd * lambertian);
			localColor += diffuse;

			// specular component

			// bling-phong imlementation
			glm::vec3 halfDir = glm::normalize(lightDir + viewDir);
			float spec = glm::max(0.0f, glm::dot(halfDir, intersectionNormal));

			// phong implementation
			//glm::vec3 l = lightDir;
			//glm::vec3 n = intersectionNormal;
			//glm::vec3 refl = (2 * glm::dot(n, l)*n) - l;
			//float spec = glm::max(0.0f, glm::dot(refl, viewDir));
			glm::vec3 specular = material.color * (material.Ks * 0.3f * pow(spec, material.Shine));
			localColor += specular;
		}
	}

	if (depth == 1) {
		return localColor;
	}

	glm::vec3 reflectColor;

	if (material.Ks > 0.0f) {
		// diffuse color from reflected ray
		glm::vec3 reflect = glm::reflect(-viewDir, intersectionNormal);
		Ray reflectRay;
		reflectRay.o = intersectionPoint;
		reflectRay.d = glm::normalize(reflect);
		reflectRay.o += EPSILON * reflectRay.d;
		// by experimenting Ks is the correct ratio to use............
		reflectColor = rayTracing(reflectRay, depth - 1, ior);
	}

	glm::vec3 refractColor;

	if (material.T > 0.0f) {
		//refraction
		// https://en.wikipedia.org/wiki/Snell%27s_law#Vector_form
		// or professor slides
		Ray refractRay;
		glm::vec3 refractDir;
		glm::vec3 v = viewDir;
		glm::vec3 n = intersectionNormal;
		float eta;
		float tior;
		//if(intoInside)
		if (intoInside)
		{
			// from outside object into inside
			//eta = ior / material.ior;
			tior = material.ior;
		}
		else
		{
			// from inside object to outside
			//eta = ior / 1.0f;
			tior = 1.0f;
		}

		eta = ior / tior;
		// surface tangent vector
		float dot = glm::dot(viewDir, intersectionNormal);

		if (ior > tior) {
			float angle = glm::acos(dot);
			float criticalAngle = glm::asin(tior / ior);

			if (angle > criticalAngle) {
				return localColor + material.Ks*reflectColor;
			}
		}

		glm::vec3 vt = glm::dot(viewDir, intersectionNormal)*intersectionNormal - viewDir;
		float sinTheta = glm::length(vt)*eta;
		float cosTheta = glm::sqrt(1.0f - sinTheta*sinTheta);
		glm::vec3 t = glm::normalize(vt);
		refractDir = glm::normalize(sinTheta*t - cosTheta*n);

		refractRay.d = refractDir;
		refractRay.o = intersectionPoint;
		refractRay.o += refractRay.d * EPSILON; //make sure ray start inside object

		refractColor = rayTracing(refractRay, depth - 1, material.ior);

		if (false) {
			// testing glm refract but it does not work
			glm::vec3 refract = glm::normalize(glm::refract(v, n, eta));
		}
		else {
			// surface tangent vector
			float dot = glm::dot(viewDir, intersectionNormal);
			if (ior > tior) {
				float angle = glm::acos(dot);
				float criticalAngle = glm::asin(tior / ior);
				if (angle > criticalAngle) {
					return localColor + material.Ks*reflectColor;
				}
			}
			glm::vec3 vt = dot*intersectionNormal - viewDir;
			float sinTheta = glm::length(vt)*eta;
			float cosTheta = glm::sqrt(1.0f - sinTheta*sinTheta);
			glm::vec3 t = glm::normalize(vt);
			refractDir = glm::normalize(sinTheta*t - cosTheta*n);
		}
	}

	glm::vec3 globalColor = localColor + material.Ks * reflectColor + material.T * refractColor;
	// We use material specular value as the mix percentage, because in example images
	// the material with Kd == 0 is the only material that has no reflection
	return  globalColor;
}