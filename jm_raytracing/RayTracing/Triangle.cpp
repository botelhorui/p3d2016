#include "Triangle.h"
#include "Constants.h"

using namespace VectorConstants;

Triangle::Triangle() {

}

Triangle::~Triangle() {

}

void Triangle::CalculateNormal() {
	vec3 auxLeft = vertices[2] - vertices[1];
	vec3 auxRight = vertices[1] - vertices[0];
	normal = auxLeft.Cross(auxRight);
	normal = normal.Normalized();
}

bool Triangle::CalculateIntersection(Ray ray, float &distance, vec3 &intersectionPoint, vec3 &normalIntersection) {
	bool intersection = false;
	float currentDistance;
	float dotNormalDirection = normal.Dot(ray.direction);

	if (fabsf(dotNormalDirection) > kMaxDifference) { // Raio paralelo ao plano (perpendicular à normal)
		currentDistance = -(normal.Dot(ray.origin) - normal.Dot(vertices[0])) / dotNormalDirection;

		// Plano atrás do raio
		if (currentDistance < 0) {
			return intersection;
		}

		if (distance >= 0){
			if (currentDistance > distance){
				return intersection;
			}
		}


		vec3 intersectionPointPlane = ray.origin + ray.direction * currentDistance;
		vec2 Pi, P0, P1, P2;
		// Plano projectado em YoZ
		if (normal.x > normal.y && normal.x > normal.z){
			Pi.x = intersectionPointPlane.y;
			Pi.y = intersectionPointPlane.z;

			P0.x = vertices[0].y;
			P0.y = vertices[0].z;

			P1.x = vertices[1].y;
			P1.y = vertices[1].z;

			P2.x = vertices[2].y;
			P2.y = vertices[2].z;
		}

		if (normal.y > normal.x && normal.y > normal.z) {
			Pi.x = intersectionPointPlane.x;
			Pi.y = intersectionPointPlane.z;

			P0.x = vertices[0].x;
			P0.y = vertices[0].z;

			P1.x = vertices[1].x;
			P1.y = vertices[1].z;

			P2.x = vertices[2].x;
			P2.y = vertices[2].z;
		}

		if (normal.z > normal.x && normal.z > normal.y) {
			Pi.x = intersectionPointPlane.x;
			Pi.y = intersectionPointPlane.y;

			P0.x = vertices[0].x;
			P0.y = vertices[0].y;

			P1.x = vertices[1].x;
			P1.y = vertices[1].y;

			P2.x = vertices[2].x;
			P2.y = vertices[2].y;
		}

		float u0 = Pi.x - P0.x;
		float u1 = P1.x - P0.x;
		float u2 = P2.x - P0.x;
		float v0 = Pi.y - P0.y;
		float v1 = P1.y - P0.y;
		float v2 = P2.y - P0.y;

		// Column major
		//M = |0  2|
		//    |1  3|
		float beta = (u0 * v2 - u2 * v0) / (u1 * v2 - u2 * v1);
		float gama = (u1 * v0 - u0 * v1) / (u1 * v2 - u2 * v1);

		if (beta >= 0.0f && gama >= 0.0f && (beta + gama) <= 1.0f) {
			distance = currentDistance;
			intersection = true;

			intersectionPoint = ray.origin + ray.direction * currentDistance;
			normalIntersection = -normal;
		}
	}

	return intersection;
}
