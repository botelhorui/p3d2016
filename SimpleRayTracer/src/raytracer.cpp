#include "raytracer.h"
#include <fstream>
#include <string>
#define _USE_MATH_DEFINES
#include <math.h>
#include <algorithm>

// TODO merge reflect refract and rSchlick2
vec3 reflect(const vec3& normal, const vec3& incident)
{
	const double cosI = -dot(normal, incident);
	return incident + 2 * cosI*normal;
}
// TODO merge reflect refract and rSchlick2
bool refract(const vec3 normal, const vec3& incident, double n1, double n2, vec3& reflect_dir)
{
	const double n = n1 / n2;
	const double cosI = -dot(normal, incident);
	const double sinT2 = n * n * (1.0 - cosI*cosI);
	if (sinT2 > 1.0) return false; // TIR
	const double cosT = sqrt(1.0 - sinT2);
	reflect_dir = n * incident + (n*cosI - cosT) * normal;
	reflect_dir = normalize(reflect_dir);
	return true;
}
// TODO merge reflect refract and rSchlick2
double rSchlick2(const vec3& normal, const vec3& incident, double n1, double n2)
{
	double r0 = (n1 - n2) / (n1 + n2);
	r0 *= r0;
	double cosX = -dot(normal, incident);
	if(n1 > n2)
	{
		const double n = n1 / n2;
		const double sinT2 = n*n*(1.0 - cosX*cosX);
		if (sinT2 > 1.0) return 1.0; //TIR
		cosX = sqrt(1.0 - sinT2);
	}
	const double x = 1.0 - cosX;
	return r0 + (1.0 - r0) * x * x * x * x * x;
}


Ray Scene::calculate_primary_ray(int x, int y)
{
	// from "2 - Ray_Tracing_Practice.pdf"
	// page 20
	Ray r;
	double df; //distance to frame
	double h; // frame height
	double w; // frame width
	vec3 xe, ye, ze; // camera space
	ze = camera.from - camera.at;
	df = ze.length;
	ze = normalize(ze);
	double angle; // half of the field of view
	angle = (M_PI / 180.0) * (camera.angle / 2.0);
	h = 2 * df * tan(angle);
	w = (camera.res_x * h) / camera.res_y; // height * ratio
	xe = normalize(cross(camera.up, ze));
	ye = normalize(cross(ze, xe));
	r.origin = camera.from;
	r.dir = (w * ((1.0 * x) / camera.res_x - .5)) * xe +
		(h * ((1.0 * y) / camera.res_y - .5)) * ye +
		(-df) * ze;
	r.dir = normalize(r.dir);
	return r;
}


vec3 Scene::calc_refract_color(Ray ray, Hit& hit)
{
	if(hit.mat.T <= 0.0)
	{
		return vec3();
	}
	float ior = hit.mat.ior;
	if(!hit.into_inside)
	{
		ior = 1.0;
	}
	vec3 refract_dir;
	if(refract(hit.normal, ray.dir, ray.ior, ior, refract_dir))
	{
		Ray refractRay(hit.pos, refract_dir, ray.depth - 1, ior);
		refractRay.offset(EPSILON);
		return ray_trace(refractRay);
	}
		// Total internal reflection
		return vec3();
	
	
}

vec3 Scene::calc_reflect_color(Ray ray, Hit& hit)
{
	
	if (hit.mat.Ks <= 0.0f)
	{
		return vec3(0, 0, 0);
	}
	vec3 reflectDir = reflect(hit.normal, ray.dir);
	Ray reflectRay(hit.pos, reflectDir, ray.depth - 1, ray.ior);
	reflectRay.offset(EPSILON);
	return ray_trace(reflectRay);
}

void Scene::calc_shadow_intersections(Ray& ray, Hit& hit)
{
	for (auto& obj : planes)
	{
		obj.calcIntersection(ray, hit);
		//if (hit.dist > 0) return;
	}
	for (auto& obj : spheres)
	{
		obj.calcIntersection(ray, hit);
		//if (hit.dist > 0) return;
	}
	for (auto& obj : triangles)
	{
		obj.calcIntersection(ray, hit);
		//if (hit.dist > 0) return;
	}
}

vec3 Scene::calc_local_color(Ray& ray, Hit& hit)
{
	vec3 localColor(0, 0, 0);
	for (auto& light: lights)
	{
		Hit shadow_hit;
		vec3 light_vec = light.pos - hit.pos;
		vec3 light_dir = normalize(light_vec);
		Ray shadow_ray(hit.pos, light_dir, 0, 0.0);
		shadow_ray.offset(EPSILON);
		calc_shadow_intersections(shadow_ray, shadow_hit);
		if (shadow_hit.dist > 0 && shadow_hit.dist < light_vec.length)
		{
			continue;
		}
		vec3 diffuseColor, specularcolor;
		double lambertian = dot(light_dir, hit.normal);
		if (lambertian > 0.0f)
		{
			diffuseColor = hit.mat.color * hit.mat.Kd * lambertian;
			//specular
			vec3 viewDir = normalize(ray.origin - hit.pos);
			vec3 halfDir = normalize(light_dir + viewDir);
			double specAngle = dot(halfDir, hit.normal);
			double specular = pow(specAngle, hit.mat.Shine * 1.f);
			if (specular > 0.0f)
			{
				//specularcolor = hit.mat.color * hit.mat.Ks * specular * 0.6f;
				specularcolor = vec3(1.f) * hit.mat.Ks * specular * 0.3f;
			}
		}
		localColor +=  diffuseColor + specularcolor;
	}
	return localColor;
}


void Scene::calc_intersection(Ray& ray, Hit& hit)
{
	for (auto& obj : planes)
	{
		obj.calcIntersection(ray, hit);
	}
	for (auto& obj : spheres)
	{
		obj.calcIntersection(ray, hit);
	}
	for (auto& obj : triangles)
	{
		obj.calcIntersection(ray, hit);
	}
}

vec3 Scene::ray_trace(Ray ray)
{
	if (ray.depth == 0)
	{
		return vec3(0);
	}
	Hit hit;
	calc_intersection(ray, hit);
	if (hit.dist == -1)
	{
		return backgroundColor;
	}
	vec3 local_color = calc_local_color(ray, hit);

	double reflectance = rSchlick2(hit.normal, ray.dir, ray.ior, hit.mat.ior);
	double transmitance = 1.0 - reflectance;
	vec3 reflect_color = calc_reflect_color(ray, hit);
	vec3 refract_color = calc_refract_color(ray, hit);
	return local_color + (1.0 - hit.mat.T) * reflect_color + hit.mat.T * refract_color;
	//return local_color + reflectance * reflect_color + transmitance * refract_color;

}
