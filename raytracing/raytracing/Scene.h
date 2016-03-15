#pragma once
#include <string>
#include <vector>
#include <fstream>

#include <glm/vec3.hpp>


using namespace std;
using namespace glm;

istream &operator>>(istream &i, vec3 &v){
	 
	i >> v.x >> v.y >> v.z;
	cout << "operator>> red '" << v.x << "," << v.y << "," << v.z << "'" << endl;
	return i;
}


class Light {
public:
	vec3 pos;
	vec3 color;
};

class Ray {
public:

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
	virtual ~SceneObject() = 0;
};
SceneObject::~SceneObject(){}

class Plane: public SceneObject {
public:
	vec3 p1;
	vec3 p2;
	vec3 p3;
};

class Sphere: public SceneObject {
public:
	vec3 pos;
	float r;
};

class Scene {
	
	
public:
	Camera camera;
	vec3 backgroundColor;
	vector<Light> lights;
		
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
				Plane pl;
				f >> pl.p1;
				f >> pl.p2;
				f >> pl.p3;
				pl.material = lastMaterial;
			}
			else if (type == "s") {
				Sphere s;
				f >> s.pos;
				f >> s.r;
				s.material = lastMaterial;
			}
			else {
				cout << "load_nff: Unrecognizable type " << type << endl;
				exit(1);
			}
		}
		return 1;
	}

	Ray calculatePrimaryRay(int x, int y) {
		return Ray();
	}

	vec3 rayTracing(Ray ray, int depth, float RefrIndex) {
		//INSERT HERE YOUR CODE
		return vec3();
	}
};
