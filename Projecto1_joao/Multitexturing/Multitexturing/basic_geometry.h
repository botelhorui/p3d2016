
#define MAX_TEXTURES 4

struct Material{
	float diffuse[4];
	float ambient[4];
	float specular[4];
	float emissive[4];
	float shininess;
	int texCount;
};

// A model can be made of many meshes. Each is stored  in the following structure
struct MyMesh {
		GLuint vao;
		GLuint texUnits[MAX_TEXTURES];
		GLuint texTypes[MAX_TEXTURES];
		float transform[16];
		int numIndexes;
		unsigned int type;
		struct Material mat;
	};

void createSkybox();
void createCube();
void CalculateTangentArray(long vertexCount, const float *vertex, const float *normal, const float *texcoord, long triangleCount, const unsigned *triangle, float *tangent);