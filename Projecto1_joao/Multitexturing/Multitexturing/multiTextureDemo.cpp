//
// AVT Multitexturing light 
// based on demos from GLSL Core Tutorial in Lighthouse3D.com   
//
// This demo was built for learning purposes only.
// Some code could be severely optimised, but I tried to
// keep as simple and clear as possible.
//
// The code comes with no warranties, use it at your own risk.
// You may use it, or parts of it, wherever you want.
//
//
//Author: Jo�o Madeiras Pereira

#include <math.h>
#include <iostream>
#include <sstream>

#include <string>

// include GLEW to access OpenGL 3.3 functions
#include "OpenGL\glew\glew.h"

// GLUT is the toolkit to interface with the OS
#include "OpenGL\freeglut\freeglut.h"

// Use Very Simple Libs
#include "VSShaderlib.h"
#include "AVTmathLib.h"
#include "VertexAttrDef.h"
#include "basic_geometry.h"
#include "TGA.h"

#define CAPTION "AVT MultiTexture Demo"
#define FPS 60
int WindowHandle = 0;
int WinX = 640, WinY = 480;

unsigned int FrameCount = 0;



VSShaderLib shader;
VSShaderLib shaderSkybox;

struct MyMesh mesh[2];
int objId = 0; //id of the object mesh - to be used as index of mesh: mesh[objID] means the current mesh


//External array storage defined in AVTmathLib.cpp

/// The storage for matrices
extern float mMatrix[COUNT_MATRICES][16];
extern float mCompMatrix[COUNT_COMPUTED_MATRICES][16];

/// The normal matrix
extern float mNormal3x3[9];

GLint pvm_uniformId;
GLint vm_uniformId;
GLint v_uniformId;
GLint p_uniformId;
GLint normal_uniformId;
GLint lPos_uniformId;
GLint tex_loc, tex_loc1;
GLint tex_loc2; // cube map

// Skybox
GLint sky_tex_loc; // cube map
GLint sky_p_uniformId;
GLint sky_v_uniformId;

GLuint TextureArray[3];
GLuint textureCubeMap;
//GLuint fragmentShaderSkyboxId;
//GLuint vertexShaderSkyboxId;

// Camera Position
float camX, camY, camZ;

// Mouse Tracking Variables
int startX, startY, tracking = 0;

// Camera Spherical Coordinates
float alpha = 39.0f, beta = 51.0f;
float r = 5.0f;

// Frame counting and FPS computation
long myTime,timebase = 0,frame = 0;
char s[32];
float lightPos[4] = {4.0f, 6.0f, 2.0f, 1.0f};

// Rotation around axis.
// 0 - none
// 1 - x
// 2 - y
// 3 - z
int rotateObject = 0;
float rotateAngleX = 0, rotateAngleY = 0, rotateAngleZ = 0;

void timer(int value)
{
	std::ostringstream oss;
	oss << CAPTION << ": " << FrameCount << " FPS @ (" << WinX << "x" << WinY << ")";
	std::string s = oss.str();
	glutSetWindow(WindowHandle);
	glutSetWindowTitle(s.c_str());
    FrameCount = 0;
    glutTimerFunc(1000, timer, 0);
}

void refresh(int value)
{
	glutPostRedisplay();
	glutTimerFunc(1000/FPS, refresh, 0);
}

// ------------------------------------------------------------
//
// Reshape Callback Function
//

void changeSize(int w, int h) {

	float ratio;
	// Prevent a divide by zero, when window is too short
	if(h == 0)
		h = 1;
	// set the viewport to be the entire window
	glViewport(0, 0, w, h);
	// set the projection matrix
	ratio = (1.0f * w) / h;
	loadIdentity(PROJECTION);
	perspective(53.13f, ratio, 0.1f, 1000.0f);
}


// ------------------------------------------------------------
//
// Render stufff
//

