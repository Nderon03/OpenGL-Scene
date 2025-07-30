
#include <GL/gl3w.h>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>

#include "camera.h"
#include "error.h"
#include "file.h"
#include "shader.h"
#include "shadow.h"
#include "object.h"
#include "texture.h"

//light direction variable here
glm::vec3 lightDirection = glm::vec3(0.1f, -.81f, -.61f);
glm::vec3 lightPos = glm::vec3(2.f, 6.f, 7.f);

SCamera Camera;
SCamera Camera2; // Second camera
SCamera* activeCamera = &Camera;

float vertices[] =
{
	// near face
	// pos                // uv               // normal
	-0.5f, -0.5f, -0.5f,   0.0f, 0.0f, 0,     0, 0, -1,
	0.5f, -0.5f, -0.5f,    1.0f, 0.0f, 0,     0, 0, -1,
	0.5f,  0.5f, -0.5f,    1.0f, 1.0f, 0,     0, 0, -1,
	0.5f,  0.5f, -0.5f,    1.0f, 1.0f, 0,     0, 0, -1,
	-0.5f,  0.5f, -0.5f,   0.0f, 1.0f, 0,     0, 0, -1,
	-0.5f, -0.5f, -0.5f,   0.0f, 0.0f, 0,     0, 0, -1,

	// far face
	// pos                // uv               // normal
	-0.8f, -0.8f, 0.8f,    0.0f, 0.0f, 0,     0, 0, 1,
	0.8f, -0.8f, 0.8f,     1.0f, 0.0f, 0,     0, 0, 1,
	0.8f,  0.8f, 0.8f,     1.0f, 1.0f, 0,     0, 0, 1,
	0.8f,  0.8f, 0.8f,     1.0f, 1.0f, 0,     0, 0, 1,
	-0.8f,  0.8f, 0.8f,    0.0f, 1.0f, 0,     0, 0, 1,
	-0.8f, -0.8f, 0.8f,    0.0f, 0.0f, 0,     0, 0, 1,

	// left face
	// pos                // uv               // normal
	-0.5f,  0.5f, -0.5f,   0.0f, 0.0f, 0,    -1, 0, 0,
	-0.8f,  0.8f,  0.8f,   1.0f, 0.0f, 0,    -1, 0, 0,
	-0.8f, -0.8f,  0.8f,   1.0f, 1.0f, 0,    -1, 0, 0,
	-0.8f, -0.8f,  0.8f,   1.0f, 1.0f, 0,    -1, 0, 0,
	-0.5f, -0.5f, -0.5f,   0.0f, 1.0f, 0,    -1, 0, 0,
	-0.5f,  0.5f, -0.5f,   0.0f, 0.0f, 0,    -1, 0, 0,

	// right face
	// pos                // uv               // normal
	0.5f,  0.5f, -0.5f,    1.0f, 0.0f, 0,     1, 0, 0,
	0.8f,  0.8f,  0.8f,    0.0f, 0.0f, 0,     1, 0, 0,
	0.8f, -0.8f,  0.8f,    0.0f, 1.0f, 0,     1, 0, 0,
	0.8f, -0.8f,  0.8f,    0.0f, 1.0f, 0,     1, 0, 0,
	0.5f, -0.5f, -0.5f,    1.0f, 1.0f, 0,     1, 0, 0,
	0.5f,  0.5f, -0.5f,    1.0f, 0.0f, 0,     1, 0, 0,

	// bottom face
	// pos                // uv               // normal
	-0.5f, -0.5f, -0.5f,   0.0f, 0.0f, 0,     0, -1, 0,
	0.5f, -0.5f, -0.5f,    1.0f, 0.0f, 0,     0, -1, 0,
	0.8f, -0.8f,  0.8f,    1.0f, 1.0f, 0,     0, -1, 0,
	0.8f, -0.8f,  0.8f,    1.0f, 1.0f, 0,     0, -1, 0,
	-0.8f, -0.8f,  0.8f,   0.0f, 1.0f, 0,     0, -1, 0,
	-0.5f, -0.5f, -0.5f,   0.0f, 0.0f, 0,     0, -1, 0,

	// top face
	// pos                // uv               // normal
	-0.5f,  0.5f, -0.5f,   0.0f, 0.0f, 0,     0, 1, 0,
	0.5f,  0.5f, -0.5f,    1.0f, 0.0f, 0,     0, 1, 0,
	0.8f,  0.8f,  0.8f,    1.0f, 1.0f, 0,     0, 1, 0,
	0.8f,  0.8f,  0.8f,    1.0f, 1.0f, 0,     0, 1, 0,
	-0.8f,  0.8f,  0.8f,   0.0f, 1.0f, 0,     0, 1, 0,
	-0.5f,  0.5f, -0.5f,   0.0f, 0.0f, 0,     0, 1, 0
};

