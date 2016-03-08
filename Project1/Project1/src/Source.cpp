#include <iostream>
#include <vector>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// Other Libs
#include <SOIL.h>


// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Other includes
#include "Shader.h"
#include "Camera.h"
#include "Model.h"


// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset); 
void mouse_button_callback(GLFWwindow* window, int button, int action, int mode);
void do_movement();

// Window dimensions
const GLuint WIDTH = 1280, HEIGHT = 800;
const std::string WINDOW_TITLE = "P3D 2016";

// Camera
Camera camera(glm::vec3(0.0f, 1.5f, 3.0f), glm::vec3(0.0f, 1.0f, 0.0f), YAW, -20.0f);

// Zoom 
bool firstMouse = true;
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
GLfloat fov = 45.0f;
bool keys[1024];

// Deltatime
GLfloat deltaTime = 0.0f;	// Time between current frame and last frame
GLfloat lastFrame = 0.0f;  	// Time of last frame

// Light
glm::vec3 lightPos(1.2f, 3.0f, -0.0f);

bool bumpMapActive = false;
bool mappingMode = true;

std::vector<Model> models;
int currentModelIndex = 0;
std::vector<int> skyboxTextures;
int currentSkyboxIndex = 0;

std::vector<Shader> shaders;
int currentShaderIndex = 0;

