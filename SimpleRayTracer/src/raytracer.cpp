#include "raytracer.h"
#include <fstream>
#include <string>
#define _USE_MATH_DEFINES
#include <math.h>
#include <algorithm>

// TODO merge reflect refract and rSchlick2
vec3 reflect(const vec3& normal, const vec3& incident)
{
	const double cosI = -dot(normal, incident);
	return incident + 2 * cosI*normal;
}
// TODO merge reflect refract and rSchlick2
bool refract(const vec3 normal, const vec3& incident, double n1, double n2, vec3& reflect_dir)
{
	const double n = n1 / n2;
	const double cosI = -dot(normal, incident);
	const double sinT2 = n * n * (1.0 - cosI*cosI);
	if (sinT2 > 1.0) return false; // TIR
	const double cosT = sqrt(1.0 - sinT2);
	reflect_dir = n * incident + (n*cosI - cosT) * normal;
	reflect_dir = normalize(reflect_dir);
	return true;
}
// TODO merge reflect refract and rSchlick2
double rSchlick2(const vec3& normal, const vec3& incident, double n1, double n2)
{
	double r0 = (n1 - n2) / (n1 + n2);
	r0 *= r0;
	double cosX = -dot(normal, incident);
	if(n1 > n2)
	{
		const double n = n1 / n2;
		const double sinT2 = n*n*(1.0 - cosX*cosX);
		if (sinT2 > 1.0) return 1.0; //TIR
		cosX = sqrt(1.0 - sinT2);
	}
	const double x = 1.0 - cosX;
	return r0 + (1.0 - r0) * x * x * x * x * x;
}


Ray Scene::calculate_primary_ray(int x, int y)
{
	// from "2 - Ray_Tracing_Practice.pdf"
	// page 20
	Ray r;
	double df; //distance to frame
	double h; // frame height
	double w; // frame width
	vec3 xe, ye, ze; // camera space
	ze = camera.from - camera.at;
	df = ze.length;
	ze = normalize(ze);
	double angle; // half of the field of view
	angle = (M_PI / 180.0) * (camera.angle / 2.0);
	h = 2 * df * tan(angle);
	w = (camera.res_x * h) / camera.res_y; // height * ratio
	xe = normalize(cross(camera.up, ze));
	ye = normalize(cross(ze, xe));
	r.origin = camera.from;
	r.dir = (w * ((1.0 * x) / camera.res_x - .5)) * xe +
		(h * ((1.0 * y) / camera.res_y - .5)) * ye +
		(-df) * ze;
	r.dir = normalize(r.dir);
	return r;
}

// Monte Carlo begins
Ray Scene::calculate_primary_ray_monte_carlo(float x, float y, float deltaX, float deltaY){
	Ray r;
	double df; //distance to frame
	double h; // frame height
	double w; // frame width
	vec3 xe, ye, ze; // camera space
	ze = camera.from - camera.at;
	df = ze.length;
	ze = normalize(ze);
	double angle; // half of the field of view
	angle = (M_PI / 180.0) * (camera.angle / 2.0);
	h = 2 * df * tan(angle);
	w = (camera.res_x * h) / camera.res_y; // height * ratio
	xe = normalize(cross(camera.up, ze));
	ye = normalize(cross(ze, xe));
	r.origin = camera.from;

	r.dir = (w * (((float)x + deltaX) / camera.res_x - 0.5)) * xe +
		(h * (((float)y + deltaY) / camera.res_y - 0.5)) * ye +
		(-df) * ze;

	r.dir = normalize(r.dir);

	return r;
}

