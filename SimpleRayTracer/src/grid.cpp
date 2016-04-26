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
	wx = bbox.max.x - bbox.min.x;
	wy = bbox.max.y - bbox.min.y;
	wz = bbox.max.z - bbox.min.z;

	double m = GRID_WIDTH_MULTIPLIER;	// increases number of cells per object
	double s = pow(wx*wy*wz / num_objects, 1.0/3.0);
	nx = (m * wx / s)+1;
	ny = (m * wy / s)+1;
	nz = (m * wz / s)+1;

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
		int ixmin = clamp(((obj_bbox.min.x - bbox.min.x) / wx*nx), 0, nx - 1);
		int iymin = clamp(((obj_bbox.min.y - bbox.min.y) / wy*ny), 0, ny - 1);
		int izmin = clamp(((obj_bbox.min.z - bbox.min.z) / wz*nz), 0, nz - 1);
		int ixmax = clamp(((obj_bbox.max.x - bbox.min.x) / wx*nx), 0, nx - 1);
		int iymax = clamp(((obj_bbox.max.y - bbox.min.y) / wy*ny), 0, ny - 1);
		int izmax = clamp(((obj_bbox.max.z - bbox.min.z) / wz*nz), 0, nz - 1);

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
	// count how many cells for a give num of objects (array's index)
	std::vector<int> counts(num_objects + 1, 0);
	for (auto& cell : cells)
	{
		counts[cell.size()]++;
	}
	printf("GRID bbox min: (%f, %f, %f)\n", bbox.min.x, bbox.min.y, bbox.min.z);
	printf("GRID bbox max: (%f, %f, %f)\n", bbox.max.x, bbox.max.y, bbox.max.z);
	printf("GRID nx:%d ny:%d nz:%d\n", nx, ny, nz);
	printf("GRID num cells:%d\n", num_cells);
	printf("GRID num objects in all cells %f\n", sum);
	printf("GRID obj/voxel %f\n", sum / num_cells);
	printf("GRID num_objects:how_many_cells:\n");
	/*
	for (int i = 0; i < counts.size(); i++)
	{
		if(counts[i]>0)
		{
			//printf("%d:%d ", i, counts[i]);			
		}
		if (i == counts.size() - 1)
		{
			printf("\n");
		}
	}*/
}

