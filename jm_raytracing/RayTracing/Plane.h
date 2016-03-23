#ifndef _PLANE_H_
#define _PLANE_H_

#include <vector>
#include "Object.h"

class Plane : public Object{
	public:
		std::vector<vec3> vertices;

		Plane();
		~Plane();
		// Base class methods
		virtual bool CalculateIntersection(Ray ray, float &distance, vec3 &intersectionPoint, vec3 &normalIntersection);
		//virtual vec3 CalculateNormal(const vec3 &intersectionPoint);
		// Own methods
		void CalculateNormal();
	private:
		vec3 normal;
};

#endif