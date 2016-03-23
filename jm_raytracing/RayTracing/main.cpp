 ///////////////////////////////////////////////////////////////////////
//
// P3D Course
// (c) 2016 by João Madeiras Pereira
// TEMPLATE: Whitted Ray Tracing NFF scenes and drawing points with Modern OpenGL
//
//You should develop your rayTracing( Ray ray, int depth, float RefrIndex) which returns a color and
// to develop your load_NFF function
//
///////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <string>
#include <stdio.h>

#include "OpenGL\glew\glew.h"
#include "OpenGL\freeglut\freeglut.h"

#include "vectors.h"
#include "Scene.h"
#include "Color.h"
#include "Ray.h"
#include "Constants.h"

#define CAPTION "ray tracer"

#define VERTEX_COORD_ATTRIB 0
#define COLOR_ATTRIB 1

#define MAX_DEPTH 6

using namespace JMVector;
using namespace VectorConstants;

// Points defined by 2 attributes: positions which are stored in vertices array and colors which are stored in colors array
float *colors;
float *vertices;

int size_vertices;
int size_colors;

GLfloat m[16];  //projection matrix initialized by ortho function

GLuint VaoId;
GLuint VboId[2];

GLuint VertexShaderId, FragmentShaderId, ProgramId;
GLint UniformId;

Scene *scene = NULL;
int resX, resY;

/* Draw Mode: 0 - point by point; 1 - line by line; 2 - full frame */
int draw_mode = 1;

int WindowHandle = 0;

///////////////////////////////////////////////////////////////////////  RAY-TRACE SCENE
Ray CalculatePrimaryRay(int x, int y) {
	vec3 test;

	Ray ray;
	float d;// , u, v;
	//float u, v;
	vec3 aux, direction;

	//u = 0.0f + ((float)resX - 0.0f) * (((float)x + 0.5f) / (float)resX);
	//v = 0.0f + ((float)resY - 0.0f) * (((float)y + 0.5f) / (float)resY);
	//u = -(resX / 2.0f) + ((float)resX) * (((float)x + 0.5f) / (float)resX);
	//v = -(resY / 2.0f) + ((float)resY) * (((float)y + 0.5f) / (float)resY);

	aux = scene->GetCamera()->eye - scene->GetCamera()->at;
	d = aux.Length();

	//direction = -d * scene->GetCamera()->ze + u * scene->GetCamera()->ye + v * scene->GetCamera()->xe;
	direction = -d * scene->GetCamera()->ze + scene->GetCamera()->h * ((float)y / (float)resY - 0.5f) * scene->GetCamera()->ye + scene->GetCamera()->w * ((float)x / (float)resX - 0.5f) * scene->GetCamera()->xe;
	direction = direction.Normalized();

	ray.origin = scene->GetCamera()->eye;
	ray.direction = direction;

	return ray;
}