vec3 Scene::calculateSecondaryRaysMonteCarlo(int divisions, float x, float y, vec3 colorPixelsPrevious[4][4], int index) {
	Ray ray;
	vec3 color;
	float thresholdCompare = MONTE_CARLO_THRESHOLD;
	float divisionFactor = 0.5f / (powf(2.0f, divisions));
	//float divisionFactorCorner = 1.0f / ((float)divisions * 2.0);
	bool dividePixel = false;
	vec3 colorPixels[4][4];

	// Compares each calculated ray in order to check if division is needed
	for (int i = 0; i < 3; i++) {
		for (int j = i + 1; j < 4; j++) {
			// If the difference between the sum of the RGB components of any two rays is greater than threshold, division of the pixel is needed
			if ((fabsf(colorPixelsPrevious[index][i].x - colorPixelsPrevious[index][j].x) + fabsf(colorPixelsPrevious[index][i].y - colorPixelsPrevious[index][j].y) + fabsf(colorPixelsPrevious[index][i].z - colorPixelsPrevious[index][j].z)) > thresholdCompare) {
				dividePixel = true;
				break;
			}
		}
	}

	if (!dividePixel || divisions == MAX_DIVISIONS) {
		{ // Calculate the average color of the pixel, based on the colors of each ray
			for (int i = 0; i < 4; i++) {
				color += colorPixelsPrevious[index][i];
			}

			color.x /= 4.0f;
			color.y /= 4.0f;
			color.z /= 4.0f;
		}

		return color;
	}

	for (int i = 0; i < 4; i++) {
		colorPixels[i][0] = colorPixelsPrevious[index][i];
	}

	// Sub-pixels
	// 2 3
	// 0 1
	// Rays in pixel
	// 7 8 9
	// 4 5 6
	// 1 2 3
	// Calculates the central ray (5) for the current pixel, which is shared with every sub-pixel
	ray = calculate_primary_ray_monte_carlo(x, y, 0.0f, 0.0f);
	ray.depth = MAX_DEPTH;
	ray.ior = 1.0f;
	colorPixels[0][1] = colorPixels[1][1] = colorPixels[2][1] = colorPixels[3][1] = ray_trace(ray);

	// Calculates the bottom center pixel (2), which is shared between the bottom left and the bottom-right sub-pixels
	ray = calculate_primary_ray_monte_carlo(x, y, 0.0f, -divisionFactor);
	ray.depth = MAX_DEPTH;
	ray.ior = 1.0f;
	colorPixels[0][2] = colorPixels[1][2] = ray_trace(ray);

	// Calculates the top center pixel (8), which is shared between the top left and the top right sub-pixels
	ray = calculate_primary_ray_monte_carlo(x, y, 0, divisionFactor);
	ray.depth = MAX_DEPTH;
	ray.ior = 1.0f;
	colorPixels[2][2] = colorPixels[3][2] = ray_trace(ray);

	// Calculates the left center pixel (4), which is shared between the bottom left and the top left sub-pixels
	ray = calculate_primary_ray_monte_carlo(x, y, -divisionFactor, 0.0f);
	ray.depth = MAX_DEPTH;
	ray.ior = 1.0f;
	colorPixels[0][3] = colorPixels[2][3] = ray_trace(ray);

	// Calculates the right center pixel (6), which is shared between the bottom right and the top right sub-pixels
	ray = calculate_primary_ray_monte_carlo(x, y, divisionFactor, 0.0f);
	ray.depth = MAX_DEPTH;
	ray.ior = 1.0f;
	colorPixels[1][3] = colorPixels[3][3] = ray_trace(ray);

	divisions++;

	// Sub-pixels
	// 2 3
	// 0 1
	color += calculateSecondaryRaysMonteCarlo(divisions, x - divisionFactor, y - divisionFactor, colorPixels, 0);
	color += calculateSecondaryRaysMonteCarlo(divisions, x + divisionFactor, y - divisionFactor, colorPixels, 1);
	color += calculateSecondaryRaysMonteCarlo(divisions, x - divisionFactor, y + divisionFactor, colorPixels, 2);
	color += calculateSecondaryRaysMonteCarlo(divisions, x + divisionFactor, y + divisionFactor, colorPixels, 3);

	color.x /= 4.0f;
	color.y /= 4.0f;
	color.z /= 4.0f;

	return color;
}

