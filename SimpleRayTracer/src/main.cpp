///////////////////////////////////////////////////////////////////////
//
// P3D Course
// (c) 2016 by Jo�o Madeiras Pereira
// TEMPLATE: Whitted Ray Tracing NFF scenes and drawing points with Modern OpenGL
//
//You should develop your rayTracing( Ray ray, int depth, float RefrIndex) which returns a color and
// to develop your load_NFF function
//
///////////////////////////////////////////////////////////////////////


#include <stdlib.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <direct.h>
#include <omp.h>

#include <GL/glew.h>
#include <GL/freeglut.h>


#include "raytracer.h"
#include <algorithm>

const char* scene_files2[] = {
	"scenes/balls_low.nff", //0
	"scenes/balls_medium.nff", //1
	"scenes/balls_high.nff", //2
	"scenes/mount_low.nff", //3
	"scenes/mount_high.nff", //4
	"scenes/mount_very_high.nff", //5
	"scenes/balls_low_large.nff",	//6
	"scenes/test1.nff",	//7
	"scenes/test1_small.nff",	//7
};
const char* scene_file2 = scene_files[SCENE_FILE];

#define CAPTION "ray tracer"
#define VERTEX_COORD_ATTRIB 0
#define COLOR_ATTRIB 1

// Points defined by 2 attributes: positions which are stored in vertices array and colors which are stored in colors array
float* colors;
float* vertices;

int size_vertices;
int size_colors;

GLfloat m[16]; //projection matrix initialized by ortho function

GLuint VaoId;
GLuint VboId[2];

GLuint VertexShaderId, FragmentShaderId, ProgramId;
GLint UniformId;

Scene scene;
int RES_X, RES_Y;


int WindowHandle = 0;

///////////////////////////////////////////////////////////////////////  RAY-TRACE SCENE


/////////////////////////////////////////////////////////////////////// ERRORS

bool isOpenGLError()
{
	bool isError = false;
	GLenum errCode;
	const GLubyte* errString;
	while ((errCode = glGetError()) != GL_NO_ERROR)
	{
		isError = true;
		errString = gluErrorString(errCode);
		std::cerr << "OpenGL ERROR [" << errString << "]." << std::endl;
	}
	return isError;
}

void checkOpenGLError(std::string error)
{
	if (isOpenGLError())
	{
		std::cerr << error << std::endl;
		exit(EXIT_FAILURE);
	}
}

/////////////////////////////////////////////////////////////////////// SHADERs
const GLchar* VertexShader =
{
	"#version 330\n"
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
	"#version 330\n"

	"in vec4 color;\n"
	"out vec4 out_Color;\n"

	"void main(void)\n"
	"{\n"
	"	out_Color = color;\n"
	"}\n"
};

void createShaderProgram()
{
	GLchar buffer[1000];
	int n = 0;

	VertexShaderId = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(VertexShaderId, 1, &VertexShader, 0);
	glCompileShader(VertexShaderId);

	glGetShaderInfoLog(VertexShaderId, 1000, &n, buffer);
	if (n > 0)
		printf("%d nc log: %s\n", n, buffer);
	n = 0;

	FragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(FragmentShaderId, 1, &FragmentShader, 0);
	glCompileShader(FragmentShaderId);


	glGetShaderInfoLog(VertexShaderId, 1000, &n, buffer);
	if (n > 0)
		printf("%d nc log: %s\n", n, buffer);
	n = 0;


	ProgramId = glCreateProgram();
	glAttachShader(ProgramId, VertexShaderId);
	glAttachShader(ProgramId, FragmentShaderId);

	glBindAttribLocation(ProgramId, VERTEX_COORD_ATTRIB, "in_Position");
	glBindAttribLocation(ProgramId, COLOR_ATTRIB, "in_Color");

	glLinkProgram(ProgramId);
	UniformId = glGetUniformLocation(ProgramId, "Matrix");

	glGetProgramInfoLog(ProgramId, 1000, &n, buffer);
	if (n > 0)
		printf("%d nc log: %s\n", n, buffer);
	n = 0;


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

	/* N�o � necess�rio fazer glBufferData, ou seja o envio dos pontos para a placa gr�fica pois isso
	� feito na drawPoints em tempo de execu��o com GL_DYNAMIC_DRAW */

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
	glDrawArrays(GL_POINTS, 0, RES_X * RES_Y);
	glFinish();

	glUseProgram(0);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	checkOpenGLError("ERROR: Could not draw scene.");
}

