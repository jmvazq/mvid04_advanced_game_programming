#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include "shader.h"
#include "camera.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

// View and camera
uint32_t screen_width = 800;
uint32_t screen_height = 600;

float lastFrame = 0.0f;
bool firstMouse = true;

float lastX = screen_width / 2.0f;
float lastY = screen_height / 2.0f;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

// Light
glm::vec3 lightPos(-2.0f, 4.0f, -1.0f);

std::string projectDir = "../tests/AG_13";
std::string assetsDir = "../assets";

// Shadow Map
const float shadow_near = 1.0f;
const float shadow_far = 7.5f;

uint32_t shadow_width = 1024;
uint32_t shadow_height = 1024;

#pragma region Functions: for Main Loop

void onChangeFrameBufferSize(GLFWwindow *window, const int32_t width, const int32_t height)
{
	screen_width = width;
	screen_height = height;

	glad_glViewport(0, 0, width, height);
}

void handleInput(GLFWwindow *window, const float dt)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		camera.handleKeyboard(Camera::Movement::Forward, dt);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		camera.handleKeyboard(Camera::Movement::Left, dt);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		camera.handleKeyboard(Camera::Movement::Backward, dt);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		camera.handleKeyboard(Camera::Movement::Right, dt);
	}

}

void onScroll(GLFWwindow* window, double xoffset, double yoffset) {
	camera.handleMouseScroll(yoffset);
}

std::pair<uint32_t, uint32_t> createShadowFBO()
{
	uint32_t fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	uint32_t depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadow_width, shadow_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	
	float borderColor[] = {	1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	
	glBindTexture(GL_TEXTURE_2D, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "Error: FrameBuffer not complete." << std::endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return std::make_pair(fbo, depthMap);
}

void onMouse(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		firstMouse = false;
		lastX = xpos;
		lastY = ypos;
	}

	const float xoffset = xpos - lastX;
	const float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	camera.handleMouseMovement(xoffset, yoffset);
}

void drawScene(const Shader& shader, const uint32_t cubeVAO, const uint32_t quadVAO, const uint32_t text1, const uint32_t text2) {

	// Floor
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, text1);

	shader.set("diffuseTexture", 0);

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(10.0f, 0.0f, 0.0f));
	shader.set("model", model);

	glBindVertexArray(quadVAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

	// Draw cubes
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, text2);
	shader.set("diffuseTexture", 0);

	// Cube 1
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0f));
	model = glm::scale(model, glm::vec3(0.5f));
	shader.set("model", model);
	glBindVertexArray(cubeVAO);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);

	// Cube 2
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.5f, -0.25f, 0.5f));
	model = glm::scale(model, glm::vec3(0.5f));
	shader.set("model", model);
	glBindVertexArray(cubeVAO);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);

	// Cube 3
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 2.0f));
	model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0f, 0.0f, 1.0f)));
	model = glm::scale(model, glm::vec3(0.25f));
	shader.set("model", model);
	glBindVertexArray(cubeVAO);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);

	glBindVertexArray(0);
}

void render(const uint32_t& cubeVAO, const uint32_t& quadVAO, const uint32_t& quadScreenVAO, const Shader& lightingShader, const Shader& depthShader, const Shader& debugShader, const int32_t text1, const uint32_t text2,
	const uint32_t fbo, const uint32_t text_fbo) {

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

	// Calculate the Light Space Matrix
	const glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, shadow_near, shadow_far);
	const glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	const glm::mat4 lightSpaceMatrix = lightProjection * lightView;

	// Shadows
	depthShader.use();
	depthShader.set("lightSpaceMatrix", lightSpaceMatrix);

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glClear(GL_DEPTH_BUFFER_BIT);
	// glCullFace(GL_FRONT); // sometimes helps to correct Peter-Panning issue
	glViewport(0, 0, shadow_width, shadow_height);
	// glCullFace(GL_BACK);
	drawScene(depthShader, cubeVAO, quadVAO, 0, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Lighting
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, screen_width, screen_height);

	lightingShader.use();

	const glm::mat4 projection = glm::perspective(glm::radians(camera.getFOV()), (float)screen_width / (float)screen_height, 0.1f, 100.0f);
	const glm::mat4 view = camera.getViewMatrix();

	lightingShader.set("projection", projection);
	lightingShader.set("view", view);
	lightingShader.set("viewPos", camera.getPosition());
	lightingShader.set("lightPos", lightPos);
	lightingShader.set("lightSpaceMatrix", lightSpaceMatrix);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, text_fbo);
	lightingShader.set("depthMap", 1);
	drawScene(lightingShader, cubeVAO, quadVAO, text1, text2);

	// Debug
	// debugShader.use();
	// glActiveTexture(GL_TEXTURE0);
	// glBindTexture(GL_TEXTURE_2D, text_fbo);
	// debugShader.set("depthMap", 0);
	// 
	// glBindVertexArray(quadScreenVAO);
	// glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}