vec3 Scene::calculatePrimaryRaysMonteCarlo(int divisions, float x, float y) {
	Ray ray;
	vec3 color;
	float thresholdCompare = MONTE_CARLO_THRESHOLD;
	vec3 colorPixels[4][4]; // The first index is the sub-pixel, and the second index is the ray color
	bool dividePixel = false;

	// Sub-pixels
	// 2 3
	// 0 1
	// Rays in pixel
	// 7 8 9
	// 4 5 6
	// 1 2 3
	// Calculate the 4 primary rays (1, 3, 7, 9) for the main pixel (each ray for each corner)
	for (int pY = 0; pY < 2; pY++) {
		for (int pX = 0; pX < 2; pX++) {
			ray = calculate_primary_ray_monte_carlo(x, y, (float)pX, (float)pY);
			ray.depth = MAX_DEPTH;
			ray.ior = 1.0f;
			colorPixels[pY * 2 + pX][0] = ray_trace(ray);
		}
	}

	// Compares each calculated ray in order to check if division is needed
	for (int i = 0; i < 3; i++) {
		for (int j = i + 1; j < 4; j++) {
			// If the difference between the sum of the RGB components of any two rays is greater than threshold, division of the pixel is needed
			if ((fabsf(colorPixels[i][0].x - colorPixels[j][0].x) + fabsf(colorPixels[i][0].y - colorPixels[j][0].y) + fabsf(colorPixels[i][0].z - colorPixels[j][0].z)) > thresholdCompare) {
				dividePixel = true;
				break;
			}
		}
	}

	// The final color is calculated
	if (!dividePixel || divisions == MAX_DIVISIONS) {
		{ // Calculate the average color of the pixel, based on the colors of each ray
			for (int i = 0; i < 4; i++) {
				color += colorPixels[i][0];
			}

			color.x /= 4.0f;
			color.y /= 4.0f;
			color.z /= 4.0f;
		}

		return color;
	}

	// Sub-pixels
	// 2 3
	// 0 1
	// Rays in pixel
	// 7 8 9
	// 4 5 6
	// 1 2 3
	// Calculates the central ray (5) for the current pixel, which is shared with every sub-pixel
	ray = calculate_primary_ray_monte_carlo(x, y, 0.5f, 0.5f);
	ray.depth = MAX_DEPTH;
	ray.ior = 1.0f;
	colorPixels[0][1] = colorPixels[1][1] = colorPixels[2][1] = colorPixels[3][1] = ray_trace(ray);

	// Calculates the bottom center pixel (2), which is shared between the bottom left and the bottom-right sub-pixels
	ray = calculate_primary_ray_monte_carlo(x, y, 0.5f, 0.0f);
	ray.depth = MAX_DEPTH;
	ray.ior = 1.0f;
	colorPixels[0][2] = colorPixels[1][2] = ray_trace(ray);

	// Calculates the top center pixel (8), which is shared between the top left and the top right sub-pixels
	ray = calculate_primary_ray_monte_carlo(x, y, 0.5f, 1.0f);
	ray.depth = MAX_DEPTH;
	ray.ior = 1.0f;
	colorPixels[2][2] = colorPixels[3][2] = ray_trace(ray);

	// Calculates the left center pixel (4), which is shared between the bottom left and the top left sub-pixels
	ray = calculate_primary_ray_monte_carlo(x, y, 0.0f, 0.5f);
	ray.depth = MAX_DEPTH;
	ray.ior = 1.0f;
	colorPixels[0][3] = colorPixels[2][3] = ray_trace(ray);

	// Calculates the right center pixel (6), which is shared between the bottom right and the top right sub-pixels
	ray = calculate_primary_ray_monte_carlo(x, y, 1.0f, 0.5f);
	ray.depth = MAX_DEPTH;
	ray.ior = 1.0f;
	colorPixels[1][3] = colorPixels[3][3] = ray_trace(ray);

	divisions++;

	// Sub-pixels
	// 2 3
	// 0 1
	color += calculateSecondaryRaysMonteCarlo(divisions, x + 0.25, y + 0.25f, colorPixels, 0);
	color += calculateSecondaryRaysMonteCarlo(divisions, x + 0.75, y + 0.25f, colorPixels, 1);
	color += calculateSecondaryRaysMonteCarlo(divisions, x + 0.25, y + 0.75f, colorPixels, 2);
	color += calculateSecondaryRaysMonteCarlo(divisions, x + 0.75, y + 0.75f, colorPixels, 3);

	color.x /= 4.0f;
	color.y /= 4.0f;
	color.z /= 4.0f;

	return color;
}

