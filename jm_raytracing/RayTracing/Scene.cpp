#include "Scene.h"
#include <fstream>
#include <string>
#include <sstream>
#include <sys\types.h>

#include "Constants.h"
#include "Plane.h"
#include "Sphere.h"

// Scene
Scene::Scene() {
	camera = new Camera();
	colorBackground = Color();
}

Scene::~Scene(){

}

Camera* Scene::GetCamera() {
	return camera;
}

Color Scene::GetColorBackground() {
	return colorBackground;
}

void CalculateCameraParameters(Camera *camera) {
	float length;
	vec3 result;
	result = camera->eye - camera->at;
	length = result.Length();

	camera->h = 2.0f * length * tanf((float)DEGREES_TO_RADIANS * camera->fovy/2.0f);
	camera->w = camera->h * camera->ResX/camera->ResY;
/**/
	camera->ze = (1.0f / length) * result;
	camera->ze = camera->ze.Normalized();

	result = camera->up.Cross(camera->ze);
	length = result.Length();
	camera->xe = (1.0f / length) * result;
	camera->xe = camera->xe.Normalized();

	result = camera->ze.Cross(camera->xe);
	//length = result.Length();
	camera->ye = result;
	camera->ye = camera->ye.Normalized();
/**/
}

bool Scene::LoadSceneNFF(std::string sceneName) {
	bool cameraInfo = false;
	std::ifstream input(sceneName);
	std::string info;
	std::string type;
	std::istringstream stream;
	Material material;

	struct stat status;

	if (stat(sceneName.c_str(), &status) == -1){
		return false;
	}

	while (std::getline(input, info)){
		//std::cout << "Line: " << info << "\n";

		//std::istringstream stream(info);
		stream = std::istringstream(info);
		stream >> type;
		//std::cout << "Type: " << type << "\n\n";

		if (cameraInfo){
			if (type == "from"){
				stream >> camera->eye.x;
				stream >> camera->eye.y;
				stream >> camera->eye.z;
				continue;
			}
			else if (type == "at") {
				stream >> camera->at.x;
				stream >> camera->at.y;
				stream >> camera->at.z;
				continue;
			}
			else if (type == "up") {
				stream >> camera->up.x;
				stream >> camera->up.y;
				stream >> camera->up.z;
				continue;
			}
			else if (type == "angle") {
				stream >> camera->fovy;
				continue;
			}
			else if (type == "hither") {
				stream >> camera->near;
				continue;
			}
			else if (type == "resolution") {
				stream >> camera->ResX;
				stream >> camera->ResY;
				CalculateCameraParameters(camera);
				cameraInfo = false;
				continue;
			}
		}

		if (type == "b"){
			stream >> colorBackground.r;
			stream >> colorBackground.g;
			stream >> colorBackground.b;
		}
		else if (type == "v"){
			cameraInfo = true;
			continue;
		}
		else if (type == "l") {
			Light light;
			stream >> light.center.x;
			stream >> light.center.y;
			stream >> light.center.z;
			stream >> light.color.r;
			stream >> light.color.g;
			stream >> light.color.b;
			lights.push_back(light);
			continue;
		}
		else if (type == "f") {
			stream >> material.color.r;
			stream >> material.color.g;
			stream >> material.color.b;
			stream >> material.kd;
			stream >> material.ks;
			stream >> material.shine;
			stream >> material.t;
			stream >> material.ior;
			continue;
		}
		else if (type == "pl") {
			Plane *plane = new Plane();
			plane->type = kObjectPlane;
			plane->material = material;

			vec3 vertex;
			stream >> vertex.x;
			stream >> vertex.y;
			stream >> vertex.z;
			plane->vertices.push_back(vertex);
			stream >> vertex.x;
			stream >> vertex.y;
			stream >> vertex.z;
			plane->vertices.push_back(vertex);
			stream >> vertex.x;
			stream >> vertex.y;
			stream >> vertex.z;
			plane->vertices.push_back(vertex);
			plane->CalculateNormal();

			objects.push_back(plane);
			continue;
		}
		else if (type == "s") {
			Sphere *sphere = new Sphere();
			sphere->type = kObjectSphere;
			sphere->material = material;

			stream >> sphere->center.x;
			stream >> sphere->center.y;
			stream >> sphere->center.z;
			float radius;
			stream >> radius;
			sphere->SetRadius(radius);
			objects.push_back(sphere);
			continue;
		}
	}

	return true;
}
