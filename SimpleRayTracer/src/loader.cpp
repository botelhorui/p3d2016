#include "raytracer.h"
#include <fstream>
#include <string>

int Scene::load_nff(std::string path){
	std::ifstream f;
	f.open(path);
	Material lastMat;
	std::string type;
	while (!f.eof())
	{
		f >> type;
		if (type == "b")
		{
			f >> backgroundColor;
		}
		else if (type == "v")
		{
			f >> type >> camera.from;
			f >> type >> camera.at;
			f >> type >> camera.up;
			f >> type;
			if(type == "focal"){
				f >>  camera.focalDist;
				f >> type >> camera.viewDist;
				f >> type >> camera.apperture;
				f >> type; // read 'angle'
			} // else angle
			f >> camera.angle;
			f >> type >> camera.hither;
			f >> type >> camera.res_x >> camera.res_y;
		}
		else if (type == "l")
		{
			vec3 pos, a,b,color;
			f >> pos >> color;
			// light's area_x and area_y are not initialized since we use default values
			lights.push_back(Light(pos, a, b, color));
		}
		else if (type == "f")
		{
			f >> lastMat.color >> lastMat.Kd >> lastMat.Ks >> lastMat.Shine >> lastMat.T >> lastMat.ior;
		}
		else if (type == "pl")
		{
			vec3 v0, v1, v2;
			f >> v0 >> v1 >> v2;
			planes.push_back(Plane(v0, v1, v2, lastMat));
		}
		else if (type == "s")
		{
			vec3 pos;
			double radius;
			f >> pos >> radius;
			spheres.push_back(Sphere(pos, radius, lastMat));
		}
		else if (type == "p")
		{
			int numVertices;
			f >> numVertices;
			vec3 v0, v1, v2;
			f >> v0 >> v1 >> v2;
			triangles.push_back(Triangle(v0, v1, v2, lastMat));
		}
		else
		{
			printf("load_nff: Unrecognizable type'%s' ", type.c_str());
			std::cin.get();
			exit(1);
		}
	}

	printf("number of lights %d\n", lights.size());
	printf("number of planes %d\n", planes.size());
	printf("number of spheres %d\n", spheres.size());
	printf("number of triangles %d\n", triangles.size());

	int objectsCount = 0;	

	for each (Plane plane in planes){
		grid.addPlane(plane);
		objectsCount++;
	}

	for each (Sphere sphere in spheres) {
		grid.addSphere(sphere);
		objectsCount++;
	}

	for each (Triangle triangle in triangles) {
		grid.addTriangle(triangle);
		objectsCount++;
	}

	grid.initializeGrid(objectsCount);

	return 0;
}