vec3 Scene::ray_trace_monte_carlo(int x, int y) {
	return calculatePrimaryRaysMonteCarlo(0, (float)x, (float)y);
}
// Monte Carlo ends

vec3 Scene::calc_refract_color(Ray ray, Hit& hit)
{
	if(hit.mat.T <= 0.0)
	{
		return vec3();
	}
	float ior = hit.mat.ior;
	if(!hit.into_inside)
	{
		ior = 1.0;
	}
	vec3 refract_dir;
	if(refract(hit.normal, ray.dir, ray.ior, ior, refract_dir))
	{
		Ray refractRay(hit.pos, refract_dir, ray.depth - 1, ior);
		refractRay.offset(EPSILON);
		return ray_trace(refractRay);
	}
		// Total internal reflection
		return vec3();
	
	
}

vec3 Scene::calc_reflect_color(Ray ray, Hit& hit)
{
	
	if (hit.mat.Ks <= 0.0f)
	{
		return vec3(0, 0, 0);
	}
	vec3 reflectDir = reflect(hit.normal, ray.dir);
	Ray reflectRay(hit.pos, reflectDir, ray.depth - 1, ray.ior);
	reflectRay.offset(EPSILON);
	return ray_trace(reflectRay);
}

void Scene::calc_shadow_intersections(Ray& ray, Hit& hit)
{
	for (auto& obj : planes)
	{
		obj.calcIntersection(ray, hit);
		//if (hit.dist > 0) return;
	}
	for (auto& obj : spheres)
	{
		obj.calcIntersection(ray, hit);
		//if (hit.dist > 0) return;
	}
	for (auto& obj : triangles)
	{
		obj.calcIntersection(ray, hit);
		//if (hit.dist > 0) return;
	}
}

void Scene::calc_shadow_ray(Hit& hit, Light& light, Ray& ray_out)
{
	double deltaX = ((double)rand()/(double)RAND_MAX)*0.25;
	double deltaY = ((double)rand()/ (double)RAND_MAX)*0.25;
	ray_out.origin = hit.pos;
	vec3 light_delta = light.pos + light.a*deltaX + light.b*deltaY;
	ray_out.dir = normalize(light_delta - hit.pos);
}