Color RayTracing(Ray ray, int depth, float refractionIndex) {
	//Color RayTracing(Ray ray, int depth){
	Color color;
	float currentDistance = -1.0f, shortestDistance = -1.0f;
	int currentObject = -1, intersectionObject = -1;
	float distance = -1.0f;
	vec3 intersectionPoint, normalIntersection, lightDirection;

	// for each object in the scene compute intersection ray - object and store the closest intersection
	for each (Object *object in scene->objects) {
		currentObject++;

		if (object->CalculateIntersection(ray, distance, intersectionPoint, normalIntersection)) {
			intersectionObject = currentObject;
		}
	}

	// if ray misses all objects shade the pixel with background color
	if (intersectionObject == -1) {
		return scene->GetColorBackground();
	}
/**/
	//color = object material’s ambient color;
	//compute normal at the hit point;
	//for (each source light) {
	//	L = unit light vector from hit point to light source; 
	//	if (L • normal>0)
	//		if (!point in shadow); //trace shadow ray
	//			color += diffuse color + specular color;
	//}
	//float lights = 0.0f;
	//float shadows = 0.0f;

	for each (Light light in scene->lights) {
		float shadowDistance = -1.0f;
		float diffuseIntensity = 0.0f;
		vec3 shadowIntersectionPoint, shadowIntersectionNormal;
		Ray shadowRay;

		lightDirection = light.center - intersectionPoint;
		lightDirection = lightDirection.Normalized();
		diffuseIntensity = normalIntersection.Dot(lightDirection);

		shadowRay.origin = intersectionPoint + lightDirection * kMaxDifference; // Add offset to shadow ray origin
		shadowRay.direction = lightDirection;

		if (diffuseIntensity > 0.0f) {
			for each (Object *object in scene->objects) {
				object->CalculateIntersection(shadowRay, shadowDistance, shadowIntersectionPoint, shadowIntersectionNormal);
			}

			if (shadowDistance == -1.0f){
/**/
				color.r += scene->objects[intersectionObject]->material.color.r * scene->objects[intersectionObject]->material.kd * diffuseIntensity;
				color.g += scene->objects[intersectionObject]->material.color.g * scene->objects[intersectionObject]->material.kd * diffuseIntensity;
				color.b += scene->objects[intersectionObject]->material.color.b * scene->objects[intersectionObject]->material.kd * diffuseIntensity;
/**/
				float specularIntensity = 0.0f;
				vec3 specular;
				//vec3 viewT, viewN;
				//viewN = (shadowRay.direction.Dot(normalIntersection)) * normalIntersection;
				//viewT = shadowRay.direction - viewN;
				//specular = viewN - viewT;
				//specular = specular.Normalized();
				specular = 2.0f * (shadowRay.direction.Dot(normalIntersection)) * normalIntersection - shadowRay.direction;

				specularIntensity = specular.Dot(scene->GetCamera()->eye.Normalized());

				if (specularIntensity > 0.0f) {
					color.r += scene->objects[intersectionObject]->material.color.r * scene->objects[intersectionObject]->material.ks * powf(specularIntensity, scene->objects[intersectionObject]->material.shine);
					color.g += scene->objects[intersectionObject]->material.color.g * scene->objects[intersectionObject]->material.ks * powf(specularIntensity, scene->objects[intersectionObject]->material.shine);
					color.b += scene->objects[intersectionObject]->material.color.b * scene->objects[intersectionObject]->material.ks * powf(specularIntensity, scene->objects[intersectionObject]->material.shine);
					//color.r += scene->objects[intersectionObject]->material.color.r * scene->objects[intersectionObject]->material.ks *specularIntensity;
					//color.g += scene->objects[intersectionObject]->material.color.g * scene->objects[intersectionObject]->material.ks *specularIntensity;
					//color.b += scene->objects[intersectionObject]->material.color.b * scene->objects[intersectionObject]->material.ks *specularIntensity;
				}
			}
		}
	}

	if (depth >= MAX_DEPTH) {
		return color;
	}

	//if (reflective object) {
	//	rRay = calculate ray in the reflected direction;
	//	rColor = trace(scene, point, rRay direction, depth + 1);
	//	reduce rColor by the specular reflection coefficient and add to color;
	//}
/**/
	if (scene->objects[intersectionObject]->material.ks > 0){
		Ray reflectedRay;
		vec3 reflectedT, reflectedN, raySimetric;
		raySimetric = -ray.direction;
		reflectedN = (raySimetric.Dot(normalIntersection)) * normalIntersection;
		reflectedT = raySimetric - reflectedN;
		reflectedRay.direction = reflectedN - reflectedT;
		reflectedRay.direction = reflectedRay.direction.Normalized();
		reflectedRay.origin = intersectionPoint + reflectedRay.direction * kMaxDifference;

		Color reflectiveColor = RayTracing(reflectedRay, depth + 1, refractionIndex);
		float reflectionIntensity = reflectedRay.direction.Dot(scene->GetCamera()->eye.Normalized());
/** /
		color.r = reflectionIntensity;
		color.g = reflectionIntensity;
		color.b = reflectionIntensity;
/**/
		if (reflectionIntensity > 0.0f){
			//color.r += reflectiveColor.r * scene->objects[intersectionObject]->material.ks * powf(reflectionIntensity, scene->objects[intersectionObject]->material.shine);
			//color.g += reflectiveColor.g * scene->objects[intersectionObject]->material.ks * powf(reflectionIntensity, scene->objects[intersectionObject]->material.shine);
			//color.b += reflectiveColor.b * scene->objects[intersectionObject]->material.ks * powf(reflectionIntensity, scene->objects[intersectionObject]->material.shine);
			color.r += reflectiveColor.r * scene->objects[intersectionObject]->material.ks * reflectionIntensity;
			color.g += reflectiveColor.g * scene->objects[intersectionObject]->material.ks * reflectionIntensity;
			color.b += reflectiveColor.b * scene->objects[intersectionObject]->material.ks * reflectionIntensity;
		}
/**/
	}
	/** /
			if (translucid object) {
				tRay = calculate ray in the refracted direction;
				tColor = trace(scene, point, tRay direction, depth + 1);
				reduce tColor by the transmittance coefficient and add to color;
			}

	/**/
	return color;
}
/////////////////////////////////////////////////////////////////////// ERRORS