void drawPoints(float* vertices, int size_vertices, float* colors, int size_colors)
{
	glBindVertexArray(VaoId);
	glUseProgram(ProgramId);

	glBindBuffer(GL_ARRAY_BUFFER, VboId[0]);
	glBufferData(GL_ARRAY_BUFFER, size_vertices, vertices, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, VboId[1]);
	glBufferData(GL_ARRAY_BUFFER, size_colors, colors, GL_DYNAMIC_DRAW);

	glUniformMatrix4fv(UniformId, 1, GL_FALSE, m);
	glDrawArrays(GL_POINTS, 0, RES_X * RES_Y);
	glFinish();

	glUseProgram(0);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	checkOpenGLError("ERROR: Could not draw scene.");
}

/////////////////////////////////////////////////////////////////////// CALLBACKS



// Render function by primary ray casting from the eye towards the scene's objects
bool alreadyRendered = false;

void threadedRenderScene(){
#pragma omp parallel
	{
		int x, y;
#pragma omp master
		printf("num threads %d\n", omp_get_num_threads());
#pragma omp for schedule(dynamic)
		for (y = 0; y < RES_Y; y++){
			for (x = 0; x < RES_X; x++){
				vec3 color;
				if (DRAW_MODE == 3) {
					Ray ray = scene.calculate_primary_ray(x, y);
					ray.depth = MAX_DEPTH;
					ray.ior = 1.0f;
					color = scene.ray_trace(ray);
				}else if (DRAW_MODE == 4) {
					color = scene.ray_trace_monte_carlo(x, y);
				}else if(DRAW_MODE == 5){
					color = scene.ray_trace_dof((double)x, (double)y);
				}
				else if (DRAW_MODE == 6) {
					color = scene.ray_trace_monte_carlo_dof(x, y);
				}

				int vertice_i = std::max(0, y - 1) * RES_X * 2 + x * 2;
				int color_i = std::max(0, y - 1) * RES_X * 3 + x * 3;
				vertices[vertice_i] = (float)x;
				vertices[vertice_i + 1] = (float)y;
				colors[color_i] = (float)color.x;
				colors[color_i + 1] = (float)color.y;
				colors[color_i + 2] = (float)color.z;
			}
		}
	}

	drawPoints();
}

void renderScene(){
	if (alreadyRendered){
		return;
	}else{
		alreadyRendered = true;
	}

	double start = omp_get_wtime();

	if (DRAW_MODE >= 3){
		threadedRenderScene();
		printf("Terminou! em %f\n", omp_get_wtime() - start);
		return;
	}

	int index_pos = 0;
	int index_col = 0;
	vec3 color;
	Ray ray;

	int divisionsX = 4, divisionsY = 4, divisionsMax = 4;
	float thresholdSampling = 0.3f;

	for (int y = 0; y < RES_Y; y++){
		for (int x = 0; x < RES_X; x++){
			Ray ray = scene.calculate_primary_ray(x, y);
			ray.depth = MAX_DEPTH;
			ray.ior = 1.0f;
			color = scene.ray_trace(ray);

			vertices[index_pos++] = (float)x;
			vertices[index_pos++] = (float)y;
			colors[index_col++] = (float)color.x;
			colors[index_col++] = (float)color.y;
			colors[index_col++] = (float)color.z;

			if (DRAW_MODE == 0)
			{ // desenhar o conte�do da janela ponto a ponto
				drawPoints();
				index_pos = 0;
				index_col = 0;
			}
		}
		//printf("line %d\n", y);
		if (DRAW_MODE == 1)
		{ // desenhar o conte�do da janela linha a linha
			drawPoints();
			index_pos = 0;
			index_col = 0;
		}
	}

	if (DRAW_MODE == 2) //preenchar o conte�do da janela com uma imagem completa
		drawPoints();

	printf("Terminou! em %f\n", omp_get_wtime() - start);
}