vec3 Scene::ray_trace_dof(double x, double y){
	// from "2 - Ray_Tracing_Practice.pdf"
	// page 20
	double df; //distance to frame
	double h; // frame height
	double w; // frame width
	vec3 xe, ye, ze; // camera space
	double angle; // half of the field of view
	ze = camera.from - camera.at;
	ze = normalize(ze);
	//df = ze.length;
	df = camera.viewDist;
	xe = normalize(cross(camera.up, ze));
	ye = normalize(cross(ze, xe));
	angle = (M_PI / 180.0) * (camera.angle / 2.0);
	h = 2 * df * tan(angle);
	w = (camera.res_x * h) / camera.res_y; // height * ratio
	// ps is the view plane point
	double psx = (w * ((1.0 * x) / camera.res_x - .5));
	double psy = (h * ((1.0 * y) / camera.res_y - .5));
	double increase = camera.focalDist / df;
	double px = psx * increase;
	double py = psy * increase;
	double pz = -camera.focalDist;
	vec3 p = px*xe + py*ye + pz*ze; // focal point
	vec3 color;
	vec3 d = normalize(px*xe + py*ye + pz*ze);
	Ray r(camera.from, d, MAX_DEPTH, 1.0);

	for (int i = 0; i < DEPTH_OF_FIELD_SAMPLES; i++){
		//double sx = - 1.0f * camera.apperture + 2.0f * ((double)rand() / (double)RAND_MAX) * camera.apperture;
		//double sy = - 1.0f * camera.apperture + 2.0f * ((double)rand() / (double)RAND_MAX) * camera.apperture;
		double sx = ((double)rand() / (double)RAND_MAX) * camera.apperture;
		double sy = ((double)rand() / (double)RAND_MAX) * camera.apperture;
		vec3 origin = camera.from + sx * xe + sy * ye;
		double dirx = px - sx;
		double diry = py - sy;
		double dirz = -camera.focalDist;
		vec3 dir = dirx * xe + diry * ye + dirz * ze;
		dir = normalize(dir);
		Ray ray(origin, dir, MAX_DEPTH, 1.0);
		color += ray_trace(ray);
	}

	return color / DEPTH_OF_FIELD_SAMPLES;
}


vec3 Scene::calculateSecondaryRaysMonteCarlo_DoF(int divisions, float x, float y, vec3 colorPixelsPrevious[4][4], int index) {
	Ray ray;
	vec3 color;
	float thresholdCompare = MONTE_CARLO_THRESHOLD;
	float divisionFactor = (0.5f / (powf(2.0f, divisions)));
	//float divisionFactorCorner = 1.0f / ((float)divisions * 2.0);
	bool dividePixel = false;
	vec3 colorPixels[4][4];

	// Compares each calculated ray in order to check if division is needed
	for (int i = 0; i < 3; i++) {
		for (int j = i + 1; j < 4; j++) {
			// If the difference between the sum of the RGB components of any two rays is greater than threshold, division of the pixel is needed
			if ((fabsf(colorPixelsPrevious[index][i].x - colorPixelsPrevious[index][j].x) + fabsf(colorPixelsPrevious[index][i].y - colorPixelsPrevious[index][j].y) + fabsf(colorPixelsPrevious[index][i].z - colorPixelsPrevious[index][j].z)) > thresholdCompare) {
				dividePixel = true;
				break;
			}
		}
	}

	if (!dividePixel || divisions == MAX_DIVISIONS) {
		{ // Calculate the average color of the pixel, based on the colors of each ray
			for (int i = 0; i < 4; i++) {
				color += colorPixelsPrevious[index][i];
			}

			color.x /= 4.0f;
			color.y /= 4.0f;
			color.z /= 4.0f;
		}

		return color;
	}

	for (int i = 0; i < 4; i++) {
		colorPixels[i][0] = colorPixelsPrevious[index][i];
	}

	// Sub-pixels
	// 2 3
	// 0 1
	// Rays in pixel
	// 7 8 9
	// 4 5 6
	// 1 2 3
	// Calculates the central ray (5) for the current pixel, which is shared with every sub-pixel
	colorPixels[0][1] = colorPixels[1][1] = colorPixels[2][1] = colorPixels[3][1] = ray_trace_dof(x, y);

	// Calculates the bottom center pixel (2), which is shared between the bottom left and the bottom-right sub-pixels
	colorPixels[0][2] = colorPixels[1][2] = ray_trace_dof(x, y - divisionFactor);

	// Calculates the top center pixel (8), which is shared between the top left and the top right sub-pixels
	colorPixels[2][2] = colorPixels[3][2] = ray_trace_dof(x, y + divisionFactor);

	// Calculates the left center pixel (4), which is shared between the bottom left and the top left sub-pixels
	colorPixels[0][3] = colorPixels[2][3] = ray_trace_dof(x - divisionFactor, y);

	// Calculates the right center pixel (6), which is shared between the bottom right and the top right sub-pixels
	colorPixels[1][3] = colorPixels[3][3] = ray_trace_dof(x - divisionFactor, y);

	divisions++;

	// Sub-pixels
	// 2 3
	// 0 1
	color += calculateSecondaryRaysMonteCarlo_DoF(divisions, x - divisionFactor, y - divisionFactor, colorPixels, 0);
	color += calculateSecondaryRaysMonteCarlo_DoF(divisions, x + divisionFactor, y - divisionFactor, colorPixels, 1);
	color += calculateSecondaryRaysMonteCarlo_DoF(divisions, x - divisionFactor, y + divisionFactor, colorPixels, 2);
	color += calculateSecondaryRaysMonteCarlo_DoF(divisions, x + divisionFactor, y + divisionFactor, colorPixels, 3);

	color.x /= 4.0f;
	color.y /= 4.0f;
	color.z /= 4.0f;

	return color;
}

