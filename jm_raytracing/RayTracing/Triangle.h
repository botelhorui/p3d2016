#ifndef _TRIANGLE_H_
#define _TRIANGLE_H_

#include <vector>
#include "vectors.h"
#include "Object.h"

using namespace JMVector;

class Triangle : public Object {
	public:
		std::vector<vec3> vertices;

		Triangle();
		~Triangle();
		void CalculateNormal();
		bool Triangle::CalculateIntersection(Ray ray, float &distance, vec3 &intersectionPoint, vec3 &normalIntersection);
	private:
		vec3 normal;
};

#endif