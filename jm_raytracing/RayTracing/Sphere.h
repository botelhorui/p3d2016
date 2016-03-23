#ifndef _SPHERE_H_
#define _SPHERE_H_

#include "Object.h"

class Sphere : public Object{
	public:
		vec3 center;

		Sphere();
		~Sphere();
		// Base class methods
		virtual bool CalculateIntersection(Ray ray, float &distance, vec3 &intersectionPoint, vec3 &normalIntersection);
		//virtual vec3 CalculateNormal(const vec3 &intersectionPoint);
		// Own methods
		void SetRadius(float r);
		float GetRadius();

	private:
		float radius;
		float squareRadius;
};

#endif