void renderScene(void) {
	GLint loc;

	FrameCount++;
	glClearColor(0.2f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glEnable(GL_TEXTURE_CUBE_MAP);
	// load identity matrices
	loadIdentity(VIEW);
	loadIdentity(MODEL);
	// set the camera using a function similar to gluLookAt
	lookAt(camX, camY, camZ, 0, 0, 0, 0, 1, 0);

/**/
	// Model rendering
	glUseProgram(shader.getProgramIndex());

	//send the light position in eye coordinates

		//glUniform4fv(lPos_uniformId, 1, lightPos); //efeito capacete do mineiro, ou seja lighPos foi definido em eye coord 

	float res[4];
	multMatrixPoint(VIEW, lightPos, res);   //lightPos definido em World Coord so it is converted to eye space
	glUniform4fv(lPos_uniformId, 1, res);

	//Associar os Texture Units aos Objects Texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TextureArray[0]);
/** /
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, TextureArray[1]);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureCubeMap);

	//Indicar aos tres samplers do GLSL quais os Texture Units a serem usados
	glUniform1i(tex_loc, 0);
	glUniform1i(tex_loc1, 1);
	glUniform1i(tex_loc2, 2);
/**/
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureCubeMap);

	glUniform1i(tex_loc, 0);
	glUniform1i(tex_loc2, 1);

	objId = 0;

	// send the material
	loc = glGetUniformLocation(shader.getProgramIndex(), "mat.ambient");
	glUniform4fv(loc, 1, mesh[objId].mat.ambient);
	loc = glGetUniformLocation(shader.getProgramIndex(), "mat.diffuse");
	glUniform4fv(loc, 1, mesh[objId].mat.diffuse);
	loc = glGetUniformLocation(shader.getProgramIndex(), "mat.specular");
	glUniform4fv(loc, 1, mesh[objId].mat.specular);
	loc = glGetUniformLocation(shader.getProgramIndex(), "mat.shininess");
	glUniform1f(loc, mesh[objId].mat.shininess);
	pushMatrix(MODEL);

	switch (rotateObject) {
		case 1:
			rotateAngleX += 0.05f;
			//rotate(MODEL, rotateAngleX, 1.0f, 0.0f, 0.0f);
			break;
		case 2:
			rotateAngleY += 0.05f;
			//rotate(MODEL, rotateAngleY, 0.0f, 1.0f, 0.0f);
			break;
		case 3:
			rotateAngleZ += 0.05f;
			//rotate(MODEL, rotateAngleZ, 0.0f, 0.0f, 1.0f);
			break;
		default:
			break;
	}

	// send matrices to OGL
	computeDerivedMatrix(PROJ_VIEW_MODEL);
	glUniformMatrix4fv(vm_uniformId, 1, GL_FALSE, mCompMatrix[VIEW_MODEL]);
	glUniformMatrix4fv(v_uniformId, 1, GL_FALSE, mMatrix[VIEW]);
	glUniformMatrix4fv(pvm_uniformId, 1, GL_FALSE, mCompMatrix[PROJ_VIEW_MODEL]);
	computeNormalMatrix3x3();
	glUniformMatrix3fv(normal_uniformId, 1, GL_FALSE, mNormal3x3);

	// Render mesh
	glBindVertexArray(mesh[objId].vao);
	glDrawElements(mesh[objId].type, mesh[objId].numIndexes, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	popMatrix(MODEL);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

/**/
	// Skybox rendering
	glDepthFunc(GL_LEQUAL);
	glUseProgram(shaderSkybox.getProgramIndex());

	objId = 1;

	glActiveTexture(GL_TEXTURE0);
	glUniform1i(sky_tex_loc, 0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureCubeMap);

	glUniformMatrix4fv(sky_p_uniformId, 1, GL_FALSE, mMatrix[PROJECTION]);
	glUniformMatrix4fv(sky_v_uniformId, 1, GL_FALSE, mMatrix[VIEW]);

	glBindVertexArray(mesh[objId].vao);
	//glDrawElements(mesh[objId].type, mesh[objId].numIndexes, GL_UNSIGNED_INT, 0);
	glDrawArrays(mesh[objId].type, 0, mesh[objId].numIndexes);
	glBindVertexArray(0);
	glDepthFunc(GL_LESS);
	// Skybox rendering
/**/

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glutSwapBuffers();
}

// ------------------------------------------------------------
//
// Events from the Keyboard
//

void processKeys(unsigned char key, int xx, int yy){
	switch(key) {
		case 27: // ESC
			glutLeaveMainLoop();
			break;
/*
		case 'a':
			printf("Rotation in X axis.\n");
			break;
		case 's':
			printf("Rotation in Y axis.\n");
			break;
		case 'd':
			printf("Rotation in Z axis.\n");
			break;
*/
		case 'c':
			printf("Camera Spherical Coordinates (%f, %f, %f)\n", alpha, beta, r);
			break;
		case 'm': 
			glEnable(GL_MULTISAMPLE); 
			break;
		case 'n': 
			glDisable(GL_MULTISAMPLE); 
			break;
	}
}


// ------------------------------------------------------------
//
// Mouse Events
//

void processMouseButtons(int button, int state, int xx, int yy){
	// start tracking the mouse
	if (state == GLUT_DOWN)  {
		startX = xx;
		startY = yy;
		if (button == GLUT_LEFT_BUTTON) {
			tracking = 1;
		}
		else {
			if (button == GLUT_RIGHT_BUTTON) {
				tracking = 2;
			}
			else
			{
				// Rotate objects.
				if (button == GLUT_MIDDLE_BUTTON)
				{
					rotateObject++;

					if (rotateObject > 3)
					{
						rotateObject = 0;
					}
				}
			}
		}
	}

	//stop tracking the mouse
	else if (state == GLUT_UP) {
		if (tracking == 1) {
			alpha -= (xx - startX);
			beta += (yy - startY);
		}
		else {
			if (tracking == 2) {
				r += (yy - startY) * 0.01f;

				if (r < 0.1f) {
					r = 0.1f;
				}
			}
		}

		tracking = 0;
	}
}

// Track mouse motion while buttons are pressed
void processMouseMotion(int xx, int yy){
	int deltaX, deltaY;
	float alphaAux, betaAux;
	float rAux;

	deltaX =  - xx + startX;
	deltaY =    yy - startY;

	//std::cout << "Delta X: " << deltaX << "\n";
	//std::cout << "Delta Y: " << deltaY << "\n\n";

	// left mouse button: move camera
	if (tracking == 1) {
		alphaAux = alpha + deltaX;
		betaAux = beta + deltaY;

		if (betaAux > 85.0f) {
			betaAux = 85.0f;
		}
		else {
			if (betaAux < -85.0f) {
				betaAux = -85.0f;
			}
		}

		rAux = r;

		camX = rAux * sin(alphaAux * 3.14f / 180.0f) * cos(betaAux * 3.14f / 180.0f);
		camZ = rAux * cos(alphaAux * 3.14f / 180.0f) * cos(betaAux * 3.14f / 180.0f);
		camY = rAux *   						       sin(betaAux * 3.14f / 180.0f);
	}
	// right mouse button: change light source position
	else {
		if (tracking == 2) {
			//std::cout << "Light position initial: (" << lightPos[0] << ", " << lightPos[1] << ", " << lightPos[2] << ")\n";

			if (deltaX > 0) {
				lightPos[0] += 0.25;
			}
			else {
				if (deltaX < 0) {
					lightPos[0] -= 0.25;
				}
			}

			if (deltaY > 0) {
				lightPos[2] += 0.25;
			}
			else {
				if (deltaY < 0) {
					lightPos[2] -= 0.25;
				}
			}

			if (lightPos[0] > 25.0f)
			{
				lightPos[0] = 25.0f;
			}
			else
			{
				if (lightPos[0] < -25.0f)
				{
					lightPos[0] = -25.0f;
				}
			}

			if (lightPos[2] > 25.0f)
			{
				lightPos[2] = 25.0f;
			}
			else
			{
				if (lightPos[2] < -25.0f)
				{
					lightPos[2] = -25.0f;
				}
			}

			//std::cout << "Light position final: (" << lightPos[0] << ", " << lightPos[1] << ", " << lightPos[2] << ")\n\n";
		}
	}

//  uncomment this if not using an idle func
//	glutPostRedisplay();
}


void mouseWheel(int wheel, int direction, int x, int y) {
	r += direction * -0.25f;
	if (r < 0.1f)
		r = 0.1f;

	camX = r * sin(alpha * 3.14f / 180.0f) * cos(beta * 3.14f / 180.0f);
	camZ = r * cos(alpha * 3.14f / 180.0f) * cos(beta * 3.14f / 180.0f);
	camY = r *   						     sin(beta * 3.14f / 180.0f);

//  uncomment this if not using an idle func
//	glutPostRedisplay();
}

// --------------------------------------------------------
//
// Shader Stuff
//


GLuint setupShaders() {
	// Shader for models
	shader.init();
	//shader.loadShader(VSShaderLib::VERTEX_SHADER, "shaders/texture_demo.vert");
	//shader.loadShader(VSShaderLib::FRAGMENT_SHADER, "shaders/texture_demo.frag");
	shader.loadShader(VSShaderLib::VERTEX_SHADER, "shaders/shader_cube_map.vert");
	shader.loadShader(VSShaderLib::FRAGMENT_SHADER, "shaders/shader_cube_map.frag");

	// set semantics for the shader variables
	glBindFragDataLocation(shader.getProgramIndex(), 0,"colorOut");
	glBindAttribLocation(shader.getProgramIndex(), VERTEX_COORD_ATTRIB, "position");
	glBindAttribLocation(shader.getProgramIndex(), NORMAL_ATTRIB, "normal");
	glBindAttribLocation(shader.getProgramIndex(), TEXTURE_COORD_ATTRIB, "texCoord");
	glBindAttribLocation(shader.getProgramIndex(), TANGENT_ATTRIB, "tangent");

	glLinkProgram(shader.getProgramIndex());

	pvm_uniformId = glGetUniformLocation(shader.getProgramIndex(), "m_pvm");
	vm_uniformId = glGetUniformLocation(shader.getProgramIndex(), "m_viewModel");
	v_uniformId = glGetUniformLocation(shader.getProgramIndex(), "m_view");
	normal_uniformId = glGetUniformLocation(shader.getProgramIndex(), "m_normal");
	lPos_uniformId = glGetUniformLocation(shader.getProgramIndex(), "l_pos");
	tex_loc = glGetUniformLocation(shader.getProgramIndex(), "texMap");
	tex_loc1 = glGetUniformLocation(shader.getProgramIndex(), "texNormal");
	tex_loc2 = glGetUniformLocation(shader.getProgramIndex(), "texCubeMap");

	//printf("\n\nInfoLog for Model Shader\n%s\n\n", shader.getAllInfoLogs().c_str());
	printf("\n\nInfoLog for World Shader\n%s", shader.getAllInfoLogs().c_str());

	return(shader.isProgramValid());
}

GLuint setupSkyboxShaders() {
	// Shader for models
	shaderSkybox.init();
	shaderSkybox.loadShader(VSShaderLib::VERTEX_SHADER, "shaders/shader_skybox.vert");
	shaderSkybox.loadShader(VSShaderLib::FRAGMENT_SHADER, "shaders/shader_skybox.frag");

	// set semantics for the shader variables
	glBindFragDataLocation(shaderSkybox.getProgramIndex(), 0, "colorOut");
	glBindAttribLocation(shaderSkybox.getProgramIndex(), VERTEX_COORD_ATTRIB, "position");

	glLinkProgram(shaderSkybox.getProgramIndex());

	sky_v_uniformId = glGetUniformLocation(shaderSkybox.getProgramIndex(), "m_view");
	sky_p_uniformId = glGetUniformLocation(shaderSkybox.getProgramIndex(), "m_projection");
	sky_tex_loc = glGetUniformLocation(shaderSkybox.getProgramIndex(), "texCubeMap");

	//printf("InfoLog for Skybox Shader\n%s\n\n", shader.getAllInfoLogs().c_str());
	printf("InfoLog for Skybox Shader\n%s", shaderSkybox.getAllInfoLogs().c_str());

	return(shaderSkybox.isProgramValid());
}
// ------------------------------------------------------------
//
// Model loading and OpenGL setup
//

void init(){
	// set the camera position based on its spherical coordinates
	camX = r * sin(alpha * 3.14f / 180.0f) * cos(beta * 3.14f / 180.0f);
	camZ = r * cos(alpha * 3.14f / 180.0f) * cos(beta * 3.14f / 180.0f);
	camY = r *   						     sin(beta * 3.14f / 180.0f);

	//Texture Object definition
/**/	
	glGenTextures(2, TextureArray);
	TGA_Texture(TextureArray, "stone.tga", 0);
	TGA_Texture(TextureArray, "normal.tga", 1);
/**/
	//glEnable(GL_TEXTURE_CUBE_MAP);
	glGenTextures(1, &textureCubeMap);
	TGA_Texture_CubeMap(textureCubeMap);

	float amb[]= {0.2f, 0.15f, 0.1f, 1.0f};
	float diff[] = {0.8f, 0.6f, 0.4f, 1.0f};
	float spec[] = {0.8f, 0.8f, 0.8f, 1.0f};	

	float amb1[] = { 0.3f, 0.0f, 0.0f, 1.0f };
	float diff1[] = { 0.8f, 0.1f, 0.1f, 1.0f };
	float spec1[] = { 0.9f, 0.9f, 0.9f, 1.0f };

	float emissive[] = {0.0f, 0.0f, 0.0f, 1.0f};
	float shininess= 100.0f;
	int texcount = 0;

	objId = 0;
	memcpy(mesh[objId].mat.ambient, amb,4*sizeof(float));
	memcpy(mesh[objId].mat.diffuse, diff,4*sizeof(float));
	memcpy(mesh[objId].mat.specular, spec,4*sizeof(float));
	memcpy(mesh[objId].mat.emissive, emissive,4*sizeof(float));
	mesh[objId].mat.shininess = shininess;
	mesh[objId].mat.texCount = texcount;
	createCube();

	objId = 1;
	createSkybox();

	// some GL settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_MULTISAMPLE);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

}

// ------------------------------------------------------------
//
// Main function
//


int main(int argc, char **argv) {

//  GLUT initialization
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA|GLUT_MULTISAMPLE);

	glutInitContextVersion (3, 3);
	glutInitContextProfile (GLUT_CORE_PROFILE );
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE | GLUT_DEBUG);

	glutInitWindowPosition(100,100);
	glutInitWindowSize(WinX, WinY);
	WindowHandle = glutCreateWindow(CAPTION);


