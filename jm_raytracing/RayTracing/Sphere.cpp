#include "Sphere.h"

Sphere::Sphere(){

}

Sphere::~Sphere(){

}

void Sphere::SetRadius(float r) {
	radius = r;
	squareRadius = r * r;
}

float Sphere::GetRadius() {
	return radius;
}

bool Sphere::CalculateIntersection(Ray ray, float &distance, vec3 &intersectionPoint, vec3 &normalIntersection){
	bool intersection = false;
	float currentDistance;
	float squareDistance, sphereB, sphereR;
	vec3 differenceCenterOrigin = center - ray.origin;

	//squareDistance = (center.x - ray.origin.x) * (center.x - ray.origin.x) + (center.y - ray.origin.y) * (center.y - ray.origin.y) + (center.z - ray.origin.z) * (center.z - ray.origin.z);
	//squareDistance = vec3(center - ray.origin).Length() * vec3(center - ray.origin).Length();
	squareDistance = differenceCenterOrigin.Length() * differenceCenterOrigin.Length();

	if (squareDistance == squareRadius) {
		return intersection;
	}

	//sphereB = ray.direction.x * (center.x - ray.origin.x) + ray.direction.y * (center.y - ray.origin.y) + ray.direction.z * (center.z - ray.origin.z);
	//sphereB = ray.origin.x * (object.center.x - ray.direction.x) + ray.origin.y * (object.center.y - ray.direction.y) + ray.origin.z * (object.center.z - ray.direction.z);
	sphereB = ray.direction.Dot(differenceCenterOrigin);

	if (squareDistance > squareRadius) {
		if (sphereB < 0) {
			return intersection;
		}
	}

	sphereR = sphereB * sphereB - squareDistance + squareRadius;

	if (sphereR < 0){
		//printf("R inferior a 0.\n");
		return intersection;
	}

	if (squareDistance > squareRadius) {
		currentDistance = sphereB - sqrtf(sphereR);
	}
	else {
		currentDistance = sphereB + sqrtf(sphereR);
	}

	if (distance < 0) {
		distance = currentDistance;
		intersection = true;
	}
	else if (currentDistance < distance) {
		distance = currentDistance;
		intersection = true;
	}
	else {
		return intersection;
	}

	//intersectionPoint = vec3(ray.origin.x + ray.direction.x * currentDistance, ray.origin.y + ray.direction.y * currentDistance, ray.origin.z + ray.direction.z * currentDistance);
	intersectionPoint = ray.origin + ray.direction * currentDistance;

	if (squareDistance < squareRadius){
		//normalIntersection = vec3(-(intersectionPoint.x - center.x) / radius, -(intersectionPoint.y - center.y) / radius, -(intersectionPoint.z - center.z) / radius);
		//normalIntersection = -(intersectionPoint - center) * (1.0f / radius);
		normalIntersection = -(intersectionPoint - center);
	}
	else {
		//normalIntersection = vec3((intersectionPoint.x - center.x) / radius, (intersectionPoint.y - center.y) / radius, (intersectionPoint.z - center.z) / radius);
		//normalIntersection = (intersectionPoint - center) * (1.0f / radius);
		normalIntersection = (intersectionPoint - center);
	}

	normalIntersection = normalIntersection.Normalized();

	return intersection;
}
