#pragma once
#include <vector>
#include <iostream>

class vec3
{
private:
	float calcLength(float x,float y, float z)
	{
		return sqrt(x*x + y*y + z*z);
	}
public:
	float length, x, y, z;
	vec3():vec3(0,0,0){}
	vec3(float v):vec3(v,v,v){}
	vec3(float x, float y, float z):x(x),y(y),z(z),length(calcLength(x,y,z)){}
};

inline vec3 operator-(vec3 const & v0, vec3 const & v1) {
	return vec3(v0.x - v1.x, v0.y - v1.y, v0.z - v1.z);
}

inline vec3 operator+(vec3 const & v0, vec3 const & v1) {
	return vec3(v0.x + v1.x, v0.y + v1.y, v0.z + v1.z);
}

inline vec3 operator*(float const &f, vec3 const & v) {
	return vec3(f*v.x, f*v.y, f*v.z);
}

inline vec3 operator-(vec3 const & v) {
	return vec3(-v.x, -v.y, -v.z);
}

inline std::istream &operator>>(std::istream &i, vec3 &v) {
	i >> v.x >> v.y >> v.z;
	return i;
}
class Material {
public:
	vec3 color;
	float Kd;
	float Ks;
	float Shine;
	float T;
	float ior;
};

class Ray
{
public:
	vec3 origin;
	vec3 direction;
};
class Light
{
public:
	vec3 pos;
	vec3 color;
	Light(vec3 pos, vec3 color): pos(pos), color(color){}
};

class Camera
{
public:
	vec3 from;
	vec3 at;
	vec3 up;
	float angle;
	float hither;
	int res_x;
	int res_y;
};

class Object
{
public:
	Material material;
	Object(Material mat):material(mat){}
	virtual float calcIntersection(const Ray& ray, vec3& hPoint, vec3& nPoint, bool& intoInside) = 0;
};
class Plane: public Object
{
public:
	vec3 normal;
	float offset;
	Plane(vec3 v0, vec3 v1, vec3 v2, Material mat);
	float calcIntersection(const Ray& ray, vec3& hPoint, vec3& nPoint, bool& intoInside)override;
};

class Sphere: public Object
{
public:
	vec3 pos;
	float radius;
	Sphere(vec3 pos, float radius, Material mat) :pos(pos), radius(radius), Object(mat) {}
	float calcIntersection(const Ray& ray, vec3& hPoint, vec3& nPoint, bool& intoInside) override;
};

class Triangle: public Object
{
public:
	vec3 v0, v1, v2;
	vec3 normal;
	float d;
	Triangle(vec3 v0, vec3 v1, vec3 v2, Material mat);
	float calcIntersection(const Ray& ray, vec3& hPoint, vec3& nPoint, bool& intoInside)override;
};

class Scene
{
public:
	Camera camera;
	vec3 backgroundColor;
	std::vector<Light> lights;
	std::vector<Plane> planes;
	std::vector<Sphere> spheres;
	std::vector<Triangle> triangles;
	std::vector<Object*> objects;

	int load_nff(std::string path);
	Ray calculatePrimaryRay(int x, int y);
	float calcIntersection(const Ray & ray, Object *& hitObject, vec3 & minHitPoint, vec3 & minNormalPoint, bool & minIntoInside);
	vec3 calcLocalColor(Ray ray, Object* object);
	vec3 calcReflectColor(Ray ray, Object* object);
	vec3 calcRefractColor(Ray ray, Object* object);
	vec3 rayTrace(Ray ray, int depth, float ior = 1.0f);
};