vec3 Grid::min_coordinates() {
	vec3 min(DBL_MAX, DBL_MAX, DBL_MAX);
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
	vec3 max(DBL_MIN, DBL_MIN, DBL_MIN);
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
	double ox = ray.origin.x;
	double oy = ray.origin.y;
	double oz = ray.origin.z;

	double dx = ray.dir.x;
	double dy = ray.dir.y;
	double dz = ray.dir.z;

	double tx_min, ty_min, tz_min;
	double tx_max, ty_max, tz_max;

	int ix, iy, iz;
	double t0, t1;


	// calculate if ray hits the grid bbox (kay ,kajya)
	
	// x direction
	double a = 1.0 / dx;
	if (a >= 0)
	{
		tx_min = (bbox.min.x - ox)*a;
		tx_max = (bbox.max.x - ox)*a;
	}
	else
	{
		tx_min = (bbox.max.x - ox)*a;
		tx_max = (bbox.min.x - ox)*a;
	}

	// y direction
	double b = 1.0 / dy;
	if (b >= 0)
	{
		ty_min = (bbox.min.y - oy)*b;
		ty_max = (bbox.max.y - oy)*b;
	}
	else
	{
		ty_min = (bbox.max.y - oy)*b;
		ty_max = (bbox.min.y - oy)*b;
	}

	double c = 1.0 / dz;
	if (c >= 0)
	{
		tz_min = (bbox.min.z - oz)*c;
		tz_max = (bbox.max.z - oz)*c;
	}
	else
	{
		tz_min = (bbox.max.z - oz)*c;
		tz_max = (bbox.min.z - oz)*c;
	}

	// find largest entering value

	if (tx_min > ty_min)
		t0 = tx_min;
	else
		t0 = ty_min;

	if (tz_min > t0)
		t0 = tz_min;

	// find smalles exiting value

	if (tx_max < ty_max)
		t1 = tx_max;
	else
		t1 = ty_max;

	if (tz_max < t1)
		t1 = tz_max;

	if (!bbox.inside(ray.origin)) {
		bool intersects = t0 < t1;
		if(!intersects)
			return;
		vec3 p = ray.origin + t0 * ray.dir;
		ix = clamp(floor((p.x - bbox.min.x)*nx / wx), 0, nx - 1);
		iy = clamp(floor((p.y - bbox.min.y)*ny / wy), 0, ny - 1);
		iz = clamp(floor((p.z - bbox.min.z)*nz / wz), 0, nz - 1);
	}else
	{
		ix = clamp(floor((ox - bbox.min.x)*nx / wx), 0, nx - 1);
		iy = clamp(floor((oy - bbox.min.y)*ny / wy), 0, ny - 1);
		iz = clamp(floor((oz - bbox.min.z)*nz / wz), 0, nz - 1);
	}
	
	double dtx = (tx_max - tx_min) / nx;
	double dty = (ty_max - ty_min) / ny;
	double dtz = (tz_max - tz_min) / nz;

	double tx_next, ty_next, tz_next;
	int ix_step, iy_step, iz_step;
	int ix_stop, iy_stop, iz_stop;


	if (dx > 0) {
		tx_next = tx_min + (ix + 1) * dtx;
		ix_step = +1;
		ix_stop = nx;
	}
	else {
		tx_next = tx_min + (nx - ix) * dtx;
		ix_step = -1;
		ix_stop = -1;
	}

	if (dx == 0.0) {
		tx_next = DBL_MAX;
		ix_step = -1;
		ix_stop = -1;
	}


	if (dy > 0) {
		ty_next = ty_min + (iy + 1) * dty;
		iy_step = +1;
		iy_stop = ny;
	}
	else {
		ty_next = ty_min + (ny - iy) * dty;
		iy_step = -1;
		iy_stop = -1;
	}

	if (dy == 0.0) {
		ty_next = DBL_MAX;
		iy_step = -1;
		iy_stop = -1;
	}

	if (dz > 0) {
		tz_next = tz_min + (iz + 1) * dtz;
		iz_step = +1;
		iz_stop = nz;
	}
	else {
		tz_next = tz_min + (nz - iz) * dtz;
		iz_step = -1;
		iz_stop = -1;
	}

	if (dz == 0.0) {
		tz_next = DBL_MAX;
		iz_step = -1;
		iz_stop = -1;
	}

	
	/*
	ix_step = dx > 0 ? 1 : -1;
	iy_step = dy > 0 ? 1 : -1;
	iz_step = dz > 0 ? 1 : -1;

	tx_next = tx_min + ix*dtx;
	ty_next = ty_min + iy*dty;
	tz_next = tz_min + iz*dtz;

	ix_stop = nx;
	iy_stop = ny;
	iz_stop = nz;
	*/
	while (true)
	{
		std::vector<Object*>& objects = cells[ix + iy*nx + nx*ny*iz];
		//hit.found = false;
		for (Object* obj : objects)
		{
			obj->calcIntersection(ray, hit);
		}		
		if (tx_next < ty_next && tx_next < tz_next)
		{
			if (hit.found && hit.dist <= tx_next)
				return;
			tx_next += dtx;			
			ix += ix_step;
			if (ix == ix_stop)
				return;
		}
		else if (ty_next < tz_next)
		{
			if (hit.found && hit.dist <= ty_next)
				return;
			ty_next += dty;
			iy += iy_step;
			if (iy == iy_stop)
				return;
		}
		else
		{
			if (hit.found && hit.dist <= tz_next)
				return;
			tz_next += dtz;
			iz += iz_step;
			if (iz == iz_stop)
				return;
		}
	}
}
