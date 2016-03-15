#include <string>
#include <vector>
#include "vectors.h"

using namespace JMVector;

#undef near
#undef far

typedef struct Camera {
	/* Camera definition*/
	vec3 eye, at, up;
	float fovy;
	float near, far; //hither and yon planes
	int ResX,ResY;
	float w, h;
	vec3 xe, ye, ze; //uvn frame

	public:
		int GetResX();
		int GetResY();
} Camera;

typedef struct Light {
	vec3 center;
	vec3 color;
}Light;

typedef struct Material {
	float red;
	float green;
	float blue;
	float kd;
	float ks;
	float shine;
	float t;
	float ior;
}Material;

enum ObjectType{
	kObjectCone, kObjectPlane, kObjectPolygon, kObjectPolygonPatch, kObjectSphere
};

typedef struct Object {
	int type;
	vec3 center;
	Material material;
	float radius;
	std::vector<vec3> vertices;
}Object;

class Scene{
	public:
		std::vector<Light> lights;
		std::vector<Object> objects;

		Scene();
		~Scene();

		Camera* GetCamera();
		vec3 GetColorBackground();
		bool LoadSceneNFF(std::string sceneName);

private:
		Camera* camera;
		vec3 colorBackground;
};
