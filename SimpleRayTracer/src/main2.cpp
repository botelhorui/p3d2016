/*
LodePNG Examples

Copyright (c) 2005-2012 Lode Vandevenne

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
claim that you wrote the original software. If you use this software
in a product, an acknowledgment in the product documentation would be
appreciated but is not required.

2. Altered source versions must be plainly marked as such, and must not be
misrepresented as being the original software.

3. This notice may not be removed or altered from any source
distribution.
*/

#include "lodepng.h"

#include <stdio.h>
#include <sstream>
#include <stdlib.h>
#include <raytracer.h>
#include <omp.h>
#include <ctime>
#include <algorithm>

DrawMode DRAW_MODE = GRID;

std::string scene_files[] = {
	"balls_low.nff",		 //0
	"balls_medium.nff",			//1
	"balls_high.nff",		//2
	"mount_low.nff",		 //3
	"mount_high.nff",		//4
	"mount_very_high.nff",	//5
	"one_ball.nff",	//6
	"three_balls.nff",		//7
	"three_balls_small.nff",		//8
};
int SCENE_FILE = 7;
std::string folder_path = "scenes/";
std::string scene_file = scene_files[SCENE_FILE];
std::string scene_file_path = folder_path + scene_file;


// Ray tracing
int MAX_DEPTH = 6;

// Monte Carlo
int MAX_DIVISIONS = 2;
double MONTE_CARLO_THRESHOLD = 0.3;

// Depth of field
int DEPTH_OF_FIELD_SAMPLES = 5;

// Soft shadows
const bool SOFT_SHADOWS_ON = true;
const soft_shadow_type SOFT_SHADOW_TYPE = SPHERE;
const int SOFT_SHADOWS_SAMPLES = 32;

// Area Soft Shadow
const vec3 LIGHT_AREA_U(1, 0, 0);
const vec3 LIGHT_AREA_V(0, 1, 0);
const double LIGHT_AREA_SIDE = 0;

// Sphere Soft Shadow
double LIGHT_RADIUS = 0.25;

//GRID acceleration
int GRID_WIDTH_MULTIPLIER = 2;

double INTERSECTION_TESTS_COUNT=0;

// Get current date/time, format is YYYY-MM-DD-HH-mm-ss
const std::string currentDateTime() {
	time_t     now = time(0);
	struct tm  tstruct;
	char       buf[80];
	//tstruct = *localtime(&now);
	localtime_s(&tstruct, &now);
	// Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
	// for more information about date/time format
	strftime(buf, sizeof(buf), "%Y-%m-%d-%H-%M-%S", &tstruct);

	return buf;
}

int main(int argc, char *argv[])
{
	Scene scene;
	if (scene.load_nff(scene_file_path))
	{
		printf("failed to load nff\n");
		system("pause");
		return 1;
	}
	std::stringstream ss;
	ss << "output/" << scene_file << "." << currentDateTime() << ".png";
	std::string output_file = ss.str();
	//const char* filename = sfn.c_str();
	const char* filename = output_file.c_str();

	/*generate some image*/
	unsigned width = scene.camera.res_x, height = scene.camera.res_y;
	unsigned char* image = (unsigned char*)malloc(width * height * 4);
	double start = omp_get_wtime();
//#pragma omp parallel
	{		
		int i, j;
//#pragma omp master
		printf("num threads %d\n", omp_get_num_threads());
//#pragma omp for schedule(dynamic)
		for (j = 0; j < height; j++)
			for (i = 0; i < width; i++)
			{
				if(i == 277 && j == 205)
				{
					//printf("");
				}
				int y = height - j;
				int x = i;
				vec3 color;
				if (DRAW_MODE == NORMAL || DRAW_MODE == GRID) {
					color = scene.ray_trace(x,y);
				}
				else if (DRAW_MODE == MONTE_CARLO) {
					color = scene.ray_trace_monte_carlo(x, y);
				}
				else if (DRAW_MODE == DOF) {
					color = scene.ray_trace_dof(x, y);
				}
				else if (DRAW_MODE == DOF_MONTE_CARLO) {
					color = scene.ray_trace_monte_carlo_dof(x, y);
				}				
				//R G B A
				int k = 4 * width * j + 4 * x;
				
				image[k + 0] = std::min(1.0, color.x) * 255; 
				image[k + 1] = std::min(1.0, color.y) * 255;
				image[k + 2] = std::min(1.0, color.z) * 255;
				image[k + 3] = 255;
			}	
	}
	double end = omp_get_wtime() - start;
	
	/*Encode the image*/
	unsigned error = lodepng_encode32_file(filename, image, width, height);

	printf("filename '%s'\n", filename);
	/*if there's an error, display it*/
	if (error) printf("error %u: %s\n", error, lodepng_error_text(error));

	printf("Total time:%f\n", end);
	printf("Total intersection tests: %f\n", INTERSECTION_TESTS_COUNT);

	scene.free();
	free(image);

	//getchar();
	return 0;
}