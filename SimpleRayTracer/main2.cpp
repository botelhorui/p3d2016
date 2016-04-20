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
#include <stdlib.h>
#include <raytracer.h>
#include <omp.h>


int main(int argc, char *argv[])
{
	Scene scene;
	if (scene.load_nff("scenes/test1.nff"))
	{
		printf("failed to load nff\n");
		system("pause");
		return 1;
	}
	const char* filename = argc > 1 ? argv[1] : "test1.png";

	/*generate some image*/
	unsigned width = scene.camera.res_x, height = scene.camera.res_y;
	unsigned char* image = (unsigned char*)malloc(width * height * 4);
	double start = omp_get_wtime();
#pragma omp parallel
	{		
		int x, y;
#pragma omp master
		printf("num threads %d\n", omp_get_num_threads());
#pragma omp for schedule(dynamic)
		for (y = 0; y < height; y++)
			for (x = 0; x < width; x++)
			{
				Ray ray = scene.calculate_primary_ray(x, height - y);
				ray.depth = MAX_DEPTH;
				ray.ior = 1.0f;
				vec3 color = scene.ray_trace(ray);
				//RGBA
				image[4 * width * y + 4 * x + 0] = color.x * 255; 
				image[4 * width * y + 4 * x + 1] = color.y * 255;
				image[4 * width * y + 4 * x + 2] = color.z * 255;
				image[4 * width * y + 4 * x + 3] = 255;
			}	
	}

	
	/*Encode the image*/
	unsigned error = lodepng_encode32_file(filename, image, width, height);

	/*if there's an error, display it*/
	if (error) printf("error %u: %s\n", error, lodepng_error_text(error));

	printf("Terminou! em %f\n", omp_get_wtime() - start);

	free(image);

	getchar();
	return 0;
}