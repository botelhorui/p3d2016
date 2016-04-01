#include "raytracer.h"
#include <fstream>
#include <string>

int Scene::load_nff(std::string path)
{
	std::ifstream file;
	file.open(path);
	Material lastMat;
	std::string type;
	while (!file.eof())
	{
		file >> type;
		if (type == "b")
		{
			file >> backgroundColor;
		}
		else if (type == "v")
		{
			file >> type >> camera.from;
			file >> type >> camera.at;
			file >> type >> camera.up;
			file >> type >> camera.angle;
			file >> type >> camera.hither;
			file >> type >> camera.res_x >> camera.res_y;
		}
		else if (type == "l")
		{
			vec3 pos, color;
			file >> pos >> color;
			lights.push_back(Light(pos, color));
		}
		else if (type == "f")
		{
			file >> lastMat.color >> lastMat.Kd >> lastMat.Ks >>
				lastMat.Shine >> lastMat.T >> lastMat.ior;
		}
		else if (type == "pl")
		{
			vec3 v0, v1, v2;
			file >> v0 >> v1 >> v2;
			objects.push_back(new Plane(v0, v1, v2, lastMat));
		}
		else if (type == "s")
		{
			vec3 pos;
			float radius;
			file >> pos >> radius;
			objects.push_back(new Sphere(pos, radius, lastMat));
		}
		else if (type == "p")
		{

		}
		else
		{
			printf("load_nff: Unrecognizable type'%s' ", type.c_str());
			std::cin.get();
			exit(1);
		}
	}
	printf("number of lights %d\n", lights.size());
	return 0;
}
