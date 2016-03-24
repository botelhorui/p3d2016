#include "Plane.h"
#include "Constants.h"

using namespace VectorConstants;

Plane::Plane(){

}

Plane::~Plane(){

}

void Plane::CalculateNormal() {
	vec3 auxLeft = vertices[2] - vertices[1];
	vec3 auxRight = vertices[1] - vertices[0];
	//vec3 normal = auxLeft.Cross(auxRight);
	normal = auxLeft.Cross(auxRight);
	normal = normal.Normalized();
}

bool Plane::CalculateIntersection(Ray ray, float &distance, vec3 &intersectionPoint, vec3 &normalIntersection) {
	bool intersection = false;
	float currentDistance;
	float dotNormalDirection = normal.Dot(ray.direction);

	if (fabsf(dotNormalDirection) > kMaxDifference) { // Raio paralelo ao plano (perpendicular à normal)
		currentDistance = -(normal.Dot(ray.origin) - normal.Dot(vertices[0])) / dotNormalDirection;

		if (currentDistance < 0) {
			return intersection;
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
		normalIntersection = -normal;
	}

	return intersection;
}
