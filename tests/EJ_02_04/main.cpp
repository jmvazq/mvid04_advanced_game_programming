#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

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

void render(uint32_t VAO[2], uint32_t program[2]) {
	glClear(GL_COLOR_BUFFER_BIT);

	// draw triangles, each with their own program

	// FIRST/YELLOW TRIANGLE
	glUseProgram(program[0]);
	glBindVertexArray(VAO[0]);
	glDrawArrays(GL_TRIANGLES, 0, 3);

	// SECOND/BLUE TRIANGLE
	glUseProgram(program[1]);
	glBindVertexArray(VAO[1]);
	glDrawArrays(GL_TRIANGLES, 0, 3);
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

uint32_t getProgram(const char *vertexShaderSource, const char *fragmentVertexShaderSource)
{
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

void createPrograms(uint32_t program[2])
{
	// Define shader sources

	// For positioning the vertices
	const char *vertexShaderSrc = "#version 330 core\n"
		"layout (location=0) in vec3 aPos;\n"
		"void main() {\n"
		"    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
		"}\0";

	// For painting the pixels
	const char *yellowFragmentShaderSrc = "#version 330 core\n" // FIRST/YELLOW TRIANGLE
		"out vec4 fragColor;\n"
		"void main() {\n"
		"    fragColor = vec4(1.0, 1.0, 53.0 / 255.0, 1.0);\n"
		"}\0";
	const char *blueFragmentShaderSrc = "#version 330 core\n" // SECOND/BLUE TRIANGLE
		"out vec4 fragColor;\n"
		"void main() {\n"
		"    fragColor = vec4(30.0 / 255.0, 144.0 / 255.0, 1.0, 1.0);\n"
		"}\0";

	program[0] = getProgram(vertexShaderSrc, yellowFragmentShaderSrc);
	program[1] = getProgram(vertexShaderSrc, blueFragmentShaderSrc);
}

#pragma endregion 

#pragma region Functions: for dealing with vertex data

uint32_t getVAO(uint32_t *VBO, float vertices[], int v_size)
{
	uint32_t VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, VBO);

	glBindVertexArray(VAO);

	// Bind buffers and assign data
	glBindBuffer(GL_ARRAY_BUFFER, *VBO);
	glBufferData(GL_ARRAY_BUFFER, v_size, vertices, GL_STATIC_DRAW);

	// Set vertex attribute pointer at 0, then enable
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Unbind buffers and array
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return VAO;
}

void createVertexData(uint32_t VBO[2], uint32_t VAO[2])
{
	float firstTriangle[] = {
		-0.8f,  -0.4f, 0.0f, // bottom left
		0.0f, -0.4f, 0.0f, // bottom right -repeats
		-0.4f, 0.4f, 0.0f, // top
	};
	float secondTriangle[] = {
		0.0f, -0.4f, 0.0f, // bottom left - repeats
		0.8f, -0.4f, 0.0f, // bottom right
		0.4f, 0.4f, 0.0f // top
	};

	VAO[0] = getVAO(&VBO[0], firstTriangle, sizeof(firstTriangle)); // create first triangle array object
	VAO[1] = getVAO(&VBO[1], secondTriangle, sizeof(secondTriangle)); // create second triangle array object
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

	// Create programs and vertex data
	uint32_t program[2];
	createPrograms(program);

	uint32_t VBO[2], VAO[2]; // vertex array and buffer objects' containers
	createVertexData(VBO, VAO);

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
	glDeleteVertexArrays(2, VAO);
	glDeleteBuffers(2, VBO);
	glDeleteProgram(program[0]);
	glDeleteProgram(program[1]);

	// Exit
	glfwTerminate();

	return 0;
}
