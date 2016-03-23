#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include "vectors.h"
#include "Color.h"

using namespace JMVector;

class Material{
	public:
		Color color;
		float kd;
		float ks;
		float shine;
		float t;
		float ior;

		Material();
		~Material();
};

#endif