uint32_t createVertexData(const float* vertices, const uint32_t n_verts, const uint32_t* indices, const uint32_t n_indices)
{
	uint32_t VAO, VBO, EBO;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	// Bind buffers and assign data
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, n_verts * sizeof(float) * 8, vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, n_indices * sizeof(uint32_t), indices, GL_STATIC_DRAW);

	// Set vertex attribute pointer at 0 (position attribute), then enable
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Set vertex attribute pointer at 1 (normal attribute), then enable
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// Set vertex attribute pointer at 0 (position attribute), then enable
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// Unbind buffers and array
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // NOTE: must happen after unbinding VAO

	return VAO;
}

uint32_t createTexture(const char* path)
{
	// Generate and bind texture
	uint32_t texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	// Texture wrapping
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Texture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nrChannels;
	unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(data);
	}
	else
	{
		std::cout << "Failed to load texture: " << path << std::endl;
	}

	return texture;
}

#pragma endregion

int main(int argc, char* argv[])
{
	if (!glfwInit()) // Initialize the GLFW library
	{
		std::cout << "Failed to initialize GLFW." << std::endl;
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // Use OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Use Core Profile

	// Create new window in windowed mode and set its OpenGL context
	GLFWwindow* window = glfwCreateWindow(screen_width, screen_height, "New Window", nullptr, nullptr);
	if (!window)
	{
		std::cout << "Failed to create GLFW Window." << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) { // Initialize GLAD
		std::cout << "Failed to initialize GLAD." << std::endl;
		return -1;
	}

	// Set callback for window resize
	glfwSetFramebufferSizeCallback(window, onChangeFrameBufferSize);
	glfwSetCursorPosCallback(window, onMouse);
	glfwSetScrollCallback(window, onScroll);

	// Cube Data
	float cube_vertices[] = {
		// Position // Normal // Texture Coords
		-0.5f, -0.5f, 0.5f,		0.0f, 0.0f, 1.0f,		0.0f, 0.0f, // Front quad
		0.5f, -0.5f, 0.5f,		0.0f, 0.0f, 1.0f,		1.0f, 0.0f,
		0.5f, 0.5f, 0.5f,		0.0f, 0.0f, 1.0f,		1.0f, 1.0f,
		-0.5f, 0.5f, 0.5f,		0.0f, 0.0f, 1.0f,		0.0f, 1.0f,
		0.5f, -0.5f, 0.5f,		1.0f, 0.0f, 0.0f,		0.0f, 0.0f, // Right quad
		0.5f, -0.5f, -0.5f,		1.0f, 0.0f, 0.0f,		1.0f, 0.0f,
		0.5f, 0.5f, -0.5f,		1.0f, 0.0f, 0.0f,		1.0f, 1.0f,
		0.5f, 0.5f, 0.5f,		1.0f, 0.0f, 0.0f,		0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,	0.0f, 0.0f, -1.0f,		1.0f, 0.0f, // Back quad
		-0.5f, 0.5f, -0.5f,		0.0f, 0.0f, -1.0f,		1.0f, 1.0f,
		0.5f, 0.5f, -0.5f,		0.0f, 0.0f, -1.0f,		0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,		0.0f, 0.0f, -1.0f,		0.0f, 0.0f,
		-0.5f, -0.5f, 0.5f,		-1.0f, 0.0f, 0.0f,		1.0f, 0.0f, // Left quad
		-0.5f, 0.5f, 0.5f,		-1.0f, 0.0f, 0.0f,		1.0f, 1.0f,
		-0.5f, 0.5f, -0.5f,		-1.0f, 0.0f, 0.0f,		0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,	-1.0f, 0.0f, 0.0f,		0.0f, 0.0f,
		-0.5f, -0.5f, 0.5f,		0.0f, -1.0f, 0.0f,		0.0f, 1.0f, // Bottom quad
		-0.5f, -0.5f, -0.5f,	0.0f, -1.0f, 0.0f,		0.0f, 0.0f,
		0.5f, -0.5f, -0.5f,		0.0f, -1.0f, 0.0f,		1.0f, 0.0f,
		0.5f, -0.5f, 0.5f,		0.0f, -1.0f, 0.0f,		1.0f, 1.0f,
		-0.5f, 0.5f, 0.5f,		0.0f, 1.0f, 0.0f,		0.0f, 0.0f, // Top quad
		0.5f, 0.5f, 0.5f,		0.0f, 1.0f, 0.0f,		1.0f, 0.0f,
		0.5f, 0.5f, -0.5f,		0.0f, 1.0f, 0.0f,		1.0f, 1.0f,
		-0.5f, 0.5f, -0.5f,		0.0f, 1.0f, 0.0f,		0.0f, 1.0f
	};

	uint32_t cube_indices[] = {
		0, 1, 2,		0, 2, 3, // Front
		4, 5, 6,		4, 6, 7, // Right
		8, 9, 10,		8, 10, 11, // Back
		12, 13, 14,		12, 14, 15, // Left
		16, 17, 18,		16, 18, 19, // Bottom
		20, 21, 22,		20, 22, 23 // Top
	};

	uint32_t cubeVAO = createVertexData(cube_vertices, (uint32_t) sizeof(cube_vertices) / sizeof(float) / 8, cube_indices, sizeof(cube_indices) / sizeof(uint32_t));
	
	// Quad Data
	float quad_vertices[] = {
		// Position // Normal // Texture Coords
		25.0, -0.5f, 25.0,			0.0f, 1.0f, 0.0f,		25.0, 0.0f,
		-25.0, -0.5f, 25.0,			0.0f, 1.0f, 0.0f,		0.0f, 0.0f,
		-25.0, -0.5f, -25.0,		0.0f, 1.0f, 0.0f,		0.0f, 25.0,

		25.0, -0.5f, 25.0,			0.0f, 1.0f, 0.0f,		25.0, 0.0f,
		-25.0, -0.5f, -25.0,		0.0f, 1.0f, 0.0f,		0.0f, 25.0,
		25.0, -0.5f, -25.0,			0.0f, 1.0f, 0.0f,		25.0, 25.0,
	};

	uint32_t quad_indices[] = { 0, 2, 1,	3, 5, 4 };

	uint32_t quadVAO = createVertexData(quad_vertices, (uint32_t) sizeof(quad_vertices) / sizeof(float) / 8, quad_indices, sizeof(quad_indices) / sizeof(uint32_t));

	// Screen quad
	float quad_screen_vertices[] = { // Position // Normal // Texture Coords
		-1.0f, -1.0f, 0.0f,		0.0f, 0.0f, 1.0f,		0.0f, 0.0f,
		1.0f, -1.0f, 0.0f,		0.0f, 0.0f, 1.0f,		1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,		0.0f, 0.0f, 1.0f,		1.0f, 1.0f,
		-1.0f, 1.0f, 0.0f,		0.0f, 0.0f, 1.0f,		0.0f, 1.0f,
	};

	uint32_t quad_screen_indices[] = {
		0, 1, 2,		0, 2, 3
	};

	uint32_t quadScreenVAO = createVertexData(quad_screen_vertices, (uint32_t) sizeof(quad_screen_vertices) / sizeof(float) / 8, quad_screen_indices, sizeof(quad_screen_indices) / sizeof(uint32_t));

	// Create program and vertex data
	const Shader lightingShader((projectDir + "/cube.vs").c_str(), (projectDir + "/cube.fs").c_str());
	const Shader depthShader((projectDir + "/depth.vs").c_str(), (projectDir + "/depth.fs").c_str());
	const Shader debugShader((projectDir + "/debug.vs").c_str(), (projectDir + "/debug.fs").c_str());

	// Textures/maps
	stbi_set_flip_vertically_on_load(true);
	uint32_t text1 = createTexture((projectDir + "/floor_albedo.png").c_str());
	uint32_t text2 = createTexture((projectDir + "/cube_albedo.png").c_str());

	const auto fbo_res = createShadowFBO();

	// Set polygon mode
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // NOTE: use GL_LINE to use "WIREFRAME MODE" instead

	// Enable cull faces
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK); // don't draw back faces

	// Enable depth testing
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	while (!glfwWindowShouldClose(window))
	{
		const float currentFrame = glfwGetTime();
		const float deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Handle input
		handleInput(window, deltaTime);

		// Render
		render(cubeVAO, quadVAO, quadScreenVAO, lightingShader, depthShader, debugShader, text1, text2, fbo_res.first, fbo_res.second);

		// Swap front and back buffers
		glfwSwapBuffers(window);

		// Poll for and process events
		glfwPollEvents();
	}

	// Delete everything before terminating
	glDeleteVertexArrays(1, &quadVAO);
	glDeleteVertexArrays(1, &quadScreenVAO);
	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteTextures(1, &text1);
	glDeleteTextures(1, &text2);

	// Exit
	glfwTerminate();

	return 0;
}
