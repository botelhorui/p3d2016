#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "vectors.h"

#undef near
#undef far

using namespace JMVector;

class Camera{
	public:
		/* Camera definition*/
		vec3 eye, at, up;
		float fovy;
		float near, far; //hither and yon planes
		int ResX, ResY;
		float w, h;
		vec3 xe, ye, ze; //uvn frame

		Camera();
		~Camera();

		int GetResX();
		int GetResY();
};

#endif