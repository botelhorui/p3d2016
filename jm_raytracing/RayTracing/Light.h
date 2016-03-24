#ifndef _LIGHT_H_
#define _LIGHT_H_

#include "vectors.h"
#include "Color.h"

using namespace JMVector;

class Light{
	public:
		vec3 center;
		Color color;

		Light();
		~Light();
};

#endif