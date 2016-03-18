#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <limits>
#include <math.h>

#include <glm/vec3.hpp>
#include <glm/glm.hpp>

using namespace std;
using namespace glm;

#define EPSILON 0.001f

istream &operator>>(istream &i, vec3 &v){
	 
	i >> v.x >> v.y >> v.z;	
	return i;
}

ostream &operator<<(ostream &o, vec3 &v) {

	o << "(" << v.x << "," <<v.y << "," << v.z << ")";
	return o;
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
	int resX;
	int resY;

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

class Plane{
public:
	vec3 n;
	float d;
	Material material;
	Plane(vec3 v0, vec3 v1, vec3 v2, Material mat):material(mat) {
		vec3 s0, s1;
		s0 = v1 - v0;
		s1 = v2 - v0;
		n = normalize(cross(s0, s1));
		d = -dot(v0, n);
	}
	float intersectDistance(const Ray& ray, const float& minDist, vec3& intersection, vec3& normal) const {
		float denom = dot(n, ray.d);
		float t = -1;
		if (abs(denom) > 0.0001f) {
			t = -  (d + dot(ray.o, n))/denom;
			if (t > 0.0001f) {
				intersection = ray.o + t*ray.d;
				normal = n;
			}
			else {
				t = -1;
			}
		}		
		return t;
	}
};

class Sphere{
public:
	vec3 pos;
	float r;
	Material material;
	Sphere(vec3 pos, float r, Material mat) : pos(pos), r(r), material(mat) {
	}

	float intersectDistance(const Ray& ray, const float& minDist, vec3& intersection, vec3& normal) const {
		vec3 diff = (pos - ray.o);
		float t = -1;
		float distsquare = dot(diff, diff);
		if (distsquare == r*r) return -1; // ray on the sphere
		float B = dot(ray.d, diff);
		if (distsquare > r*r) {
			if (B < 0) return -1; // sphere behind eye
		}
		float R = B*B - distsquare + r*r;
		if (R < 0) return -1; // does not intersect sphere
		if (distsquare > r*r) {
			t = B - glm::sqrt(R);
		}
		else {
			t = B + glm::sqrt(R);
		}
		intersection = ray.o + t *ray.d;
		normal = normalize(intersection - pos);
		intersection += normal * EPSILON;
		return t;
	}
};

class Triangle{
public:
	vec3 v0;
	vec3 v1;
	vec3 v2;	
	Material material;
	Triangle(vec3 v0, vec3 v1, vec3 v2, Material mat) : v0(v0), v1(v1), v2(v2), material(mat) {
	}
	float  intersectDistance(const Ray& ray,const float& minDist,const vec3& intersection, vec3& normal) const {
		return -1;
	}
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
				planes.push_back(Plane(v0, v1, v2, lastMaterial));
			}
			else if (type == "s") {
				vec3 pos;
				float r;
				f >> pos;
				f >> r;
				spheres.push_back(Sphere(pos, r, lastMaterial));
			}
			else if (type == "") {

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
				triangles.push_back(Triangle(v0, v1, v2, lastMaterial));
			}else {
				printf("load_nff: Unrecognizable '%s' ", type);
				cin.get();
				exit(1);
			}
		}
		printf("number of planes: %d\n", planes.size());
		printf("number of spheres: %d\n", spheres.size());
		printf("number of triangles: %d\n", triangles.size());
		return 1;
	}

	Ray calculatePrimaryRay(int x, int y) {
		// from "2 - Ray_Tracing_Practice.pdf"
		// page 20
		Ray r;
		float df, h, w;
		vec3 xe, ye, ze;				
		df = (float)(camera.from - camera.at).length();
		h = 2 * df * tan(camera.angle / 2.0f);
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

	vec3 getColor(vec3 viewDir, Material material, vec3 point, vec3 normal) {
		// multiple lights sources
		vec3 lcolor(0, 0, 0);
		vec3 Kd = material.color * material.Kd;
		vec3 Ks = material.color *  material.Ks;
		for (auto & l: lights) {
			// shadow feelers
			// for each light calculate ray call rayTracing
			Ray r;
			r.o = point;
			r.d = normalize(l.pos - r.o);
			if (isShadow(r,l)) {
				continue;
			}
			// else calculate bling phong color
			vec3 lightDir = normalize(l.pos - point);			
			float lambertian = dot(lightDir, normal);
			if (lambertian < 0) continue;
			vec3 halfDir = normalize(lightDir + viewDir);
			float spec = glm::max(0.0f, dot(halfDir, normal));
			vec3 diffuse = l.color * Kd * lambertian;
			vec3 specular = l.color * Ks * pow(spec, material.Shine);
			lcolor += diffuse;
			lcolor += specular;
		}	

		//lcolor += backgroundColor * Kd;
		return lcolor;
	}

	bool isShadow(Ray &ray, Light& l) {
		float lrdist = (l.pos - ray.o).length();
		float minDist = numeric_limits<float>::max();
		for (auto const& pl : planes) {
			vec3 intersection, normal;
			float dist = pl.intersectDistance(ray, minDist, intersection, normal);
			if (dist >= EPSILON && dist < lrdist) {
				return true;
			}			
		}
		for (auto const& s : spheres) {
			vec3 intersection, normal;
			float dist = s.intersectDistance(ray, minDist, intersection, normal);
			if (dist >= EPSILON && dist < lrdist) {
				return true;
			}
		}
		for (auto const& t : triangles) {
			vec3 intersection, normal;
			float dist = t.intersectDistance(ray, minDist, intersection, normal);
			if (dist >= EPSILON && dist < lrdist) {
				return true;
			}
		}
		return false;
	}

	bool getIntersection(const Ray& ray, Material& material, float& minDist, vec3& minIntersection, vec3&minNormal) {
		bool found = false;

		
		for (auto const& plane : planes) {
			vec3 intersection, normal;
			float dist = plane.intersectDistance(ray, minDist, intersection, normal);
			if (dist <= 0) continue;
			if (found == false || dist < minDist) {
				minDist = dist;
				minIntersection = intersection;
				minNormal = normal;
				material = plane.material;
				found = true;
			}
		}

		for (auto const& sphere : spheres) {
			vec3 intersection, normal;
			float dist = sphere.intersectDistance(ray, minDist, intersection, normal);
			if (dist <= 0) continue;
			if (found == false || dist < minDist) {
				minDist = dist;
				minIntersection = intersection;
				minNormal = normal;
				material = sphere.material;
				found = true;
			}
		}

		for (auto const& triangle : triangles) {
			vec3 intersection, normal;
			float dist = triangle.intersectDistance(ray, minDist, intersection, normal);
			if (dist <= 0) continue;
			if (found == false || dist < minDist) {
				minDist = dist;
				minIntersection = intersection;
				minNormal = normal;
				material = triangle.material;
				found = true;
			}
		}

		return found;
	}


	vec3 rayTracing(Ray ray, int depth, float RefrIndex) {
		if (depth == 0) {
			return backgroundColor;
		}
		float minDist;
		vec3 minIntersection;
		vec3  minNormal;
		bool found = false;
		Material material;
		found = getIntersection(ray, material, minDist, minIntersection, minNormal);		
		if (!found) return backgroundColor;

		vec3 viewDir = normalize(ray.o - minIntersection);
		vec3 reflect = glm::reflect(-viewDir, minNormal);
		// calculate reflection from other objects
		Ray reflectRay;
		reflectRay.o = minIntersection;
		reflectRay.d = normalize(reflect);
		vec3 reflectColor = rayTracing(reflectRay, depth - 1, RefrIndex);
		// localColor		
		vec3 localColor = getColor(viewDir, material, minIntersection, minNormal);
		// use specular because in image example plane floor does not reflect sphere and Ks is 0
		return mix(localColor, reflectColor, material.Ks);		
	}
};
