#include "raytracer.h"
#include <fstream>
#include <string>
#define _USE_MATH_DEFINES
#include <math.h>
#include <algorithm>

int clamp(int n, int lower, int upper)
{
	return std::max(lower, std::min(n, upper));
}

/*
Code based on chapter 22 from "Ray Tracing From The Ground Up"
*/
void Grid::initializeGrid()
{

	// compute bounding box
	bbox.min = min_coordinates();
	bbox.max = max_coordinates();

	// calculate number of cells 
	int num_objects = scene->objects.size();
	// width in x,y,z direction
	double wx = bbox.max.x - bbox.min.x;
	double wy = bbox.max.y - bbox.min.y;
	double wz = bbox.max.z - bbox.min.z;
	double m = GRID_WIDTH_MULTIPLIER;	// increases number of cells per object
	double s = pow(wx*wy*wz / num_objects, 0.3333333);
	nx = m * wx / s + 1;
	ny = m * wy / s + 1;
	nz = m * wz / s + 1;

	int num_cells = nx * ny * nz;

	cells.reserve(num_cells);

	for (int j = 0; j < num_cells; j++)
		cells.push_back(std::vector<Object*>());

	BBox obj_bbox;
	int index; // cell array index
	double sum = 0;
	for (Object* obj : scene->objects)
	{
		obj_bbox = obj->bbox;
		int ixmin = clamp((obj_bbox.min.x - bbox.min.x)*nx / wx, 0, nx - 1);
		int iymin = clamp((obj_bbox.min.y - bbox.min.y)*ny / wy, 0, ny - 1);
		int izmin = clamp((obj_bbox.min.z - bbox.min.z)*nz / wz, 0, nz - 1);
		int ixmax = clamp((obj_bbox.max.x - bbox.min.x)*nx / wx, 0, nx - 1);
		int iymax = clamp((obj_bbox.max.y - bbox.min.y)*ny / wy, 0, ny - 1);
		int izmax = clamp((obj_bbox.max.z - bbox.min.z)*nz / wz, 0, nz - 1);

		// Add object to the cells
		for (int iz = izmin; iz <= izmax; iz++)
			for (int iy = iymin; iy <= iymax; iy++)
				for (int ix = ixmin; ix <= ixmax; ix++)
				{
					index = ix + nx * iy + nx * ny * iz;
					cells[index].push_back(obj);
					sum++;
				}
	}

	printf("Grid bbox min: (%f, %f, %f)\n", bbox.min.x, bbox.min.y, bbox.min.z);
	printf("Grid bbox max: (%f, %f, %f)\n", bbox.max.x, bbox.max.y, bbox.max.z);
	printf("Grid nx:%d ny:%d nz:%d\n", nx, ny, nz);
	printf("Grid obj/voxel %f\n", sum / num_cells);
}

vec3 Grid::min_coordinates() {
	vec3 min(FLT_MAX, FLT_MAX, FLT_MAX);
	BBox bbox;
	for (Object*& obj : scene->objects)
	{
		if (obj->isBounded() == false) continue;
		bbox = obj->bbox;
		if (bbox.min.x < min.x)
			min.x = bbox.min.x;
		if (bbox.min.y < min.y)
			min.y = bbox.min.y;
		if (bbox.min.z < min.z)
			min.z = bbox.min.z;
	}
	min = min - EPSILON;
	return min;
}

vec3 Grid::max_coordinates() {
	vec3 max(FLT_MIN, FLT_MIN, FLT_MIN);
	BBox bbox;
	for (Object*& obj : scene->objects)
	{
		if (obj->isBounded() == false) continue;
		bbox = obj->bbox;
		if (bbox.max.x > max.x)
			max.x = bbox.max.x;
		if (bbox.max.y > max.y)
			max.y = bbox.max.y;
		if (bbox.max.z > max.z)
			max.z = bbox.max.z;
	}
	max = max + EPSILON;
	return max;
}