float floorVertices[] =
{
	//back face
	//pos                    //uv               //normal
	-0.5f, -0.5f, -0.5f,      0.0f, 0.0f, 0.0f,   0, 0, -1,
	 0.5f, -0.5f, -0.5f,      64.0f, 0.0f, 0.0f,   0, 0, -1,
	 0.5f,  0.5f, -0.5f,      64.0f, 64.0f, 0.0f,   0, 0, -1,
	 0.5f,  0.5f, -0.5f,      64.0f, 64.0f, 0.0f,   0, 0, -1,
	-0.5f,  0.5f, -0.5f,      0.0f, 64.0f, 0.0f,   0, 0, -1,
	-0.5f, -0.5f, -0.5f,      0.0f, 0.0f, 0.0f,   0, 0, -1,

	//front face
	-0.5f, -0.5f,  0.5f,      0.0f, 0.0f, 0.0f,   0, 0, 1,
	 0.5f, -0.5f,  0.5f,      64.0f, 0.0f, 0.0f,   0, 0, 1,
	 0.5f,  0.5f,  0.5f,      64.0f, 64.0f, 0.0f,   0, 0, 1,
	 0.5f,  0.5f,  0.5f,      64.0f, 64.0f, 0.0f,   0, 0, 1,
	-0.5f,  0.5f,  0.5f,      0.0f, 64.0f, 0.0f,   0, 0, 1,
	-0.5f, -0.5f,  0.5f,      0.0f, 0.0f, 0.0f,   0, 0, 1,

	//left face
	-0.5f,  0.5f,  0.5f,      0.0f, 0.0f, 0.0f,   -1, 0, 0,
	-0.5f,  0.5f, -0.5f,      64.0f, 0.0f, 0.0f,   -1, 0, 0,
	-0.5f, -0.5f, -0.5f,      64.0f, 64.0f, 0.0f,   -1, 0, 0,
	-0.5f, -0.5f, -0.5f,      64.0f, 64.0f, 0.0f,   -1, 0, 0,
	-0.5f, -0.5f,  0.5f,      0.0f, 64.0f, 0.0f,   -1, 0, 0,
	-0.5f,  0.5f,  0.5f,      0.0f, 0.0f, 0.0f,   -1, 0, 0,

	//right face
	 0.5f,  0.5f,  0.5f,      0.0f, 0.0f, 0.0f,   1, 0, 0,
	 0.5f,  0.5f, -0.5f,      64.0f, 0.0f, 0.0f,   1, 0, 0,
	 0.5f, -0.5f, -0.5f,      64.0f, 64.0f, 0.0f,   1, 0, 0,
	 0.5f, -0.5f, -0.5f,      64.0f, 64.0f, 0.0f,   1, 0, 0,
	 0.5f, -0.5f,  0.5f,      0.0f, 64.0f, 0.0f,   1, 0, 0,
	 0.5f,  0.5f,  0.5f,      0.0f, 0.0f, 0.0f,   1, 0, 0,

	 //bottom face
	 -0.5f, -0.5f, -0.5f,      0.0f, 0.0f, 0.0f,   0, -1, 0,
	  0.5f, -0.5f, -0.5f,      64.0f, 0.0f, 0.0f,   0, -1, 0,
	  0.5f, -0.5f,  0.5f,      64.0f, 64.0f, 0.0f,   0, -1, 0,
	  0.5f, -0.5f,  0.5f,      64.0f, 64.0f, 0.0f,   0, -1, 0,
	 -0.5f, -0.5f,  0.5f,      0.0f, 64.0f, 0.0f,   0, -1, 0,
	 -0.5f, -0.5f, -0.5f,      0.0f, 0.0f, 0.0f,   0, -1, 0,

	 //top face
	 -0.5f,  0.5f, -0.5f,      0.0f, 0.0f, 0.0f,   0, 1, 0,
	  0.5f,  0.5f, -0.5f,      64.0f, 0.0f, 0.0f,   0, 1, 0,
	  0.5f,  0.5f,  0.5f,      64.0f, 64.0f, 0.0f,   0, 1, 0,
	  0.5f,  0.5f,  0.5f,      64.0f, 64.0f, 0.0f,   0, 1, 0,
	 -0.5f,  0.5f,  0.5f,      0.0f,64.0f, 0.0f,   0, 1, 0,
	 -0.5f,  0.5f, -0.5f,      0.0f, 0.0f, 0.0f,   0, 1, 0
};

