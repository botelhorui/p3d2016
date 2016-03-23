#ifndef _SCENE_H_
#define _SCENE_H_

#include <string>
#include <vector>
#include "vectors.h"
#include "Camera.h"
#include "Light.h"
#include "Object.h"

using namespace JMVector;

class Scene{
	public:
		std::vector<Light> lights;
		std::vector<Object*> objects;

		Scene();
		~Scene();

		Camera* GetCamera();
		Color GetColorBackground();
		bool LoadSceneNFF(std::string sceneName);

private:
		Camera* camera;
		Color colorBackground;
};

#endif