vec3 Scene::calculatePrimaryRaysMonteCarlo_DoF(int divisions, float x, float y) {
	Ray ray;
	vec3 color;
	float thresholdCompare = MONTE_CARLO_THRESHOLD;
	vec3 colorPixels[4][4]; // The first index is the sub-pixel, and the second index is the ray color
	bool dividePixel = false;

	// Sub-pixels
	// 2 3
	// 0 1
	// Rays in pixel
	// 7 8 9
	// 4 5 6
	// 1 2 3
	// Calculate the 4 primary rays (1, 3, 7, 9) for the main pixel (each ray for each corner)
	for (int pY = 0; pY < 2; pY++) {
		for (int pX = 0; pX < 2; pX++) {
			colorPixels[pY * 2 + pX][0] = ray_trace_dof(x + (float)pX, y + (float)pY);
		}
	}

	// Compares each calculated ray in order to check if division is needed
	for (int i = 0; i < 3; i++) {
		for (int j = i + 1; j < 4; j++) {
			// If the difference between the sum of the RGB components of any two rays is greater than threshold, division of the pixel is needed
			if ((fabsf(colorPixels[i][0].x - colorPixels[j][0].x) + fabsf(colorPixels[i][0].y - colorPixels[j][0].y) + fabsf(colorPixels[i][0].z - colorPixels[j][0].z)) > thresholdCompare) {
				dividePixel = true;
				break;
			}
		}
	}

	// The final color is calculated
	if (!dividePixel || divisions == MAX_DIVISIONS) {
		{ // Calculate the average color of the pixel, based on the colors of each ray
			for (int i = 0; i < 4; i++) {
				color += colorPixels[i][0];
			}

			color.x /= 4.0f;
			color.y /= 4.0f;
			color.z /= 4.0f;
		}

		return color;
	}

	// Sub-pixels
	// 2 3
	// 0 1
	// Rays in pixel
	// 7 8 9
	// 4 5 6
	// 1 2 3
	// Calculates the central ray (5) for the current pixel, which is shared with every sub-pixel
	colorPixels[0][1] = colorPixels[1][1] = colorPixels[2][1] = colorPixels[3][1] = ray_trace_dof(x + 0.5f, y + 0.5f);

	// Calculates the bottom center pixel (2), which is shared between the bottom left and the bottom-right sub-pixels
	colorPixels[0][2] = colorPixels[1][2] = ray_trace_dof(x +  0.5f, y);

	// Calculates the top center pixel (8), which is shared between the top left and the top right sub-pixels
	colorPixels[2][2] = colorPixels[3][2] = ray_trace_dof(x + 0.5f, y + 1.0f);

	// Calculates the left center pixel (4), which is shared between the bottom left and the top left sub-pixels
	colorPixels[0][3] = colorPixels[2][3] = ray_trace_dof(x, y + 0.5f);

	// Calculates the right center pixel (6), which is shared between the bottom right and the top right sub-pixels
	colorPixels[1][3] = colorPixels[3][3] = ray_trace_dof(x + 1.0f, y + 0.5f);

	divisions++;

	// Sub-pixels
	// 2 3
	// 0 1
	color += calculateSecondaryRaysMonteCarlo_DoF(divisions, x + 0.25, y + 0.25f, colorPixels, 0);
	color += calculateSecondaryRaysMonteCarlo_DoF(divisions, x + 0.75, y + 0.25f, colorPixels, 1);
	color += calculateSecondaryRaysMonteCarlo_DoF(divisions, x + 0.25, y + 0.75f, colorPixels, 2);
	color += calculateSecondaryRaysMonteCarlo_DoF(divisions, x + 0.75, y + 0.75f, colorPixels, 3);

	color.x /= 4.0f;
	color.y /= 4.0f;
	color.z /= 4.0f;

	return color;
}