#define NUM_BUFFERS 2
#define NUM_VAOS 2
GLuint Buffers[NUM_BUFFERS];
GLuint VAOs[NUM_VAOS];

#define WIDTH 1024
#define HEIGHT 768
#define SH_MAP_WIDTH 2048
#define SH_MAP_HEIGHT 2048


// Initialise floor texture that will be used
GLuint tex;
GLuint tex2;
void initTexture() {
	tex = CreateTexture("ground.jpg");
	tex2 = CreateTexture("pattern.jpg");
}

// Draw the floor
void drawFloor(unsigned int shaderProgram)
{
	glBindVertexArray(VAOs[1]);
	glBindBuffer(GL_ARRAY_BUFFER, 1);

	glActiveTexture(GL_TEXTURE1);

	glUniform1i(glGetUniformLocation(shaderProgram, "Texture"), 1);

	//floor
	glm::mat4 model = glm::mat4(1.f);
	model = glm::translate(model, glm::vec3(0, -3, 0));
	model = glm::scale(model, glm::vec3(100, 0.1, 100));
	// Assign the texture
	glBindTexture(GL_TEXTURE_2D, tex);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glDrawArrays(GL_TRIANGLES, 0, 36);

}

// Draw the frustums
void drawFrustums(unsigned int shaderProgram)
{
	glBindVertexArray(VAOs[0]);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glActiveTexture(GL_TEXTURE1);

	glUniform1i(glGetUniformLocation(shaderProgram, "Texture"), 1);

	//frustums
	for (int i = -1; i < 2; i++)
	{
		for (int j = -1; j < 2; j++)
		{
			for (int k = -1; k < 2; k++)
			{
				glm::mat4 model = glm::mat4(1.f);
				float xOffset = 2.0f * sin(glfwGetTime());

				model = glm::translate(model, glm::vec3(float(i * 2) + xOffset, float(j * 2), float(k * 2)));
				model = glm::rotate(model, (float)glfwGetTime() / 2.f, glm::vec3(0.0f, 1.0f, .0f));
				float scaleFactor = 0.6f + 0.4f * sin(glfwGetTime());
				model = glm::scale(model, glm::vec3(scaleFactor, scaleFactor, scaleFactor));
				model = glm::rotate(model, (float)glfwGetTime() / 2.f, glm::vec3(0.0f, 1.0f, .0f));
				// Bind to its own texture
				glBindTexture(GL_TEXTURE_2D, tex2);
				glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
				glDrawArrays(GL_TRIANGLES, 0, 36);
			}
		}
	}
}

vector<Object> objs;
float treeRotationAngle = 0.0f; // Tree angle
float treeRotationSpeed = 1.0f; // How quick tree spins
// Render tree
void drawTree(unsigned int shaderProgram)
{

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glActiveTexture(GL_TEXTURE1);
	glUniform1i(glGetUniformLocation(shaderProgram, "Texture"), 1);

	// Define model matrix for positioning and scaling
	glm::mat4 model = glm::mat4(1.f);
	model = glm::translate(model, glm::vec3(8.f, -2.8f, -5.f));
	model = glm::scale(model, glm::vec3(0.01, 0.01, 0.01));

	model = glm::rotate(model, glm::radians(treeRotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	// Iterate over each object in the vector
	for (int i = 0; i < objs.size(); i++)
	{
		// Bind texture
		glBindTexture(GL_TEXTURE_2D, objs[i].texture);

		// Bind VAO and VBO
		glBindVertexArray(objs[i].VAO);

		// Set model matrix uniform
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

		// Draw the object
		glDrawArrays(GL_TRIANGLES, 0, (objs[i].tris.size() * 3));
	}

}

// Set up the tree's resources
void createTree()
{
	for (int i = 0; i < objs.size(); i++)
	{
		objs[i].texture = CreateTexture(objs[i].mtl.fil_name);

		glGenVertexArrays(1, &objs[i].VAO);
		glGenBuffers(1, &objs[i].VBO);

		glBindVertexArray(objs[i].VAO);
		glBindBuffer(GL_ARRAY_BUFFER, objs[i].VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * (objs[i].tris.size() * 27), objs[i].tris.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, (9 * sizeof(float)), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, (9 * sizeof(float)), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, (9 * sizeof(float)), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);
	}
}


bool firstMouse = true; // Check if mouse clicked
// Position of mouse
float lastX = WIDTH / 2.0f;
float lastY = HEIGHT / 2.0f;

// Handle mouse event
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	activeCamera->Yaw += xoffset;
	activeCamera->Pitch += yoffset;

	// make sure that when pitch is out of bounds, screen doesn't get flipped
	if (activeCamera->Pitch > 89.0f)
		activeCamera->Pitch = 89.0f;
	if (activeCamera->Pitch < -89.0f)
		activeCamera->Pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(activeCamera->Yaw)) * cos(glm::radians(activeCamera->Pitch));
	front.y = sin(glm::radians(activeCamera->Pitch));
	front.z = sin(glm::radians(activeCamera->Yaw)) * cos(glm::radians(activeCamera->Pitch));
	activeCamera->Front = glm::normalize(front);
}


