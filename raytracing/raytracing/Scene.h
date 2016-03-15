#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <limits>


#include <glm/vec3.hpp>
#include <glm/glm.hpp>

using namespace std;
using namespace glm;

istream &operator>>(istream &i, vec3 &v){
	 
	i >> v.x >> v.y >> v.z;	
	return i;
}


class Light {
public:
	vec3 pos;
	vec3 color;
};

class Ray {
public:
	vec3 o;
	vec3 d;
};

class Camera {
public:
	vec3 from;
	vec3 at;
	vec3 up;
	float angle;
	float hither;
	float resX;
	float resY;

	int getResX() {
		return 0;
	}
	int getResY() {
		return 0;
	}
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

class SceneObject {
public:
	Material material;
	virtual float intersectDistance(Ray r, vec3& intersection) = 0;
	virtual ~SceneObject() = 0;
};
SceneObject::~SceneObject(){}

class Plane: public SceneObject {
public:
	vec3 p1;
	vec3 p2;
	vec3 p3;
	float intersectDistance(Ray r, vec3& intersection) {
		return -1;
	}
};

class Sphere: public SceneObject {
public:
	vec3 pos;
	float r;
	float intersectDistance(Ray r, vec3& intersection) {
		return -1;
	}
};

class Scene {
	
	
public:
	Camera camera;
	vec3 backgroundColor;
	vector<Light> lights;
	vector<SceneObject*> objects;
		
	int load_nff(string path) {
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
				// TODO color is optional
				f >> l.color;
				lights.push_back(l);
			}
			else if (type == "f") {
				f >> lastMaterial.color.r;
				f >> lastMaterial.color.g;
				f >> lastMaterial.color.b;
				f >> lastMaterial.Kd;
				f >> lastMaterial.Ks;
				f >> lastMaterial.Shine;
				f >> lastMaterial.T;
				f >> lastMaterial.index_of_refraction;
			}
			else if (type == "pl") {
				Plane *pl = new Plane();
				f >> pl->p1;
				f >> pl->p2;
				f >> pl->p3;
				pl->material = lastMaterial;
				objects.push_back(pl);
			}
			else if (type == "s") {
				Sphere *s = new Sphere();
				f >> s->pos;
				f >> s->r;
				s->material = lastMaterial;
				objects.push_back(s);
			}
			else {
				cout << "load_nff: Unrecognizable type " << type << endl;
				exit(1);
			}
		}
		return 1;
	}

	Ray calculatePrimaryRay(int x, int y) {
		Ray r;
		float df, h, w;
		vec3 xe, ye, ze;				
		df = (float)(camera.from - camera.at).length();
		h = 2 * df * tan(camera.angle);
		w = (camera.resX * h) / camera.resY;
		ze = (camera.from - camera.at) / df;
		xe = normalize(cross(camera.up, ze));
		ye = cross(ze, xe);
		r.o = camera.from;
		r.d = -df * ze;
		r.d += h * (1.0f * y / camera.resY - .5f)* ye;
		r.d += w * (1.0f * x / camera.resX - .5f)* xe;
		return r;
	}

	vec3 rayTracing(Ray ray, int depth, float RefrIndex) {
		float minDistance = numeric_limits<float>::max();
		vec3 minIntersection;
		SceneObject *objIntersection;
		for (vector<SceneObject*>::iterator it = objects.begin(); it != objects.end(); it++) {
			vec3 intersection;
			SceneObject *obj = *it;
			float dist = obj->intersectDistance(ray, intersection);
			if(dist < 0) {
				continue;
			}
			if (dist < minDistance) {
				minDistance = dist;
				minIntersection = intersection;
				objIntersection = obj;
			}
		}
		// do color calculations ...
		// TODO
		if (minDistance == numeric_limits<float>::max()) {
			return vec3(0, 0, 0);
		}
		else {
			return vec3(1, 1, 1);
		}		
	}
};
