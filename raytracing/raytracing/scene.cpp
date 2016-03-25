#include <string>
#include <vector>
#include <fstream>
#define _USE_MATH_DEFINES
#include <math.h>

#include <glm/vec3.hpp>
#include <glm/glm.hpp>

#include "scene.h"
#include <iostream>

using namespace std;
using namespace glm;

istream &operator>>(istream &i, vec3 &v) {
	i >> v.x >> v.y >> v.z;
	return i;
}

ostream &operator<<(ostream &o, vec3 &v) {
	o << "(" << v.x << "," << v.y << "," << v.z << ")";
	return o;
}

int Scene::load_nff(string path) {
	ifstream f;
	f.open(path);
	Material lastMaterial;
	while (!f.eof()) {
		string type;
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
			cout << "camera.from: " << camera.from << endl;
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
			cout << "l.pos: " << l.pos << endl;
			// TODO color is optional
			f >> l.color;
			lights.push_back(l);
		}
		else if (type == "f") {
			f >> lastMaterial.color.r;
			f >> lastMaterial.color.g;
			f >> lastMaterial.color.b;
			cout << lastMaterial.color << endl;
			f >> lastMaterial.Kd;
			f >> lastMaterial.Ks;
			f >> lastMaterial.Shine;
			f >> lastMaterial.T;
			f >> lastMaterial.index_of_refraction;
		}
		else if (type == "pl") {
			vec3 v0, v1, v2;
			f >> v0;
			f >> v1;
			f >> v2;
			objects.push_back(new Plane(v0, v1, v2, lastMaterial));
		}
		else if (type == "s") {
			vec3 pos;
			float r;
			f >> pos;
			f >> r;
			objects.push_back(new Sphere(pos, r, lastMaterial));
		}
		else if (type == "") {
			cout << "read empty line" << endl;
		}
		else if (type == "p") {
			int numVertices;
			f >> numVertices;
			if (numVertices != 3) {
				printf("load_nff: p format expected 3 vertices. got: %d\n", numVertices);
				exit(1);
			}
			vec3 v0, v1, v2;
			f >> v0;
			f >> v1;
			f >> v2;
			objects.push_back(new Triangle(v0, v1, v2, lastMaterial));
		}
		else {
			printf("load_nff: Unrecognizable '%s' ", type.c_str());
			cin.get();
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
	vec3 xe, ye, ze;
	df = glm::length(camera.from - camera.at);
	float angle = (M_PI / 180.0f)*(camera.angle / 2.0f);
	h = 2 * df * tan(angle);
	w = (camera.resX * h) / camera.resY;
	ze = (camera.from - camera.at) / df;
	xe = normalize(cross(camera.up, ze));
	ye = normalize(cross(ze, xe));
	r.o = camera.from;
	r.d = (-df) * ze;
	r.d += (h * ((1.0f * y) / camera.resY - .5f))* ye;
	r.d += (w * ((1.0f * x) / camera.resX - .5f))* xe;
	r.d = normalize(r.d);
	return r;
}

vec3 Scene::getColor(vec3 viewDir, Material material, vec3 point, vec3 normal) {
	// multiple lights sources
	vec3 lcolor(0, 0, 0);
	for (auto & l : lights) {
		// shadow feelers
		// for each light calculate ray and call rayTracing
		Ray r;
		r.o = point;
		r.d = normalize(l.pos - r.o);
		if (isShadow(r, l)) {
			continue;
		}
		// if light is not blocked. calculate bling phong color

		// ambient component (it is not defined in the nff file


		// diffuse component
		vec3 lightDir = normalize(l.pos - point);
		float lambertian = glm::max(0.0f, dot(lightDir, normal));
		vec3 diffuse = material.color * (material.Kd * lambertian);
		lcolor += diffuse;

		// specular component
		vec3 halfDir = normalize(lightDir + viewDir);
		float spec = glm::max(0.0f, dot(halfDir, normal));
		vec3 specular = vec3(1, 1, 1) * (material.Ks * pow(spec, material.Shine));
		lcolor += specular;

	}
	//lcolor += material.color;
	return lcolor;
}

bool Scene::isShadow(Ray &ray, Light& l) {
	float lrdist = glm::length(l.pos - ray.o);
	float minDist = numeric_limits<float>::max();
	vec3 intersection, normal;
	for (auto obj : objects) {
		vec3 intersection, normal;
		float dist = obj->intersectDistance(ray, minDist, intersection, normal);
		if (dist >= 0 && dist < lrdist) {
			return true;
		}
	}
	return false;
}

bool Scene::getIntersection(const Ray& ray, Material& material, float& minDist, vec3& minIntersection, vec3&minNormal) {
	bool found = false;
	minDist = numeric_limits<float>::max();
	for (auto obj : objects) {
		vec3 intersection, normal;
		float dist = obj->intersectDistance(ray, minDist, intersection, normal);
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

vec3 Scene::rayTracing(Ray ray, int depth, float RefrIndex) {
	if (depth == 0) {
		return vec3(0, 0, 0);
	}
	float minDist;
	vec3 intersectionPoint;
	vec3  intersectionNormal;
	bool found = false;
	Material material;
	found = getIntersection(ray, material, minDist, intersectionPoint, intersectionNormal);
	if (!found) return backgroundColor;

	// intersection found. compute color:
	vec3 viewDir = normalize(ray.o - intersectionPoint);

	// from multiple lights sources
	// localColor	
	vec3 localColor(0, 0, 0);
	for (auto & l : lights) {
		// shadow feelers
		// for each light calculate ray and call rayTracing
		Ray r;
		r.o = intersectionPoint;
		r.d = normalize(l.pos - r.o);
		if (isShadow(r, l)) {
			continue;
		}
		// else light is not blocked. calculate bling phong color

		// ambient component (it is not defined in the nff file)

		// diffuse component
		vec3 lightDir = normalize(l.pos - intersectionPoint);
		float lambertian = glm::max(0.0f, dot(lightDir, intersectionNormal));

		if (lambertian > 0.0f) {

			vec3 diffuse = material.color * (material.Kd * lambertian);
			localColor += diffuse;

			// specular component

			// bling-phong imlementation
			//vec3 halfDir = normalize(lightDir + viewDir);
			//float spec = glm::max(0.0f, dot(halfDir, intersectionNormal));

			// phong implementation
			vec3 l = lightDir;
			vec3 n = intersectionNormal;
			vec3 refl = (2 * dot(n, l)*n) - l;
			float spec = glm::max(0.0f, dot(refl, viewDir));
			vec3 specular = material.color * (material.Ks * pow(spec, material.Shine));
			localColor += specular;
		}

	}
	if (depth == 1) {
		return localColor;
	}
	// diffuse color from reflected ray
	vec3 reflect = glm::reflect(-viewDir, intersectionNormal);
	Ray reflectRay;
	reflectRay.o = intersectionPoint;
	reflectRay.d = normalize(reflect);
	// by experimenting Ks is the correct ratio to use............
	vec3 reflectColor = material.Ks * rayTracing(reflectRay, depth - 1, RefrIndex);

	//refraction
	Ray refractRay;
	vec3 refractDir;
	vec3 v = viewDir;
	vec3 n = intersectionNormal;
	float eta = RefrIndex / material.index_of_refraction;
	if (false) {
		vec3 refract = glm::refract(v, n, eta);
	}
	else {
		// surface tangent vector
		vec3 vt = dot(viewDir, intersectionNormal)*intersectionNormal - viewDir;
		float sinTheta = glm::length(vt)*eta;
		float cosTheta = glm::sqrt(1.0f - sinTheta*sinTheta);
		vec3 t = glm::normalize(vt);
		refractDir = glm::normalize(sinTheta*t + cosTheta*(-n));
	}
	refractRay.d = refractDir;
	refractRay.o = intersectionPoint - ((2 * EPSILON)*n); //make sure ray start inside object
	vec3 refractColor = material.T * rayTracing(refractRay, depth - 1, material.index_of_refraction);

	vec3 globalColor = localColor + reflectColor + refractColor;
	// We use material specular value as the mix percentage, because in example images
	// the material with Kd == 0 is the only material that has no reflection
	return  globalColor;
}