bool c_key_pressed = false; // click 'c' to change camera
float temp; // save cam_dist
// Define light in use
bool spotlightActive = false;
bool positionalActive = false;
// Handle keyboard events
void processKeyboard(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	float x_offset = 0.f;
	float y_offset = 0.f;
	bool cam_changed = false;


	// Check if the 'C' key is pressed
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS && !c_key_pressed) {
		// Toggle active camera when 'C' key is pressed for the first time
		if (activeCamera == &Camera) {
			activeCamera = &Camera2;
			temp = cam_dist;
			cam_dist = 20.f;
		}
		else {
			activeCamera = &Camera;
			cam_dist = temp;
		}
		// Mark the 'C' key as pressed
		c_key_pressed = true;
	}
	else if (glfwGetKey(window, GLFW_KEY_C) == GLFW_RELEASE) {
		// Mark the 'C' key as released when it is released
		c_key_pressed = false;
	}

	// Handle lights
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		lightDirection = activeCamera->Front;
		lightPos = activeCamera->Position;
		spotlightActive = false;
		positionalActive = false;
	}
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
		lightDirection = activeCamera->Front;
		lightPos = activeCamera->Position;
		spotlightActive = true;
		positionalActive = false;
	}
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
		lightDirection = activeCamera->Front;
		lightPos = activeCamera->Position;
		spotlightActive = false;
		positionalActive = true;
	}

	if (activeCamera == &Camera) {
		// Movement
		float cameraSpeed = 0.5f;
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		{
			activeCamera->Position += glm::normalize(glm::cross(activeCamera->Front, activeCamera->Up)) * cameraSpeed;
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		{
			activeCamera->Position -= glm::normalize(glm::cross(activeCamera->Front, activeCamera->Up)) * cameraSpeed;
		}

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		{
			activeCamera->Position += cameraSpeed * activeCamera->Front;
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		{
			activeCamera->Position -= cameraSpeed * activeCamera->Front;
		}
	}

	// Which camera
	if (activeCamera == &Camera2) {
		x_offset = .01f;
		y_offset = 0.f;
		cam_changed = true;
	}

	// Handle tree
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		treeRotationAngle -= treeRotationSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		treeRotationAngle += treeRotationSpeed;
	}

	// Apply changes
	if (cam_changed)
	{
		MoveAndOrientCamera(*activeCamera, glm::vec3(0.f, 0.f, 0.f), cam_dist, x_offset, y_offset);
	}
}

void SizeCallback(GLFWwindow* window, int w, int h)
{
	glViewport(0, 0, w, h);
}