bool isOpenGLError() {
	bool isError = false;
	GLenum errCode;
	const GLubyte *errString;
	while ((errCode = glGetError()) != GL_NO_ERROR) {
		isError = true;
		errString = gluErrorString(errCode);
		std::cerr << "OpenGL ERROR [" << errString << "]." << std::endl;
	}
	return isError;
}

void checkOpenGLError(std::string error)
{
	if(isOpenGLError()) {
		std::cerr << error << std::endl;
		exit(EXIT_FAILURE);
	}
}
/////////////////////////////////////////////////////////////////////// SHADERs
const GLchar* VertexShader =
{
	"#version 330 core\n"

	"in vec2 in_Position;\n"
	"in vec3 in_Color;\n"
	"uniform mat4 Matrix;\n"
	"out vec4 color;\n"

	"void main(void)\n"
	"{\n"
	"	vec4 position = vec4(in_Position, 0.0, 1.0);\n"
	"	color = vec4(in_Color, 1.0);\n"
	"	gl_Position = Matrix * position;\n"

	"}\n"
};

const GLchar* FragmentShader =
{
	"#version 330 core\n"

	"in vec4 color;\n"
	"out vec4 out_Color;\n"

	"void main(void)\n"
	"{\n"
	"	out_Color = color;\n"
	"}\n"
};

void createShaderProgram()
{
	VertexShaderId = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(VertexShaderId, 1, &VertexShader, 0);
	glCompileShader(VertexShaderId);

	FragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(FragmentShaderId, 1, &FragmentShader, 0);
	glCompileShader(FragmentShaderId);

	ProgramId = glCreateProgram();
	glAttachShader(ProgramId, VertexShaderId);
	glAttachShader(ProgramId, FragmentShaderId);

	glBindAttribLocation(ProgramId, VERTEX_COORD_ATTRIB, "in_Position");
	glBindAttribLocation(ProgramId, COLOR_ATTRIB, "in_Color");
	
	glLinkProgram(ProgramId);
	UniformId = glGetUniformLocation(ProgramId, "Matrix");

	checkOpenGLError("ERROR: Could not create shaders.");
}

void destroyShaderProgram()
{
	glUseProgram(0);
	glDetachShader(ProgramId, VertexShaderId);
	glDetachShader(ProgramId, FragmentShaderId);

	glDeleteShader(FragmentShaderId);
	glDeleteShader(VertexShaderId);
	glDeleteProgram(ProgramId);

	checkOpenGLError("ERROR: Could not destroy shaders.");
}
/////////////////////////////////////////////////////////////////////// VAOs & VBOs
void createBufferObjects()
{
	glGenVertexArrays(1, &VaoId);
	glBindVertexArray(VaoId);
	glGenBuffers(2, VboId);
	glBindBuffer(GL_ARRAY_BUFFER, VboId[0]);

	/* Não é necessário fazer glBufferData, ou seja o envio dos pontos para a placa gráfica pois isso 
	é feito na drawPoints em tempo de execução com GL_DYNAMIC_DRAW */

	glEnableVertexAttribArray(VERTEX_COORD_ATTRIB);
	glVertexAttribPointer(VERTEX_COORD_ATTRIB, 2, GL_FLOAT, 0, 0, 0);
	
	glBindBuffer(GL_ARRAY_BUFFER, VboId[1]);
	glEnableVertexAttribArray(COLOR_ATTRIB);
	glVertexAttribPointer(COLOR_ATTRIB, 3, GL_FLOAT, 0, 0, 0);
	
// unbind the VAO
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableVertexAttribArray(VERTEX_COORD_ATTRIB);
	glDisableVertexAttribArray(COLOR_ATTRIB);
	checkOpenGLError("ERROR: Could not create VAOs and VBOs.");
}