vec3 Scene::ray_trace_monte_carlo_dof(int x, int y) {
	return calculatePrimaryRaysMonteCarlo_DoF(0, (float)x, (float)y);
}

vec3 Scene::calc_local_color(Ray& ray, Hit& hit){
	vec3 localColor(0, 0, 0);
	const double NUM_SAMPLES = 1 << 8;
	for (auto& light: lights)
	{
		vec3 c(0, 0, 0);
		for (int si = 0; si < NUM_SAMPLES; si++)
		{
			Hit shadow_hit;
			vec3 light_vec = light.pos - hit.pos;
			vec3 light_dir = normalize(light_vec);
			//Ray shadow_ray(hit.pos, light_dir, 0, 0.0);
			Ray shadow_ray;
			calc_shadow_ray(hit, light, shadow_ray);
			shadow_ray.offset(EPSILON);
			calc_shadow_intersections(shadow_ray, shadow_hit);
			if (shadow_hit.dist > 0 && shadow_hit.dist < light_vec.length)
			{
				continue;
			}
			vec3 diffuseColor, specularcolor;
			double lambertian = dot(light_dir, hit.normal);
			if (lambertian > 0.0f)
			{
				diffuseColor = hit.mat.color * hit.mat.Kd * lambertian;
				//specular
				vec3 viewDir = normalize(ray.origin - hit.pos);
				vec3 halfDir = normalize(light_dir + viewDir);
				double specAngle = dot(halfDir, hit.normal);
				double specular = pow(specAngle, hit.mat.Shine * 4.f);
				if (specular > 0.0f)
				{
					//specularcolor = hit.mat.color * hit.mat.Ks * specular * 0.6f;
					specularcolor = hit.mat.color * hit.mat.Ks * specular;;
				}
			}
			c += diffuseColor + specularcolor;
		}

		localColor += (1.0/NUM_SAMPLES)*c;
	}
	
	return localColor;
}

void Scene::calc_intersection(Ray& ray, Hit& hit){
	for (auto& obj : planes)
	{
		obj.calcIntersection(ray, hit);
	}
	for (auto& obj : spheres)
	{
		obj.calcIntersection(ray, hit);
	}
	for (auto& obj : triangles)
	{
		obj.calcIntersection(ray, hit);
	}
}

vec3 Scene::ray_trace(Ray ray)
{
	if (ray.depth == 0)
	{
		return vec3(0);
	}
	Hit hit;
	calc_intersection(ray, hit);
	if (hit.dist == -1)
	{
		return backgroundColor;
	}
	vec3 local_color = calc_local_color(ray, hit);

	double reflectance = rSchlick2(hit.normal, ray.dir, ray.ior, hit.mat.ior);
	double transmitance = 1.0 - reflectance;
	vec3 reflect_color = calc_reflect_color(ray, hit);
	vec3 refract_color = calc_refract_color(ray, hit);

	return local_color + (1.0 - hit.mat.T)* 0.7f * reflect_color + hit.mat.T * refract_color;
	//return local_color + reflectance * reflect_color + transmitance * refract_color;

}