void generateDepthMap(unsigned int shadowShaderProgram, ShadowStruct shadow, glm::mat4 projectedLightSpaceMatrix)
{
	glUseProgram(shadowShaderProgram);
	glUniformMatrix4fv(glGetUniformLocation(shadowShaderProgram, "projectedLightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(projectedLightSpaceMatrix));
	glViewport(0, 0, SH_MAP_WIDTH, SH_MAP_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, shadow.FBO);
	glClear(GL_DEPTH_BUFFER_BIT);

	drawFloor(shadowShaderProgram);
	drawFrustums(shadowShaderProgram);
	drawTree(shadowShaderProgram);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void renderWithShadow(unsigned int renderShaderProgram, ShadowStruct shadow, glm::mat4 projectedLightSpaceMatrix)
{
	glViewport(0, 0, WIDTH, HEIGHT);

	static const GLfloat bgd[] = { .8f, .8f, .8f, 1.f }; //background colour
	glClearBufferfv(GL_COLOR, 0, bgd);
	glClear(GL_DEPTH_BUFFER_BIT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glUseProgram(renderShaderProgram);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, shadow.Texture);

	glUniform1i(glGetUniformLocation(renderShaderProgram, "shadowMap"), 0);


	glUniformMatrix4fv(glGetUniformLocation(renderShaderProgram, "projectedLightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(projectedLightSpaceMatrix));
	glUniform3f(glGetUniformLocation(renderShaderProgram, "lightDirection"), lightDirection.x, lightDirection.y, lightDirection.z);
	glUniform3f(glGetUniformLocation(renderShaderProgram, "lightColour"), 1.f, 1.f, 1.f); //light colour
	glUniform3f(glGetUniformLocation(renderShaderProgram, "camPos"), activeCamera->Position.x, activeCamera->Position.y, activeCamera->Position.z);
	glUniform3f(glGetUniformLocation(renderShaderProgram, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

	glm::mat4 view = glm::mat4(1.f);
	view = glm::lookAt(activeCamera->Position, activeCamera->Position + activeCamera->Front, activeCamera->Up);
	glUniformMatrix4fv(glGetUniformLocation(renderShaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

	glm::mat4 projection = glm::mat4(1.f);
	projection = glm::perspective(glm::radians(45.f), (float)WIDTH / (float)HEIGHT, .01f, 100.f);
	glUniformMatrix4fv(glGetUniformLocation(renderShaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	GLuint spotlightActiveLocation = glGetUniformLocation(renderShaderProgram, "spotlightActive");
	glUniform1i(spotlightActiveLocation, spotlightActive);

	GLuint positionalActiveLocation = glGetUniformLocation(renderShaderProgram, "positionalActive");
	glUniform1i(positionalActiveLocation, positionalActive);

	drawFloor(renderShaderProgram);
	drawFrustums(renderShaderProgram);
	drawTree(renderShaderProgram);
}

int main(int argc, char** argv)
{
	glfwInit();

	glfwWindowHint(GLFW_SAMPLES, 16); // Antialiasing
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Lighting", NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSetWindowSizeCallback(window, SizeCallback);

	gl3wInit();

	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(DebugCallback, 0);

	ShadowStruct shadow = setup_shadowmap(SH_MAP_WIDTH, SH_MAP_HEIGHT);

	// Declare shaders in use
	GLuint program = CompileShader("phong.vert", "phong.frag");
	GLuint shadow_program = CompileShader("shadow.vert", "shadow.frag");
	//GLuint texture_program = CompileShader("textured.vert", "textured.frag");

	// Initialise camera
	InitCamera(Camera);
	InitCamera(Camera2);
	cam_dist = 20.f;
	MoveAndOrientCamera(Camera, glm::vec3(0, 0, 0), cam_dist, 0.f, 0.f);
	MoveAndOrientCamera(Camera2, glm::vec3(0, 0, 0), cam_dist, 0.f, 0.f);

	// Load tree object
	obj_parse("objs/white_oak/white_oak.obj", &objs);
	createTree();
	initTexture();

	// Handle VAOs and Buffers
	glCreateBuffers(NUM_BUFFERS, Buffers);
	glNamedBufferStorage(Buffers[0], sizeof(vertices), vertices, 0);
	glGenVertexArrays(NUM_VAOS, VAOs);
	glBindVertexArray(VAOs[0]);
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, (9 * sizeof(float)), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, (9 * sizeof(float)), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, (9 * sizeof(float)), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glNamedBufferStorage(Buffers[1], sizeof(floorVertices), floorVertices, 0);
	glBindVertexArray(VAOs[1]);
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[1]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, (9 * sizeof(float)), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, (9 * sizeof(float)), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, (9 * sizeof(float)), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glEnable(GL_DEPTH_TEST); // Enable depth testing
	glEnable(GL_BLEND); // Enable blending
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	while (!glfwWindowShouldClose(window))
	{

		float near_plane = 1.0f, far_plane = 70.5f;
		glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
		glm::mat4 lightView = glm::lookAt(lightPos, lightPos + lightDirection, glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 projectedLightSpaceMatrix = lightProjection * lightView;

		generateDepthMap(shadow_program, shadow, projectedLightSpaceMatrix);

		renderWithShadow(program, shadow, projectedLightSpaceMatrix);

		glfwSwapBuffers(window);

		glfwPollEvents();
		processKeyboard(window);
		if (activeCamera == &Camera) {
			mouse_callback(window, lastX, lastY);
		}
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}