// The MAIN function, from here we start the application and run the game loop
int main()
{
	// Init GLFW
	glfwInit();
	// Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, WINDOW_TITLE.c_str(), nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Set the required callback functions
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	// GLFW Options, locks cursor inside window
	
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	glewInit();

	// Define the viewport dimensions
	glViewport(0, 0, WIDTH, HEIGHT);

	
	// Build and compile our shader program

	// texture and phong
	Shader simpleShader("shaders/simple.vs", "shaders/simple.frag");	

	// texture, phong, normal
	Shader normalShader("shaders/bump.vs", "shaders/bump.frag");

	// reflect only
	Shader reflectOnlyShader("shaders/reflect_only.vs", "shaders/reflect_only.frag");

	// reclect, texture, phong
	Shader reflectShader("shaders/reflect.vs", "shaders/reflect.frag");

	// phong, texture, normal, reflect
	Shader mixShader("shaders/mix.vs", "shaders/mix.frag");

	// sphere map
	Shader sphereMapShader("shaders/sphere.vs", "shaders/sphere.frag");

	// add to vector
	shaders.push_back(simpleShader);
	shaders.push_back(normalShader);
	shaders.push_back(reflectOnlyShader);
	shaders.push_back(reflectShader);
	shaders.push_back(mixShader);
	shaders.push_back(sphereMapShader);

	Shader lampShader("shaders/lamp.vs", "shaders/lamp.frag");
	Shader skyboxShader("shaders/skybox.vs", "shaders/skybox.frag");
	
	Model cubeModel("resources/models/cube/cube.obj");
	Model lampModel("resources/models/lamp/lamp.obj");	
	Model sphereModel("resources/models/sphere/sphere.obj");	
	Model torusModel("resources/models/torus/torus.obj");	
	Model nanosuitModel("resources/models/nanosuit/nanosuit.obj");	

	glm::mat4 model;	
	model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f)); // Translate it down a bit so it's at the center of the scene
	model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));	// It's a bit too big for our scene, so scale it down
	nanosuitModel.setModelMatrix(model);

	models.push_back(cubeModel);
	models.push_back(torusModel);
	models.push_back(sphereModel);
	models.push_back(nanosuitModel);
	// TODO import other formats

	// Draw in wireframe
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// Point light positions	
	glm::vec3 pointLight(-1.7f, 1.2f, 1.0f);

	// load skybox
	// Setup skybox VAO
	GLuint skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glBindVertexArray(0);

	vector<const GLchar*> faces;
	
	faces.push_back("resources/textures/cubemap/posx.tga");
	faces.push_back("resources/textures/cubemap/negx.tga");
	faces.push_back("resources/textures/cubemap/posy.tga");
	faces.push_back("resources/textures/cubemap/negy.tga");
	faces.push_back("resources/textures/cubemap/posz.tga");
	faces.push_back("resources/textures/cubemap/negz.tga");
	GLuint cubemapTexture = loadCubemap(faces);
	

	faces.clear();
	faces.push_back("resources/textures/waterbox/posx.jpg");
	faces.push_back("resources/textures/waterbox/negx.jpg");
	faces.push_back("resources/textures/waterbox/posy.jpg");
	faces.push_back("resources/textures/waterbox/negy.jpg");
	faces.push_back("resources/textures/waterbox/posz.jpg");
	faces.push_back("resources/textures/waterbox/negz.jpg");
	GLuint watermapTexture = loadCubemap(faces);

	skyboxTextures.push_back(watermapTexture);
	skyboxTextures.push_back(cubemapTexture);
	

	glEnable(GL_DEPTH_TEST);

	GLint sphereTexture = TextureFromFile("spheremap.JPG", "resources/textures");

	// Game loop
	while (!glfwWindowShouldClose(window))
	{
		// Calculate deltatime of current frame
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
		glfwPollEvents();
		do_movement();

		// Render
		// Clear the colorbuffer

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// set view matrix
		glm::mat4 view = camera.GetViewMatrix();		
		// set projection matrix
		glm::mat4 projection = glm::perspective(camera.Zoom, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);


		Shader shader = shaders[currentShaderIndex];

		// Activate shader
		shader.Use();

		// Set skybox
		/*
		glActiveTexture(GL_TEXTURE0+3);
		glUniform1i(glGetUniformLocation(shader.Program, "skybox"), 3);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTextures[currentSkyboxIndex]);
		*/
		glActiveTexture(GL_TEXTURE0 + 3);
		glUniform1i(glGetUniformLocation(shader.Program, "sphereMap"), 3);
		glBindTexture(GL_TEXTURE_2D, sphereTexture);
		
		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		
		glUniform3f(glGetUniformLocation(shader.Program, "viewPos"), camera.Position.x, camera.Position.y, camera.Position.z);
		pointLight.x = 2*sin(glfwGetTime());		
		glUniform3f(glGetUniformLocation(shader.Program, "lightPos"), pointLight.x, pointLight.y, pointLight.z);
		models[currentModelIndex].Draw(shader);

		// draw lamp
		model = glm::mat4();
		model = glm::translate(model, pointLight);
		model = glm::scale(model, glm::vec3(0.2));	
		lampModel.setModelMatrix(model);
		lampShader.Use();
		glUniformMatrix4fv(glGetUniformLocation(lampShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(glGetUniformLocation(lampShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(lampShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		lampModel.Draw(lampShader);
		
		// draw skybox
		glDepthFunc(GL_LEQUAL);  // Change depth function so depth test passes when values are equal to depth buffer's content
		skyboxShader.Use();
		glm::mat4 skyboxView = glm::mat4(glm::mat3(camera.GetViewMatrix()));
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader.Program, "view"), 1, GL_FALSE, glm::value_ptr(skyboxView));
		glUniformMatrix4fv(glGetUniformLocation(skyboxShader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(glGetUniformLocation(skyboxShader.Program, "skybox"), 0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTextures[currentSkyboxIndex]);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS); // Set depth function back to default	

		// Swap the screen buffers
		glfwSwapBuffers(window);
	}
	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
	return 0;
}


#pragma region "User input"

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}	else if (key == GLFW_KEY_O && action == GLFW_PRESS) {
		currentModelIndex = (currentModelIndex + 1) % models.size();
	} else if (key == GLFW_KEY_C && action == GLFW_PRESS) {
		currentSkyboxIndex = (currentSkyboxIndex + 1) % skyboxTextures.size();
	} else if (key == GLFW_KEY_M && action == GLFW_PRESS) {
		currentShaderIndex = (currentShaderIndex + 1) % shaders.size();
	}
	else if (key == GLFW_KEY_1 && action == GLFW_PRESS) {	
		glm::mat4 m = models[currentModelIndex].getModelMatrix();
		m = glm::rotate(m, 10.0f, glm::vec3(1, 0, 0));
		models[currentModelIndex].setModelMatrix(m);

	} else if (key == GLFW_KEY_2 && action == GLFW_PRESS) {
		glm::mat4 m = models[currentModelIndex].getModelMatrix();
		m = glm::rotate(m, 10.0f, glm::vec3(0, 1, 0));
		models[currentModelIndex].setModelMatrix(m);

	}	else if (key == GLFW_KEY_3 && action == GLFW_PRESS) {
		glm::mat4 m = models[currentModelIndex].getModelMatrix();
		m = glm::rotate(m, 10.0f, glm::vec3(0, 0, 1));
		models[currentModelIndex].setModelMatrix(m);
	}

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
			keys[key] = false;
	}
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mode) {
	/*
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		if (action == GLFW_PRESS) {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		else
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
	}
	*/
}

void do_movement()
{
	// Camera controls
	if (keys[GLFW_KEY_W])
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (keys[GLFW_KEY_S])
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (keys[GLFW_KEY_A])
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (keys[GLFW_KEY_D])
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if (keys[GLFW_KEY_SPACE])
		camera.ProcessKeyboard(UP, deltaTime);
	if (keys[GLFW_KEY_Z])
		camera.ProcessKeyboard(DOWN, deltaTime);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = lastY - ypos; // Reversed since y-coordinates go from bottom to left
	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

#pragma endregion