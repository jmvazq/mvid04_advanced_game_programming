#define _USE_MATH_DEFINES

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include <cmath>


#pragma region Functions: for Main Loop

void onChangeFrameBufferSize(GLFWwindow *window, const int32_t width, const int32_t height)
{
	glad_glViewport(0, 0, width, height);
}

void handleInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
}

void render(uint32_t VAO, uint32_t program) {
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(program);
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 7);
}

#pragma endregion

#pragma region Functions: for creating program with shaders
bool checkShader(uint32_t shader)
{
	int success;
	char infoLog[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(shader, 512, nullptr, infoLog);
		std::cout << "Error compiling Shader: " << std::endl << infoLog << std::endl;
		return false;
	}
	return true;
}

bool checkProgram(uint32_t program)
{
	int success;
	char infoLog[512];
	glGetProgramiv(program, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(program, 512, nullptr, infoLog);
		std::cout << "Error compiling Shader Program: " << std::endl << infoLog << std::endl;
		return false;
	}
	return true;
}


uint32_t createProgram()
{
	// Define shader sources

	// For positioning the vertices
	const char *vertexShaderSource = "#version 330 core\n"
		"layout (location=0) in vec3 aPos;\n"
		"void main() {\n"
		"    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
		"}\0";

	// For painting the pixels
	const char *fragmentVertexShaderSource = "#version 330 core\n"
		"out vec4 fragColor;\n"
		"void main() {\n"
		"    fragColor = vec4(1.0, 1.0, 53.0 / 255.0, 1.0);\n"
		"}\0";

	// Compile shader sources
	uint32_t vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
	glCompileShader(vertexShader);
	checkShader(vertexShader);

	uint32_t fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentVertexShaderSource, nullptr);
	glCompileShader(fragmentShader);
	checkShader(fragmentShader);

	// Create program, attach shaders, and link program
	const uint32_t shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	checkProgram(shaderProgram);

	// Delete shaders, since they're already attached to the program
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return shaderProgram;
}

#pragma endregion 

#pragma region Functions: for dealing with vertex data

uint32_t createVertexData(uint32_t *VBO)
{
	// HEXAGON
	const uint32_t n_sides = 6;
	const uint32_t n_vertices = n_sides + 1; // 6 corners + center
	const float r = 0.75f; // radius or side length

	float xPositions[n_vertices];
	float yPositions[n_vertices];

	// Define center position
	xPositions[0] = 0.0f;
	yPositions[0] = 0.0f;

	// Calculate positions - x and y components
	for (uint32_t i = 0; i < n_vertices; i++) {
		xPositions[i] = r * cos(2 * M_PI * i / n_sides);
		yPositions[i] = r * sin(2 * M_PI * i / n_sides);
	}

	float vertices[n_vertices * 3]; // triangle vertex array
	// Assign vertex positions
	for (uint32_t i = 0; i < n_vertices; i++)
	{
		int xPos = i * 3;
		int yPos = i * 3 + 1;
		int zPos = i * 3 + 2;

		// calculate vertex coordinates
		vertices[xPos] = xPositions[i]; // x
		vertices[yPos] = yPositions[i]; // y
		vertices[zPos] = 0.0f; // z
	}

	uint32_t VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, VBO);

	glBindVertexArray(VAO);

	// Bind buffers and assign data
	glBindBuffer(GL_ARRAY_BUFFER, *VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Set vertex attribute pointer at 0, then enable
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Unbind buffers and array
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return VAO;
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
	GLFWwindow* window = glfwCreateWindow(800, 600, "New Window", nullptr, nullptr);
	if (!window)
	{
		std::cout << "Failed t create GLFW Window" << std::endl;
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

	// Create program and vertex data
	uint32_t program = createProgram();
	uint32_t VBO; // vertex buffer object
	uint32_t VAO = createVertexData(&VBO); // get vertex array object

	// Set window bg color
	glClearColor(148.0f / 255.0f, 0.0f, 211.0f / 255.0f, 1.0f);

	// Set polygon mode
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // NOTE: use GL_LINE to use "WIREFRAME MODE" instead

	// Enable cull faces
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK); // don't draw back faces

	while (!glfwWindowShouldClose(window))
	{
		// Handle input
		handleInput(window);

		// Render
		render(VAO, program);

		// Swap front and back buffers
		glfwSwapBuffers(window);

		// Poll for and process events
		glfwPollEvents();
	}

	// Delete everything before terminating
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(program);

	// Exit
	glfwTerminate();

	return 0;
}
