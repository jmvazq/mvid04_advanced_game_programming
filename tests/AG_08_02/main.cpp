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

// Lots of cubes
glm::vec3 cube_positions[] = {
	glm::vec3(0.0f, 0.0f, 0.0f),
	glm::vec3(2.0f, 5.0f, -15.0f),
	glm::vec3(-1.5f, -2.2f, -2.5f),
	glm::vec3(-3.8f, -2.0f, -12.3f),
	glm::vec3(2.4f, -0.4f, -3.5f),
	glm::vec3(-1.7f, 3.0f, -7.5f),
	glm::vec3(1.3f, -2.0f, -2.5f),
	glm::vec3(1.5f, 2.0f, -2.5f),
	glm::vec3(1.5f, 0.2f, -1.5f),
	glm::vec3(-1.3f, 1.0f, -1.5f)
};

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

void render(const uint32_t& VAO, const Shader& shader_lamp, const Shader& shader_cube, const int32_t text_dif, const uint32_t text_spec) {
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);

	glm::mat4 proj = glm::mat4(1.0f);
	proj = glm::perspective(glm::radians(camera.getFOV()), (float) screen_width / screen_height, 0.1f, 60.0f);

	const glm::mat4 view = camera.getViewMatrix();

	// Draw the lamp
	shader_lamp.use();

	// model transformations
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, lightPos);
	model = glm::scale(model, glm::vec3(0.2f));

	shader_lamp.set("projection", proj);
	shader_lamp.set("view", view);
	shader_lamp.set("model", model);

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);

	// Draw the cubes
	shader_cube.use();

	shader_cube.set("projection", proj);
	shader_cube.set("view", view);

	shader_cube.set("viewPos", camera.getPosition());

	// Cube - Light properties
	shader_cube.set("light.direction", -0.2, -1.0f, -0.3f);
	shader_cube.set("light.ambient", 0.1f, 0.1f, 0.1f);
	shader_cube.set("light.diffuse", 0.5f, 0.5f, 0.5f);
	shader_cube.set("light.specular", 1.0f, 1.0f, 1.0f);

	// Cube - Material properties
	shader_cube.set("material.ambient", 0.135f, 0.2225f, 0.1575f);
	shader_cube.set("material.diffuse", 0);
	shader_cube.set("material.specular", 1);
	shader_cube.set("material.shininess", (float) 0.1f * 128);

	// Set text_dif and specular map
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, text_dif);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, text_spec);

	shader_cube.set("texture1", 0);
	shader_cube.set("texture2", 1);

	glBindVertexArray(VAO);

	for (uint32_t i = 0; i < 10; i++)
	{
		// Draw a rotating cube

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, cube_positions[i]);
		const float angle = 10.f + (20.0f * i);
		model = glm::rotate(model, static_cast<float>(glfwGetTime()) * glm::radians(angle), glm::vec3(0.5f, 1.0f, 0.0f));

		shader_cube.set("model", model);

		const glm::mat3 normalMat = glm::inverse(glm::transpose(model));
		shader_cube.set("normalMat", normalMat);

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
	}
}

uint32_t createVertexData(uint32_t *VBO, uint32_t *EBO)
{
	// Whoah! A Cube!
	float vertices[] = {
		// Position=0 // Normal=1 // UVs=2
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

	uint32_t indices[] = {
		0, 1, 2,		0, 2, 3, // Front
		4, 5, 6,		4, 6, 7, // Right
		8, 9, 10,		8, 10, 11, // Back
		12, 13, 14,		12, 14, 15, // Left
		16, 17, 18,		16, 18, 19, // Bottom
		20, 21, 22,		20, 22, 23 // Top
	};

	uint32_t VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, VBO);
	glGenBuffers(1, EBO);

	glBindVertexArray(VAO);

	// Bind buffers and assign data
	glBindBuffer(GL_ARRAY_BUFFER, *VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

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

	// Create program and vertex data
	const Shader shader_light("../tests/AG_08_02/light.vs", "../tests/AG_08_02/light.fs");
	const Shader shader_cube("../tests/AG_08_02/cube.vs", "../tests/AG_08_02/cube.fs");

	uint32_t VBO, EBO; // vertex and element buffer objects
	uint32_t VAO = createVertexData(&VBO, &EBO); // vertex array object

	// textures/maps
	stbi_set_flip_vertically_on_load(true);
	uint32_t text_dif = createTexture("../tests/AG_08_02/AG07_albedo.png");
	uint32_t text_spec = createTexture("../tests/AG_08_02/AG07_specular.png");

	// Set window bg color
	glClearColor(0.0f / 255.0f, 0.0f / 255.0f, 0.0f / 255.0f, 1.0f);

	// Set polygon mode
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // NOTE: use GL_LINE to use "WIREFRAME MODE" instead

	// Enable cull faces
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK); // don't draw back faces

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
		render(VAO, shader_light, shader_cube, text_dif, text_spec);

		// Swap front and back buffers
		glfwSwapBuffers(window);

		// Poll for and process events
		glfwPollEvents();
	}

	// Delete everything before terminating
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteTextures(1, &text_dif);
	glDeleteTextures(1, &text_spec);

	// Exit
	glfwTerminate();

	return 0;
}