//  Callback Registration
	glutDisplayFunc(renderScene);
	glutReshapeFunc(changeSize);
	//glutIdleFunc(renderScene);

//	Mouse and Keyboard Callbacks
	glutKeyboardFunc(processKeys);
	glutMouseFunc(processMouseButtons);
	glutMotionFunc(processMouseMotion);
	glutMouseWheelFunc ( mouseWheel ) ;
	glutTimerFunc(0,timer,0);
	glutTimerFunc(0, refresh, 0);


//	return from main loop
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

//	Init GLEW
	glewExperimental = GL_TRUE;
	glewInit();

	printf ("Vendor: %s\n", glGetString (GL_VENDOR));
	printf ("Renderer: %s\n", glGetString (GL_RENDERER));
	printf ("Version: %s\n", glGetString (GL_VERSION));
	printf ("GLSL: %s\n", glGetString (GL_SHADING_LANGUAGE_VERSION));
/**/
	if (!setupShaders()) {
		return(1);
	}

	printf("Program ID: %d\n\n", shader.getProgramIndex());
/**/
	if (!setupSkyboxShaders()) {
		return(1);
	}

	printf("Program ID: %d\n\n", shaderSkybox.getProgramIndex());

	init();

	//  GLUT main loop
	glutMainLoop();

	return(0);

}

