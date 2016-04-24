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
			Plane* obj = new Plane(v0, v1, v2, lastMat);	
			// Planes are not added to scene.objects because they are a particular case
			planes.push_back(obj);
		}
		else if (type == "s")
		{
			vec3 pos;
			double radius;
			f >> pos >> radius;			
			Sphere* obj = new Sphere(pos, radius, lastMat);
			objects.push_back(obj);
			spheres.push_back(obj);
		}
		else if (type == "p")
		{
			int numVertices;
			f >> numVertices;
			vec3 v0, v1, v2;
			f >> v0 >> v1 >> v2;
			Triangle* obj = new Triangle(v0, v1, v2, lastMat);
			objects.push_back(obj);
			triangles.push_back(obj);
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

	// TODO
	if(DRAW_MODE == GRID)
	{
		grid.scene = this;
		grid.initializeGrid();
	}		
	return 0;
}

