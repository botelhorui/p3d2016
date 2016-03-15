#include "Scene.h"
#include <fstream>
#include <string>
#include <sstream>

// Camera
int Camera::GetResX() {
	return ResX;
}

int Camera::GetResY() {
	return ResY;
}

// Scene
Scene::Scene() {
	camera = new Camera();
	colorBackground = vec3();
}

Scene::~Scene(){

}

Camera* Scene::GetCamera() {
	return camera;
}

vec3 Scene::GetColorBackground() {
	return colorBackground;
}

void CalculateCameraParameters(Camera *camera) {
	float length;
	vec3 result;
	result = camera->eye - camera->at;
	length = result.Length();

	camera->h = 2.0f * length * tanf(camera->fovy/2.0f);
	camera->w = camera->h * camera->ResX/camera->ResY;

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
}

bool Scene::LoadSceneNFF(std::string sceneName) {
	bool cameraInfo = false;
	std::ifstream input(sceneName);
	std::string info;
	std::string type;
	std::istringstream stream;
	Material material;

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
			stream >> colorBackground.x;
			stream >> colorBackground.y;
			stream >> colorBackground.z;
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
			stream >> light.color.x;
			stream >> light.color.y;
			stream >> light.color.z;
			lights.push_back(light);
			continue;
		}
		else if (type == "f") {
			stream >> material.red;
			stream >> material.green;
			stream >> material.blue;
			stream >> material.kd;
			stream >> material.ks;
			stream >> material.shine;
			stream >> material.t;
			stream >> material.ior;
			continue;
		}
		else if (type == "pl") {
			Object object;
			object.type = kObjectPlane;
			object.material = material;

			vec3 vertex;
			stream >> vertex.x;
			stream >> vertex.y;
			stream >> vertex.z;
			object.vertices.push_back(vertex);
			stream >> vertex.x;
			stream >> vertex.y;
			stream >> vertex.z;
			object.vertices.push_back(vertex);
			stream >> vertex.x;
			stream >> vertex.y;
			stream >> vertex.z;
			object.vertices.push_back(vertex);

			objects.push_back(object);
			continue;
		}
		else if (type == "s") {
			Object object;
			object.type = kObjectSphere;
			object.material = material;

			stream >> object.center.x;
			stream >> object.center.y;
			stream >> object.center.z;
			stream >> object.radius;
			objects.push_back(object);
			continue;
		}
	}

	return true;
}
