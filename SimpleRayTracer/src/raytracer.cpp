#include "raytracer.h"
#include <fstream>
#include <string>
#define _USE_MATH_DEFINES
#include <math.h>
vec3 normalize(vec3 v)
{
	float l = v.length;
	return vec3(v.x / l, v.y / l, v.z / l);
}

vec3 cross(vec3 x, vec3 y)
{
	return vec3(
		x.y * y.z - y.y * x.z,
		x.z * y.x - y.z * x.x,
		x.x * y.y - y.x * x.y);
}
float dot(vec3 v0, vec3 v1)
{
	return v0.x*v1.x +
		v0.y*v1.y +
		v0.z*v1.z;
}


Plane::Plane(vec3 v0, vec3 v1, vec3 v2, Material mat):Object(mat)
{
	normal = normalize(cross(v1 - v0, v2 - v0));
	offset = -dot(v0, normal);
}
float Plane::calcIntersection(const Ray& ray, vec3& hPoint, vec3& nPoint, bool& intoInside)
{
	float denom = dot(normal, ray.direction);
	float t = -1;
	if(abs(denom) > 0)
	{
		t = -(offset + dot(ray.origin,normal)) / denom;
		if(t > 0)
		{
			hPoint = ray.origin + t*ray.direction;
			if(denom > 0)
			{
				nPoint = -normal; //if ray is under the plane
			}else
			{
				nPoint = normal;
			}
		}
	}
	intoInside = false;
	return t;
}
float Sphere::calcIntersection(const Ray& ray, vec3& hPoint, vec3& nPoint, bool& intoInside)
{
	return -1;
}
Triangle::Triangle(vec3 v0, vec3 v1, vec3 v2, Material mat):
	Object(mat),v0(v0), v1(v1), v2(v2)  {
	normal = normalize(cross(v1 - v0, v2 - v0));
	// for each point P of the plane, P.N is constant
	d = -dot(v0, normal);
}



float Triangle::calcIntersection(const Ray& ray, vec3& hPoint, vec3& nPoint, bool& intoInside)
{
	return -1;
}


Ray Scene::calculatePrimaryRay(int x, int y)
{
	// from "2 - Ray_Tracing_Practice.pdf"
	// page 20
	Ray r;
	float df; //distance to frame
	float h; // frame height
	float w; // frame width
	vec3 xe, ye, ze; // camera space
	ze = camera.from - camera.at;
	df = ze.length;
	ze = normalize(ze);
	float angle; // half of the field of view
	angle = ((float)M_PI / 180.0f)*(camera.angle / 2.0f);
	h = 2 * df * tan(angle);
	w = (camera.res_x * h) / camera.res_y; // height * ratio
	xe = normalize(cross(camera.up, ze));
	ye = normalize(cross(ze, xe));
	r.origin = camera.from;
	r.direction = (w * ((1.0f * x) / camera.res_x - .5f))* xe +
		(h * ((1.0f * y) / camera.res_y - .5f))* ye +
		(-df) * ze;
	r.direction = normalize(r.direction);
	return r;
}

float Scene::calcIntersection(const Ray& ray, Object* & hitObject, vec3& minHitPoint, vec3& minNormalPoint, bool& minIntoInside)
{
	float minDist = -1;
	for (auto obj : objects)
	{
		vec3 hPoint, nPoint;
		bool intoInside;
		float dist = obj->calcIntersection(ray, hPoint, nPoint, intoInside);
		if (dist > 0 && (minDist == -1 || dist < minDist))
		{
			minDist = dist;
			minHitPoint = hPoint;
			minNormalPoint = nPoint;
			minIntoInside = intoInside;
			hitObject = obj;
		}
	}
	return minDist;
}

vec3 Scene::calcRefractColor(Ray ray, Object* object)
{
}

vec3 Scene::calcReflectColor(Ray ray, Object* object)
{
}

vec3 Scene::calcLocalColor(Ray ray, Object * object)
{
	return vec3();
}

vec3 Scene::rayTrace(Ray ray, int depth, float ior)
{
	if (depth == 0) {
		return vec3(0);
	}
	float minDist = -1;
	vec3 minHitPoint;
	vec3 minNormalPoint;
	bool minIntoInside;
	Object* hitObject = nullptr;
	minDist = calcIntersection(ray, hitObject, minHitPoint, minNormalPoint, minIntoInside);	
	if(minDist == -1)
	{
		return backgroundColor;
	}
	vec3 localColor = calcLocalColor(ray, hitObject);
	vec3 reflectColor = calcReflectColor(ray, hitObject);
	vec3 refractColor = calcRefractColor(ray, hitObject);
	return localColor + reflectColor + refractColor;
}
