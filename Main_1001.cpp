

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"
#include "ft2build.h"
#include FT_FREETYPE_H  

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Globe.h"
#include "geojson.hh"
#include "GlobeEffect.h"
#include "Type.h"
#include "YText.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <map>

#define PI 3.141592
#define DIMENSION DIMENSION3

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos);
void cursor_enter_callback(GLFWwindow* window, int status);
void processInput(GLFWwindow* window);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

float cameraX = 0.0f;
float cameraY = 1.0f;
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
	//glfwSetCursorPosCallback(window, cursor_pos_callback);
	//glfwSetCursorEnterCallback(window, cursor_enter_callback);
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

	std::vector<float> lat;
	std::vector<float> lng;

	float cX = 0, cY = 0, cZ = 0;

	GlobeEffect globeEffect(1.005f, DIMENSION);
	globeEffect.ConvertGeoJson("border_50m/", COUNTRYALL);
	globeEffect.MakeAreaObjectList();

// make VAO and bind buffers //

	// sphere VAO
	unsigned int VAO, VBO, EBO;
	globe.BindGlobeBuffer(VAO, VBO, EBO);

	// surface effect VAO
	unsigned int VAO2, VBO2, EBO2;
	globeEffect.BindAreaBuffer(VAO2, VBO2, EBO2, 10000);

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
	//unsigned char* data = stbi_load("globee2.png", &width, &height, &nrChannels, 0);
	unsigned char* data = stbi_load("world_map_nasa_3x5400x2700.jpg", &width, &height, &nrChannels, 0);

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
	//data = stbi_load("globee3.png", &width, &height, &nrChannels, 0);
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

	// Prepare text and projection matrix //
	YText yText;
	yText.PrepareCharacters();

	// To draw texts at specific window position
	Shader textShader("text.vs", "text.fs");
	glm::mat4 textProjection = glm::ortho(0.0f, static_cast<GLfloat>(SCR_WIDTH), 0.0f, static_cast<GLfloat>(SCR_HEIGHT));
	textShader.use();
	textShader.setMat4("projection",textProjection);
	yText.BindBuffer();

	// To draw texts at country position on the globe
	Shader countrytextShader("countrytext.vs", "countrytext.fs");
	glm::mat4 countrytextProjection = glm::ortho(0.0f, static_cast<GLfloat>(SCR_WIDTH), 0.0f, static_cast<GLfloat>(SCR_HEIGHT));
	countrytextShader.use();
	countrytextShader.setMat4("projection", countrytextProjection);
	yText.BindBufferC();

	//--------------------------------------------------------------------------------//

	// set blend mode(alpha)
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);

	// -- //
	glm::mat4 worldRotate = glm::rotate(glm::mat4(1.0f), glm::radians(60.0f), cameraFront);
	glm::vec4 cameraPosVec4 = glm::vec4(cameraPos, 1.0f);
	cameraPosVec4 = worldRotate * cameraPosVec4;
	// -- //

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
		//view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.5f));
		// -- //
		//cameraPos.x = cameraPosVec4.x;
		//cameraPos.y = cameraPosVec4.y;
		//cameraPos.z = cameraPosVec4.z;
		// -- //
		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		//model = glm::translate(model, glm::vec3(0.0f, 0.0f, -3.0f));
		if (DIMENSION == DIMENSION3) {
			//model = glm::rotate(model, glm::radians(60.0f), glm::vec3(-1.0f, 0.5f, 0.0f));
			model = glm::rotate(model, glm::radians(60.0f), glm::vec3(-1.0f, 0.0f, 0.0f));
			float rotationAngle = 10.0f * glfwGetTime();
			model = glm::rotate(model, glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
			model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
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

// Draw border // 
		roiShader.use();
		roiShader.setInt("texture1", 0);
		roiShader.setInt("texture2", 1);
		roiShader.setMat4("projection", projection); 
		roiShader.setMat4("view", view);
		roiShader.setMat4("model", model);

#if 1
		int listSize = globeEffect.ObjectList_.size();
		for (int i = 0; i < listSize; i++) {
			VertexObject vertexObject = globeEffect.ObjectList_[i];
			glLineWidth(3.3f);
			globeEffect.BindAreaObject(VAO2, VBO2, EBO2, vertexObject);
			glDrawElements(GL_LINE_LOOP, vertexObject.indiceCnt * 3, GL_UNSIGNED_INT, (void*)0);
		}
		glBindVertexArray(0);
#endif
		// -- //


		// render text //
		yText.RenderText(textShader, std::wstring(L"Interactive Globe"), 25.0f, 25.0f, 1.0f, glm::vec3(1.0f, 0.3f, 0.3f));
		yText.RenderText(textShader, L"Push", 560.0f, 550.0f, 0.5f, glm::vec3(0.4f, 0.4f, 0.4f));
		yText.RenderText(textShader, L"W  S  A  D", 640.0f, 550.0f, 0.5f, glm::vec3(0.8f, 0.5f, 0.5f));

		countrytextShader.use();

		// get text position //
		float pX, pY, pZ;
		glm::vec4 pos;
		float textX, textY;

		GlobeEffect::LatLon2Xyz(35.9078f, 127.7669f, pX, pY, pZ, DIMENSION);//East
		pos = glm::vec4(pX, pY, pZ, 1.0f);
		pos = view * model * pos;
		pos.x /= -pos.z;
		pos.y /= -pos.z;
		pos = projection * pos;
		textX = (pos.x + 1)/2 * SCR_WIDTH;//because -1<=pos.x<=1 --> 0<=pos.x<=SCR_WIDTH
		textY = (pos.y + 1)/2 * SCR_HEIGHT;
		if (pos.z <= 2.0f)
			yText.RenderCountryText(countrytextShader, L"대한민국", textX, textY, 0, 0.5f, glm::vec3(1.0f, 0.3f, 0.3f));

		GlobeEffect::LatLon2Xyz(37.0902f, 360.0f - 95.7129f, pX, pY, pZ, DIMENSION);//West
		pos = glm::vec4(pX, pY, pZ, 1.0f);
		pos = view * model * pos;
		pos.x /= -pos.z;
		pos.y /= -pos.z;
		pos = projection * pos;
		textX = (pos.x + 1)/2 * SCR_WIDTH;//because -1<=pos.x<=1 --> 0<=pos.x<=SCR_WIDTH
		textY = (pos.y + 1)/2 * SCR_HEIGHT;
		if (pos.z <= 2.0f)
			yText.RenderCountryText(countrytextShader, L"미국", textX, textY, 0, 0.5f, glm::vec3(1.0f, 0.3f, 0.3f));

		GlobeEffect::LatLon2Xyz(56.1304f, 360.0f - 106.3468f, pX, pY, pZ, DIMENSION);//West
		pos = glm::vec4(pX, pY, pZ, 1.0f);
		pos = view * model * pos;
		pos.x /= -pos.z;
		pos.y /= -pos.z;
		pos = projection * pos;
			textX = (pos.x + 1) / 2 * SCR_WIDTH;//because -1<=pos.x<=1 --> 0<=pos.x<=SCR_WIDTH
			textY = (pos.y + 1) / 2 * SCR_HEIGHT;
		if (pos.z <= 2.0f)
			yText.RenderCountryText(countrytextShader, L"캐나다", textX, textY, 0, 0.5f, glm::vec3(1.0f, 0.3f, 0.3f));

		GlobeEffect::LatLon2Xyz(35.8617f, 104.1954f, pX, pY, pZ, DIMENSION);//East
		pos = glm::vec4(pX, pY, pZ, 1.0f);
		pos = view * model * pos;
		pos.x /= -pos.z;
		pos.y /= -pos.z;
		pos = projection * pos;
		textX = (pos.x + 1)/2 * SCR_WIDTH;//because -1<=pos.x<=1 --> 0<=pos.x<=SCR_WIDTH
		textY = (pos.y + 1)/2 * SCR_HEIGHT;
		if (pos.z <= 2.0f)
			yText.RenderCountryText(countrytextShader, L"중국", textX, textY, 0, 0.5f, glm::vec3(1.0f, 0.3f, 0.3f));

		GlobeEffect::LatLon2Xyz(55.7381f, 360.0f - 3.4360f, pX, pY, pZ, DIMENSION);//West
		pos = glm::vec4(pX, pY, pZ, 1.0f);
		pos = view * model * pos;
		pos.x /= -pos.z;
		pos.y /= -pos.z;
		pos = projection * pos;
		textX = (pos.x + 1)/2 * SCR_WIDTH;//because -1<=pos.x<=1 --> 0<=pos.x<=SCR_WIDTH
		textY = (pos.y + 1)/2 * SCR_HEIGHT;
		if (pos.z <= 2.0f)
			yText.RenderCountryText(countrytextShader, L"영국", textX, textY, 0, 0.5f, glm::vec3(1.0f, 0.3f, 0.3f));

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	glDeleteVertexArrays(1, &VAO2);
	glDeleteBuffers(1, &VBO2);
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
