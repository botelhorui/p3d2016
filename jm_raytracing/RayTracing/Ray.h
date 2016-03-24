#ifndef _RAY_H_
#define _RAY_H_

#include "vectors.h"

using namespace JMVector;

class Ray{
	public:
		vec3 direction;
		vec3 origin;

		Ray();
		~Ray();

private:
};

#endif