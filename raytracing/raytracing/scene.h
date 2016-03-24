#include <string>
#include <vector>
#include <fstream>
#include <limits>
#define _USE_MATH_DEFINES
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

class Object {
public:
	Material material;
	Object(Material mat) : material(mat){}
	virtual float intersectDistance(const Ray& ray, const float& minDist, vec3& intersection, vec3& normal) const {
		return -1;
	};
};
class Plane: public Object{
public:
	vec3 n;
	float d;
	Plane(vec3 v0, vec3 v1, vec3 v2, Material mat): Object(mat){
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

class Sphere: public Object{
public:
	vec3 pos;
	float r;
	Sphere(vec3 pos, float r, Material mat) : pos(pos), r(r), Object(mat) {
	}

	float intersectDistance(const Ray& ray, const float& minDist, vec3& intersection, vec3& normal) const {
		vec3 diff = (pos - ray.o);
		float t = -1;
		float distsquare = glm::length(diff) * glm::length(diff);
		
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

class Triangle: public Object{
public:
	vec3 V0;
	vec3 V1;
	vec3 V2;
	vec3 normal;
	// 
	float d;


	Triangle(vec3 v0, vec3 v1, vec3 v2, Material mat) : V0(v0), V1(v1), V2(v2), Object(mat){
		normal = normalize(cross(v1 - v0, v2 - v0));
		// for each point P of the plane, P.N is constant
		d = -dot(v0, normal);
	}
	float intersectDistance(const Ray& ray, const float& minDist, vec3& intersection, vec3& normal) const {
		float NO = dot(ray.o, this->normal);
		float ND = dot(ray.d, this->normal);
		if (ND == 0) { // ray parallel to triangle
			return -1;
		}
		float t = (-d -NO) / ND;
		if (t < 0.0f) { // triangle behind ray
			return -1;
		}
		if (minDist < t) { // a closed intersection has already been found
			return -1;
		}
		vec3 P = ray.o + t*ray.d;
		int i0, i1, i2; // index of the vectors of the projection of the triangle on a primary plane
		float maxN = glm::max(this->normal.x, this->normal.y);
		maxN = glm::max(maxN, this->normal.z);
		if (this->normal.x == maxN) { 
			i0 = 0;
			i1 = 1;
			i2 = 2;
		}
		else if (this->normal.y == maxN) {
			i0 = 1; 
			i1 = 0;
			i2 = 2;
		}
		else {
			i0 = 2;
			i1 = 0;
			i2 = 1;
		}

		float u0 = P[i1] - V0[i1];
		float v0 = P[i2] - V0[i2];
		float u1 = V1[i1] - V0[i1];
		float u2 = V2[i1] - V0[i1];
		float v1 = V1[i2] - V0[i2];
		float v2 = V2[i2] - V0[i2];
		float alpha, beta;

		if (u1 == 0) {
			beta = u0 / u2;
			if (0 <= beta && beta <= 1) {
				alpha = (v0 - beta * v2) / v1;
			}
			else {
				return -1;
			}
		}
		else {
			beta = (v0*u1 - u0*v1) / (v2*u1 - u2*v1);
			if (0 <= beta && beta <= 1) {
				alpha = (u0 - beta * u2) / u1;
			}
			else {
				return -1;
			}
		}

		if (alpha >= 0 && beta >= 0 && (alpha + beta) <= 1) {
			
			normal = this->normal;
			intersection = P;
			intersection += normal * EPSILON;
			return t;
		}
		
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
	vector<Object*> objects;

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
			}else {
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

	Ray calculatePrimaryRay(int x, int y) {
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

	vec3 getColor(vec3 viewDir, Material material, vec3 point, vec3 normal) {
		// multiple lights sources
		vec3 lcolor(0, 0, 0);
		for (auto & l: lights) {
			// shadow feelers
			// for each light calculate ray and call rayTracing
			Ray r;
			r.o = point;
			r.d = normalize(l.pos - r.o);
			if (isShadow(r,l)) {
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
			vec3 specular = vec3(1,1,1) * (material.Ks * pow(spec, material.Shine));			
			lcolor += specular;
			
		}	
		//lcolor += material.color;
		return lcolor;
	}

	bool isShadow(Ray &ray, Light& l) {
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

	bool getIntersection(const Ray& ray, Material& material, float& minDist, vec3& minIntersection, vec3&minNormal) {
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


	vec3 rayTracing(Ray ray, int depth, float RefrIndex) {
		if (depth == 0) {
			return vec3(0,0,0);
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
		vec3 refract = glm::refract(-viewDir, intersectionNormal, RefrIndex/material.index_of_refraction);
		Ray refractRay;
		refractRay.o = intersectionPoint - ((3* EPSILON)*intersectionNormal); //make sure ray start inside object
		refractRay.d = normalize(refract);
		vec3 refractColor = material.Ks * rayTracing(refractRay, depth - 1, material.index_of_refraction);
		refractColor = max(refractColor, vec3(0, 0, 0));
		

		vec3 globalColor = localColor + reflectColor + refractColor;
		// We use material specular value as the mix percentage, because in example images
		// the material with Kd == 0 is the only material that has no reflection
		return  globalColor;
	}
};