void cleanup()
{
	destroyShaderProgram();
	destroyBufferObjects();
}

void ortho(float left, float right, float bottom, float top,
           float nearp, float farp)
{
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

void reshape(int w, int h)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glViewport(0, 0, w, h);
	ortho(0, (float)RES_X, 0, (float)RES_Y, -1.0, 1.0);
}

/////////////////////////////////////////////////////////////////////// SETUP
void setupCallbacks()
{
	glutCloseFunc(cleanup);
	glutDisplayFunc(renderScene);
	glutReshapeFunc(reshape);
}

void setupGLEW()
{
	glewExperimental = GL_TRUE;
	GLenum result = glewInit();
	if (result != GLEW_OK)
	{
		std::cerr << "ERROR glewInit: " << glewGetString(result) << std::endl;
		exit(EXIT_FAILURE);
	}
	GLenum err_code = glGetError();
	printf("Vendor: %s\n", glGetString(GL_VENDOR));
	printf("Renderer: %s\n", glGetString(GL_RENDERER));
	printf("Version: %s\n", glGetString(GL_VERSION));
	printf("GLSL: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
}

void setupGLUT(int argc, char* argv[])
{
	glutInit(&argc, argv);

	glutInitContextVersion(3, 3);
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
	glutInitContextProfile(GLUT_CORE_PROFILE);

	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

	glutInitWindowPosition(640, 100);
	glutInitWindowSize(RES_X, RES_Y);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
	glDisable(GL_DEPTH_TEST);
	WindowHandle = glutCreateWindow(CAPTION);
	if (WindowHandle < 1)
	{
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

int main2(int argc, char* argv[])
{
	if (scene.load_nff(scene_file))
	{
		printf("failed to load nff\n");
		system("pause");
		return 1;
	}

	RES_X = scene.camera.res_x;
	RES_Y = scene.camera.res_y;

	if (DRAW_MODE == 0){ // desenhar o conte�do da janela ponto a ponto
		size_vertices = 2 * sizeof(float);
		size_colors = 3 * sizeof(float);
		printf("DRAWING MODE: POINT BY POINT\n");
	}else if (DRAW_MODE == 1){ // desenhar o conte�do da janela linha a linha
		size_vertices = 2 * RES_X * sizeof(float);
		size_colors = 3 * RES_X * sizeof(float);
		printf("DRAWING MODE: LINE BY LINE\n");
	}else if (DRAW_MODE == 2){ // preencher o conte�do da janela com uma imagem completa
		size_vertices = 2 * RES_X * RES_Y * sizeof(float);
		size_colors = 3 * RES_X * RES_Y * sizeof(float);
		printf("DRAWING MODE: FULL IMAGE\n");
	}
	else if (DRAW_MODE >= 3){ // use omp to draw image at once
		size_vertices = 2 * RES_X * RES_Y * sizeof(float);
		size_colors = 3 * RES_X * RES_Y * sizeof(float);
		printf("DRAWING MODE: FULL IMAGE WITH THREADS\n");

		if (DRAW_MODE == 3) {
			printf("    Simple Render\n");
		}else if (DRAW_MODE == 4) {
			printf("    Monte Carlo Render\n");
		}else if (DRAW_MODE == 5) {
			printf("    Depth of Field Render\n");
		}
		else if (DRAW_MODE == 6) {
			printf("    Depth of Field with Monte Carlo Render\n");
		}
	}else{
		printf("Draw mode not valid \n");
		exit(0);
	}

	printf("resx = %d  resy= %d.\n", RES_X, RES_Y);

	vertices = (float*)malloc(size_vertices);

	if (vertices == NULL) {
		exit(1);
	}

	colors = (float*)malloc(size_colors);

	if (colors == NULL) {
		exit(1);
	}

	init(argc, argv);
	glutMainLoop();
	exit(EXIT_SUCCESS);
}


///////////////////////////////////////////////////////////////////////


