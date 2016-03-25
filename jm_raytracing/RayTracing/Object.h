#ifndef _OBJECT_H_
#define _OBJECT_H_

#include "Material.h"
#include "Ray.h"

enum ObjectType {
	kObjectPlane, kObjectSphere, kObjectTriangle
};

class Object{
	public:
		int type;
		Material material;

		Object();
		~Object();
		virtual bool CalculateIntersection(Ray ray, float &distance, vec3 &intersectionPoint, vec3 &normalIntersection);
		//virtual vec3 CalculateNormal(const vec3 &intersectionPoint) = 0;
};

#endif