#include "vec.h"
#include "raytracer.h"
#include <algorithm>


/*
Intput: ray
Output: hit
sets hit's fields to closest intersection
*/
void Scene::calc_intersection(Ray& ray, Hit& hit) {
	if (GRID_ON)
	{
		grid.calc_hit(ray, hit);
	}else{
		for (Object* obj : objects)
		{
			obj->calcIntersection(ray, hit);
		}
	}
	// Planes are unbounded so they are processed separatedly from spheres, triangles
	for (Plane* pl : planes)
	{
		pl->calcIntersection(ray, hit);
	}
}

bool Plane::calcIntersection(const Ray& ray, Hit& hit)
{
	inc_intersection_count();
	double denom = dot(normal, ray.dir);
	double dist = -1;
	if (abs(denom) > 0)
	{
		dist = -(offset + dot(ray.origin, normal)) / denom;		
		vec3 pos = ray.origin + dist * ray.dir;
		vec3 hitNormal;
		if (denom > 0)
		{
			hitNormal = -normal; //if ray is under the plane
		}
		else
		{
			hitNormal = normal;
		}
		hit.updateMinHit(pos, hitNormal, false, mat, dist);

		return true;
	}

	return false;
}

bool Sphere::calcIntersection(const Ray& ray, Hit& hit)
{
	inc_intersection_count();
	double dist = -1;
	vec3 rayToCenter = (center - ray.origin);
	double squareLength = rayToCenter.length * rayToCenter.length;
	double squareRadius = radius * radius;
	
	bool hitNormalSignPositive;
	if (squareLength == squareRadius)
	{
		return false; // ray origin on the surface of the sphere
	}
	double B = dot(rayToCenter, ray.dir);
	if (squareLength > squareRadius && B < 0)
	{
		return false; // ray outside sphere but looking back since cos is negative
	}
	double R = B * B - squareLength + squareRadius;
	if (R < 0)
	{
		return false; // does not intersect
	}
	if (squareLength > squareRadius)
	{
		dist = B - sqrt(R);
		hitNormalSignPositive = true; // outside sphere
	}
	else
	{
		dist = B + sqrt(R);
		hitNormalSignPositive = false; // inside sphere
	}
	vec3 hitNormal;
	vec3 hitPos;
	hitPos = ray.origin + ray.dir * dist;
	bool into_inside;
	if (hitNormalSignPositive)
	{
		hitNormal = normalize(hitPos - center);
		into_inside = true;
	}
	else
	{
		hitNormal = normalize(center - hitPos);
		into_inside = false;
	}
	hit.updateMinHit(hitPos, hitNormal, into_inside, mat, dist);

	return true;
}

bool Triangle::calcIntersection(const Ray& ray, Hit& hit)
{
	inc_intersection_count();
	double dist = -1;
	double NO = dot(ray.origin, this->normal);
	double ND = dot(ray.dir, this->normal);
	if (ND == 0)
	{ // ray parallel to triangle
		return false;
	}
	dist = (-d - NO) / ND;
	if (dist < 0.0f)
	{ // triangle behind ray
		return false;
	}
	if (!hit.isCloser(dist))
	{ // a closed intersection has already been found
		return false;
	}
	vec3 P = ray.origin + dist * ray.dir;
	int i0, i1, i2; // index of the vectors of the projection of the triangle on a primary plane
	double maxN = std::max(this->normal.x, this->normal.y);
	maxN = std::max(maxN, this->normal.z);

	if (this->normal.x == maxN)
	{
		i0 = 0;
		i1 = 1;
		i2 = 2;
	}
	else if (this->normal.y == maxN)
	{
		i0 = 1;
		i1 = 0;
		i2 = 2;
	}
	else
	{
		i0 = 2;
		i1 = 0;
		i2 = 1;
	}

	double u0 = P[i1] - p1[i1];
	double v0 = P[i2] - p1[i2];
	double u1 = p2[i1] - p1[i1];
	double u2 = p3[i1] - p1[i1];
	double v1 = p2[i2] - p1[i2];
	double v2 = p3[i2] - p1[i2];
	double alpha, beta;

	if (u1 == 0)
	{
		beta = u0 / u2;
		if (0 <= beta && beta <= 1)
		{
			alpha = (v0 - beta * v2) / v1;
		}
		else
		{
			return false;
		}
	}
	else
	{
		beta = (v0 * u1 - u0 * v1) / (v2 * u1 - u2 * v1);
		if (0 <= beta && beta <= 1)
		{
			alpha = (u0 - beta * u2) / u1;
		}
		else
		{
			return false;
		}
	}
	if (alpha >= 0 && beta >= 0 && (alpha + beta) <= 1)
	{
		hit.updateMinHit(P, normal, false, mat, dist);	
		return true;
	}

	return false;
}


