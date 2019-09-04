
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Globe.h"
#include "GlobeEffect.h"
#include "Type.h"
#include "parser.h"

#include <iostream>
#include <fstream>
#include <vector>

#define PI 3.141592
#define DIMENSION DIMENSION3

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos);
void cursor_enter_callback(GLFWwindow* window, int status);
void processInput(GLFWwindow* window);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

// settings
const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 800;

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float deltaTime = 0.0f;
float lastFrameTime = 0.0f;

bool firstMouse = true;
float yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch = 0.0f;
float lastX = 800.0f / 2.0;
float lastY = 600.0f / 2.0;
float fov = 45.0f;


int main()
{

	// glfw: initialize and configure
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

	// glfw window creation
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "GLOBE", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, cursor_pos_callback);
	glfwSetCursorEnterCallback(window, cursor_enter_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	// glad: load all OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	glEnable(GL_DEPTH_TEST);

	// build and compile our shader program
	Shader globeShader("globe.vs", "globe.fs");
	Shader roiShader("roi.vs", "roi.fs");

	// make sphere vertex // 
	Globe globe(1.0f, DIMENSION);
	globe.CreateGlobe();//(vertex, indice, globeIndiceCnt);


	// make surface effect //

	//int roiSize = 4;
	//float lat[roiSize] = { 38.0f, 38.0f, 34.0f, 34.0f};
	//float lng[roiSize] = { 126.0f, 130.0f, 130.0f, 126.0f};
	std::vector<float> lat;
	std::vector<float> lng;
	GeoJsonParser::GeoJson2LatLon("custom.geo.china.json", lat, lng);
	int roiSize = lat.size();

	float cX = 0, cY = 0, cZ = 0;

	GlobeEffect globeEffect(1.015f, DIMENSION);
	globeEffect.GetCentorVector(&lat[0], &lng[0], lat.size(), cX, cY, cZ);
	globeEffect.PushCentorPos(cX, cY, cZ);
	globeEffect.CreateRoi(&lat[0], &lng[0], roiSize);
	roiSize = globeEffect.vertices_.size();

// make VAO and bind buffers //

	// sphere VAO
	unsigned int VAO, VBO, EBO;
	globe.BindGlobeBuffer(VAO, VBO, EBO);

	// surface effect VAO
	unsigned int VAO2, VBO2, EBO2;
	globeEffect.BindRoiBuffer(VAO2, VBO2, EBO2);

// -- //


	// load and create a texture //
	//--------------------------------------------------------------------------------//

	// texture 1
	unsigned int texture1;
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// load image, create texture and generate mipmaps
	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	unsigned char* data = stbi_load("globee.jpg", &width, &height, &nrChannels, 0);
	//unsigned char* data = stbi_load("world_map_nasa_3x5400x2700.jpg", &width, &height, &nrChannels, 0);

	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

#if 1
	// texture 2
	unsigned int texture2;
	glGenTextures(1, &texture2);
	glBindTexture(GL_TEXTURE_2D, texture2);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps

	data = stbi_load("world_map_nasa_3x5400x2700.jpg", &width, &height, &nrChannels, 0);
	if (data)
	{
		// note that the awesomeface.png has transparency and thus an alpha channel, so make sure to tell OpenGL the data type is of GL_RGBA
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
#endif
	//--------------------------------------------------------------------------------//


	// set blend mode(alpha)
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);

	// rendering loop //
	while (!glfwWindowShouldClose(window))
	{
		// get input
		processInput(window);

		// init
		//glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer

		 // bind textures on corresponding texture units
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);

		// activate shader
		globeShader.use();

		// set texture idx(uniform)
		// tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
		globeShader.setInt("texture1", 0);
		globeShader.setInt("texture2", 1);

		// set 
		//glm::vec2 u_resolution(SCR_WIDTH/SCR_WIDTH, SCR_HEIGHT/SCR_WIDTH);
		//globeShader.setVec2("u_resolution", u_resolution);

		// create transformations
		glm::mat4 view = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
		glm::mat4 projection = glm::mat4(1.0f);
		glm::mat4 model = glm::mat4(1.0f);

		projection = glm::perspective(glm::radians(fov), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.5f));
		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		//model = glm::translate(model, glm::vec3(0.0f, 0.0f, -3.0f));
		if (DIMENSION == DIMENSION3) {
			model = glm::rotate(model, glm::radians(60.0f), glm::vec3(-1.0f, 0.5f, 0.0f));
			float rotationAngle = 10.0f * glfwGetTime();
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
		}
		else {
			model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			model = glm::rotate(model, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
		}

		
		// pass transformation matrices to the shader
		globeShader.setMat4("projection", projection); // note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
		globeShader.setMat4("view", view);
		globeShader.setMat4("model", model);

#if 1
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, globe.indiceCnt_*3, GL_UNSIGNED_INT, (void*)0);
		glBindVertexArray(0);
#endif

		roiShader.use();
		roiShader.setInt("texture1", 0);
		roiShader.setInt("texture2", 1);
		roiShader.setMat4("projection", projection); 
		roiShader.setMat4("view", view);
		roiShader.setMat4("model", model);
#if 1
		glBindVertexArray(VAO2);
		//glDrawElements(GL_TRIANGLES, globeEffect.indiceCnt_*3, GL_UNSIGNED_INT, (void*)0);
		glDrawElements(GL_TRIANGLES, (roiSize-2)*3, GL_UNSIGNED_INT, (void*)0);
		glBindVertexArray(0);
#endif


		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	glDeleteVertexArrays(1, &VAO);
	glDeleteVertexArrays(1, &VAO2);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &VBO2);
	glDeleteBuffers(1, &EBO);
	glDeleteBuffers(1, &EBO2);

	// glfw: terminate, clearing all previously allocated GLFW resources.
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	float currentFrameTime = glfwGetTime();
	deltaTime = currentFrameTime - lastFrameTime;
	lastFrameTime = currentFrameTime;

	float cameraSpeed = 2.0f*deltaTime; // adjust accordingly
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}


void cursor_enter_callback(GLFWwindow* window, int status)
{
	if (status == false) {
		firstMouse = true;
	}
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		
	}
}

void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos)
{
	double x, y;
	glfwGetCursorPos(window, &x, &y);
	//printf("x:%lf, y:%lf\n", x, y);

	if (firstMouse)
	{
		//lastX = xpos;
		//lastY = ypos;
		lastX = SCR_WIDTH/2;
		lastY = SCR_HEIGHT/2;
		glfwSetCursorPos(window, lastX, lastY);
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.05;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (fov >= 1.0f && fov <= 45.0f)
		fov -= yoffset;
	if (fov <= 1.0f)
		fov = 1.0f;
	if (fov >= 45.0f)
		fov = 45.0f;
}