void Grid::calc_hit(Ray& ray, Hit& hit)
{
	// calculate if ray hits the grid bbox (kay ,kajya)
	double ox = ray.origin.x;
	double oy = ray.origin.y;
	double oz = ray.origin.z;

	double dx = ray.origin.x + DBL_EPSILON;
	double dy = ray.origin.y + DBL_EPSILON;
	double dz = ray.origin.z + DBL_EPSILON;

	double tx_min, ty_min, tz_min;
	double tx_max, ty_max, tz_max;

	double a = 1.0 / dx;
	if( a >= 0)
	{
		tx_min = (bbox.min.x - ox)*a;
		tx_max = (bbox.max.x - ox)*a;
	}else
	{
		tx_min = (bbox.max.x - ox)*a;
		tx_max = (bbox.min.x - ox)*a;
	}

	double b = 1.0 / dy;
	if( b >= 0)
	{
		ty_min = (bbox.min.y - oy)*b;
		ty_max = (bbox.max.y - oy)*b;
	}else
	{
		ty_min = (bbox.max.y - oy)*b;
		ty_max = (bbox.min.y - oy)*b;
	}

	double c = 1.0 / dz;
	if(b >= 0)
	{
		tz_min = (bbox.min.z - oz)*c;
		tz_max = (bbox.max.z - oz)*c;
	}else
	{
		tz_min = (bbox.max.z - oz)*c;
		tz_max = (bbox.min.z - oz)*c;
	}

	double t_max, t_min;

	// find largest entering value

	if (tx_min > ty_min)
		t_max = tx_min;
	else
		t_max = ty_min;

	if (tz_min > t_max)
		t_max = tz_min;

	// find smalles  entering value

	if (tx_max < ty_max)
		t_min = tx_max;
	else
		t_min = ty_max;

	if (tz_max < t_min)
		t_min = tz_max;
	
	bool intersects = t_max < t_min && t_min > EPSILON;

	if (!intersects)
		return;

	// voxel coordinates

	int ix, iy, iz;
	double wx = bbox.max.x - bbox.min.x;
	double wy = bbox.max.y - bbox.min.y;
	double wz = bbox.max.z - bbox.min.z;
	if(bbox.inside(ray.origin))
	{
		ix = clamp((ox - bbox.min.x)*nx / wx, 0, nx - 1);
		iy = clamp((oy - bbox.min.y)*ny / wy, 0, ny - 1);
		iz = clamp((oz - bbox.min.z)*nz / wz, 0, nz - 1);
	}else
	{
		vec3 p = ray.origin + t_min * ray.dir;
		ix = clamp((p.x - bbox.min.x)*nx / wx, 0, nx - 1);
		iy = clamp((p.y - bbox.min.y)*ny / wy, 0, ny - 1);
		iz = clamp((p.z - bbox.min.z)*nz / wz, 0, nz - 1);
	}

	double dtx = (tx_max - tx_min) / nx;
	double dty = (ty_max - ty_min) / ny;
	double dtz = (tz_max - tz_min) / nz;
	
	double tx_next, ty_next, tz_next;
	int ix_step, iy_step, iz_step;
	int ix_stop, iy_stop, iz_stop;

	ix_step = ray.dir.x > 0 ? 1 : -1;
	iy_step = ray.dir.y > 0 ? 1 : -1;
	iz_step = ray.dir.z > 0 ? 1 : -1;
	
	tx_next = tx_min + (ix + ix_step)*dtx;
	ty_next = ty_min + (iy + iy_step)*dty;
	tz_next = tz_min + (iz + iz_step)*dtz;

	ix_stop = nx;
	iy_stop = ny;
	iz_stop = nz;

	while(true)
	{
		std::vector<Object*>& objects = cells[ix + iy*nx + nx*ny*iz];
		for (Object* obj : objects)
		{
			obj->calcIntersection(ray, hit);
		}
		if (hit.found)
			return;
		if(tx_next < ty_next && tx_next < tz_next)
		{
			tx_next += dtx;
			ix_step++;
			if (ix_step == ix_stop)
				return;
		}else if(ty_next < tz_next)
		{
			ty_next += dty;
			iy_step++;
			if (iy_step == iy_stop)
				return;
		}
		else
		{
			tz_next += dtx;
			iz_step++;
			if (iz_step == iz_stop)
				return;
		}
	}
}