void destroyBufferObjects()
{
	glDisableVertexAttribArray(VERTEX_COORD_ATTRIB);
	glDisableVertexAttribArray(COLOR_ATTRIB);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glDeleteBuffers(1, VboId);
	glDeleteVertexArrays(1, &VaoId);
	checkOpenGLError("ERROR: Could not destroy VAOs and VBOs.");
}

void drawPoints()
{
	glBindVertexArray(VaoId);
	glUseProgram(ProgramId);

	glBindBuffer(GL_ARRAY_BUFFER, VboId[0]);
	glBufferData(GL_ARRAY_BUFFER, size_vertices, vertices, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, VboId[1]);
	glBufferData(GL_ARRAY_BUFFER, size_colors, colors, GL_DYNAMIC_DRAW);

	glUniformMatrix4fv(UniformId, 1, GL_FALSE, m);
	glDrawArrays(GL_POINTS, 0, resX * resY);
	glFinish();

	glUseProgram(0);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	checkOpenGLError("ERROR: Could not draw scene.");
}

/////////////////////////////////////////////////////////////////////// CALLBACKS

// Render function by primary ray casting from the eye towards the scene's objects

void renderScene(){
	int index_pos=0;
	int index_col=0;

	Color color;
	Ray ray;

	for (int y = 0; y < resY; y++){
		for (int x = 0; x < resX; x++){
		    //YOUR 2 FUNTIONS: 
			ray = CalculatePrimaryRay(x, y);
			color = RayTracing(ray, 1, 1.0f);
/** /
			float maximum = 0.0f;

			if (color.r > maximum){
				maximum = color.r;
			}
			if (color.g > maximum) {
				maximum = color.g;
			}
			if (color.b > maximum) {
				maximum = color.b;
			}

			if (maximum > 1.0f){
				color.r /= maximum;
				color.g /= maximum;
				color.b /= maximum;
			}
/**/
			vertices[index_pos++]= (float)x;
			vertices[index_pos++]= (float)y;
			colors[index_col++]= (float)color.r;
			colors[index_col++]= (float)color.g;
			colors[index_col++]= (float)color.b;	

			if(draw_mode == 0) {  // desenhar o conteúdo da janela ponto a ponto
				drawPoints();
				index_pos=0;
				index_col=0;
			}
		}
/** /
		printf("Line %d\n", y);
		std::cout << "Ray origin: " << ray.origin << "\n";
		std::cout << "Ray direction: " << ray.direction << "\n\n";
/**/
		if(draw_mode == 1) {  // desenhar o conteúdo da janela linha a linha
				drawPoints();
				index_pos=0;
				index_col=0;
/**/
		}
	}

	if(draw_mode == 2) //preenchar o conteúdo da janela com uma imagem completa
		 drawPoints();
	printf("Terminou!\n"); 	
}

void cleanup(){
	destroyShaderProgram();
	destroyBufferObjects();
}

void ortho(float left, float right, float bottom, float top, float nearp, float farp){
	m[0 * 4 + 0] = 2 / (right - left);
	m[0 * 4 + 1] = 0.0;
	m[0 * 4 + 2] = 0.0;
	m[0 * 4 + 3] = 0.0;
	m[1 * 4 + 0] = 0.0;
	m[1 * 4 + 1] = 2 / (top - bottom);
	m[1 * 4 + 2] = 0.0;
	m[1 * 4 + 3] = 0.0;
	m[2 * 4 + 0] = 0.0;
	m[2 * 4 + 1] = 0.0;
	m[2 * 4 + 2] = -2 / (farp - nearp);
	m[2 * 4 + 3] = 0.0;
	m[3 * 4 + 0] = -(right + left) / (right - left);
	m[3 * 4 + 1] = -(top + bottom) / (top - bottom);
	m[3 * 4 + 2] = -(farp + nearp) / (farp - nearp);
	m[3 * 4 + 3] = 1.0;
}

