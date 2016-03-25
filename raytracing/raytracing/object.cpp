#include <string>
#define _USE_MATH_DEFINES
#include <math.h>

#include <glm/vec3.hpp>
#include <glm/glm.hpp>
#include "scene.h"

float Plane::intersectDistance(const Ray& ray, const float& minDist, glm::vec3& intersection, glm::vec3& normal) const {
	float denom = glm::dot(n, ray.d);
	float t = -1;
	if (abs(denom) > EPSILON) {
		t = -(d + glm::dot(ray.o, n)) / denom;
		if (t > EPSILON) {
			intersection = ray.o + t*ray.d;
			normal = n;
		}
		else {
			t = -1;
		}
	}
	return t;
}


float Sphere::intersectDistance(const Ray& ray, const float& minDist, glm::vec3& intersection, glm::vec3& normal) const {
	glm::vec3 diff = (pos - ray.o);
	float distance = -1;
	float distsquare = glm::length(diff) * glm::length(diff);	
	if (distsquare == radius*radius) {
		return distance;
	}
	float B = glm::dot(ray.d, diff);
	if (distsquare > radius*radius) {
		if (B < 0){
			return distance; // sphere behind eye
		}
	}
	float R = B*B - distsquare + radius*radius;
	if (R < 0)
	{
		return -1; // does not intersect sphere
	}
	if (distsquare > radius*radius) {
		distance = B - glm::sqrt(R);
	}
	else {
		distance = B + glm::sqrt(R);
	}
	intersection = ray.o + distance *ray.d;
	normal = glm::normalize(intersection - pos);
	if(distsquare < radius*radius)
	{
		normal = -normal;
	}
	return distance;
}

float Triangle::intersectDistance(const Ray& ray, const float& minDist, glm::vec3& intersection, glm::vec3& normal) const {
	float NO = glm::dot(ray.o, this->normal);
	float ND = glm::dot(ray.d, this->normal);
	if (ND == 0) { // ray parallel to triangle
		return -1;
	}
	float t = (-d - NO) / ND;
	if (t < 0.0f) { // triangle behind ray
		return -1;
	}
	if (minDist < t) { // a closed intersection has already been found
		return -1;
	}
	glm::vec3 P = ray.o + t*ray.d;
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