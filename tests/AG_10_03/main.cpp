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

Camera camera(glm::vec3(-1.0f, 2.0f, 3.0f));

// Light
glm::vec3 lightPos(1.2f, 1.0f, -2.0f);

std::string projectDir = "../tests/AG_10_03";
std::string assetsDir = "../assets";

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

void render(const uint32_t& cubeVAO, const uint32_t& quadVAO, const Shader& lightingShader, const Shader& blendingShader, const int32_t text_dif, const uint32_t text_spec, const uint32_t text_tree) {
	glClearColor(0.0f / 255.0f, 0.0f / 255.0f, 0.0f / 255.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Textures
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, text_dif);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, text_spec);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, text_tree);

	lightingShader.use();

	lightingShader.set("objectColor", 1.0f, 0.5f, 0.31f);

	lightingShader.set("viewPos", camera.getPosition());
	lightingShader.set("light.position", lightPos);
	lightingShader.set("light.ambient", 0.2f, 0.2f, 0.2f);
	lightingShader.set("light.diffuse", 0.5f, 0.5f, 0.5f);
	lightingShader.set("light.specular", 1.0f, 1.0f, 1.0f);
	lightingShader.set("material.ambient", 1.0f, 0.5f, 0.31f);
	//lightingShader.set("material.diffuse", 1.0f, 0.5f, 0.31f);
	lightingShader.set("material.diffuse", 0);
	lightingShader.set("material.specular", 1);
	lightingShader.set("material.shininess", 32.0f);

	// Projection and View
	glm::mat4 proj = glm::mat4(1.0f);
	proj = glm::perspective(glm::radians(camera.getFOV()), (float) screen_width / screen_height, 0.1f, 100.0f);

	const glm::mat4 view = camera.getViewMatrix();

	// Re-use these:
	glm::mat4 model;
	glm::mat3 normalMat;

	// Quad / ground
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(3.0f, 1.0f, 3.0f));

	lightingShader.set("projection", proj);
	lightingShader.set("view", view);
	lightingShader.set("model", model);

	normalMat = glm::inverse(glm::transpose(glm::mat3(model)));
	lightingShader.set("normalMat", normalMat);

	glBindVertexArray(quadVAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

	// First cube
	model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(0.0f, 0.7f, 1.0f));
	model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
	lightingShader.set("model", model);

	normalMat = glm::inverse(glm::transpose(glm::mat3(model)));
	lightingShader.set("normalMat", normalMat);

	glBindVertexArray(cubeVAO);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);

	// Second cube
	model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(0.0f, 0.7f, 0.0f));
	model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
	lightingShader.set("model", model);

	normalMat = glm::inverse(glm::transpose(glm::mat3(model)));
	lightingShader.set("normalMat", normalMat);

	glBindVertexArray(cubeVAO);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);

	// Third cube
	model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(0.0f, 0.7f, -1.0f));
	model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
	lightingShader.set("model", model);

	normalMat = glm::inverse(glm::transpose(glm::mat3(model)));
	lightingShader.set("normalMat", normalMat);

	glBindVertexArray(cubeVAO);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);

	// Quad / tree with alpha blending
	blendingShader.use();

	glm::mat4 model_tree = glm::mat4(1.0f);
	model_tree = glm::translate(model_tree, glm::vec3(0.0f, 0.645f, 1.1f));
	model_tree = glm::scale(model_tree, glm::vec3(0.3f, 0.3f, 0.3f));
	model_tree = glm::rotate(model_tree, glm::pi<float>() / 2.0f, glm::vec3(1.0f, 0.0f, 0.0f));

	blendingShader.set("projection", proj);
	blendingShader.set("view", view);
	blendingShader.set("model", model_tree);

	normalMat = glm::inverse(glm::transpose(glm::mat3(model_tree)));
	blendingShader.set("normalMat", normalMat);

	blendingShader.set("texture", 2);

	glBindVertexArray(quadVAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

	glBindVertexArray(0); // no need to unbind it every time
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
		const auto format = nrChannels == 4 ? GL_RGBA : GL_RGB;
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
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

	// Create program and vertex data
	const Shader lightingShader((projectDir + "/cube.vs").c_str(), (projectDir + "/cube.fs").c_str());
	const Shader blendingShader((projectDir + "/blending.vs").c_str(), (projectDir + "/blending.fs").c_str());

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
	float quad_vertices[] = { // Position // Normal // Texture Coords
		-0.5f, 0.5f, 0.5f,		0.0f, 1.0f, 0.0f,		0.0f, 0.0f,
		0.5f, 0.5f, 0.5f,		0.0f, 1.0f, 0.0f,		1.0f, 0.0f,
		0.5f, 0.5f, -0.5f,		0.0f, 1.0f, 0.0f,		1.0f, 1.0f,
		-0.5f, 0.5f, -0.5f,		0.0f, 1.0f, 0.0f,		0.0f, 1.0f
	};

	uint32_t quad_indices[] = {
		0, 1, 2,		0, 2, 3
	};

	uint32_t quadVAO = createVertexData(quad_vertices, (uint32_t) sizeof(quad_vertices) / sizeof(float) / 8, quad_indices, sizeof(quad_indices) / sizeof(uint32_t));

	// Textures/maps
	stbi_set_flip_vertically_on_load(true);
	uint32_t text_dif = createTexture((projectDir + "/albedo.png").c_str());
	uint32_t text_spec = createTexture((projectDir + "/specular.png").c_str());
	uint32_t text_tree = createTexture((projectDir + "/tree.png").c_str());

	// Set polygon mode
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // NOTE: use GL_LINE to use "WIREFRAME MODE" instead

	// Enable cull faces
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK); // don't draw back faces

	// Enable Depth Testing 
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// Enable Blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	while (!glfwWindowShouldClose(window))
	{
		const float currentFrame = glfwGetTime();
		const float deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Handle input
		handleInput(window, deltaTime);

		// Render
		render(cubeVAO, quadVAO, lightingShader, blendingShader, text_dif, text_spec, text_tree);

		// Swap front and back buffers
		glfwSwapBuffers(window);

		// Poll for and process events
		glfwPollEvents();
	}

	// Delete everything before terminating
	glDeleteVertexArrays(1, &quadVAO);
	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteTextures(1, &text_dif);
	glDeleteTextures(1, &text_spec);

	// Exit
	glfwTerminate();

	return 0;
}