void reshape(int w, int h){
    glClear(GL_COLOR_BUFFER_BIT);
	glViewport(0, 0, w, h);
	ortho(0, (float)resX, 0, (float)resY, -1.0, 1.0);
}

/////////////////////////////////////////////////////////////////////// SETUP
void setupCallbacks() {
	glutCloseFunc(cleanup);
	glutDisplayFunc(renderScene);
	glutReshapeFunc(reshape);
}

void setupGLEW() {
	glewExperimental = GL_TRUE;
	GLenum result = glewInit() ; 
	if (result != GLEW_OK) { 
		std::cerr << "ERROR glewInit: " << glewGetString(result) << std::endl;
		exit(EXIT_FAILURE);
	} 
	GLenum err_code = glGetError();
	printf ("Vendor: %s\n", glGetString (GL_VENDOR));
	printf ("Renderer: %s\n", glGetString (GL_RENDERER));
	printf ("Version: %s\n", glGetString (GL_VERSION));
	printf ("GLSL: %s\n", glGetString (GL_SHADING_LANGUAGE_VERSION));
}

void setupGLUT(int argc, char* argv[]){
	glutInit(&argc, argv);
	
	glutInitContextVersion(3, 3);
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
	glutInitContextProfile(GLUT_CORE_PROFILE);

	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE,GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	
	glutInitWindowPosition(640,100);
	glutInitWindowSize(resX, resY);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
	glDisable(GL_DEPTH_TEST);
	WindowHandle = glutCreateWindow(CAPTION);
	if(WindowHandle < 1) {
		std::cerr << "ERROR: Could not create a new rendering window." << std::endl;
		exit(EXIT_FAILURE);
	}
}

void init(int argc, char* argv[])
{
	setupGLUT(argc, argv);
	setupGLEW();
	std::cerr << "CONTEXT: OpenGL v" << glGetString(GL_VERSION) << std::endl;
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	createShaderProgram();
	createBufferObjects();
	setupCallbacks();
}

int main(int argc, char* argv[]){
    //INSERT HERE YOUR CODE FOR PARSING NFF FILES
	scene = new Scene();

	if (!(scene->LoadSceneNFF("Scenes/balls_low.nff"))) {
		return 0;
	}

	resX = scene->GetCamera()->GetResX();
	resY = scene->GetCamera()->GetResY();

	if(draw_mode == 0) { // desenhar o conteúdo da janela ponto a ponto
		size_vertices = 2 * sizeof(float);
		size_colors = 3 * sizeof(float);
		printf("DRAWING MODE: POINT BY POINT\n");
	}
	else if(draw_mode == 1) { // desenhar o conteúdo da janela linha a linha
		size_vertices = 2 * resX * sizeof(float);
		size_colors = 3 * resX * sizeof(float);
		printf("DRAWING MODE: LINE BY LINE\n");
	}
	else if(draw_mode == 2) { // preencher o conteúdo da janela com uma imagem completa
		size_vertices = 2 * resX * resY * sizeof(float);
		size_colors = 3 * resX * resY * sizeof(float);
		printf("DRAWING MODE: FULL IMAGE\n");
	}
	else {
		printf("Draw mode not valid \n");
		exit(0);
	}
	printf("resx = %d  resy= %d.\n", resX, resY);

	vertices = (float*)malloc(size_vertices);
    if (vertices==NULL) exit (1);

	colors = (float*)malloc(size_colors);
    if (colors==NULL) exit (1);

	init(argc, argv);
	glutMainLoop();	
	exit(EXIT_SUCCESS);
}
///////////////////////////////////////////////////////////////////////