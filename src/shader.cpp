#include <shader.h>

#include <fstream>
#include <iostream>
#include <sstream>

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

Shader::Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath) {
	std::string sVertexCode, sFragmentCode, sGeometryCode;
	loadShader(vertexPath, &sVertexCode);
	loadShader(fragmentPath, &sFragmentCode);
	if (geometryPath) {
		loadShader(geometryPath, &sGeometryCode);
	}
	const char* vertexCode = sVertexCode.c_str();
	const char* fragmentCode = sFragmentCode.c_str();

	uint32_t vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vertexCode, NULL);
	glCompileShader(vertex);
	
	checkErrors(vertex, Type::Vertex);
	
	uint32_t fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fragmentCode, NULL);
	glCompileShader(fragment);
	
	checkErrors(fragment, Type::Fragment);
	
	uint32_t geometry;
	if (geometryPath) {
		const char* geometryCode = sGeometryCode.c_str();
		geometry = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometry, 1, &geometryCode, NULL);
		glCompileShader(geometry);
		
		checkErrors(geometry, Type::Geometry);
	}
	
	id_ = glCreateProgram();
	glAttachShader(id_, vertex);
	glAttachShader(id_, fragment);
	if (geometryPath) {
		glAttachShader(id_, geometry);
	}
	glLinkProgram(id_);
	
	checkErrors(id_, Type::Program);
	
	glDeleteShader(vertex);
	glDeleteShader(fragment);
	if (geometryPath) {
		glDeleteShader(geometry);
	}
}

Shader::~Shader() {
	glDeleteProgram(id_);
}

void Shader::use() const {
	glUseProgram(id_);
}

void Shader::loadShader(const char* path, std::string* code) const
 {
	std::ifstream file;

	file.open(path, std::ios_base::in);
	if (file) {
		std::stringstream stream;
		stream << file.rdbuf();
		file.close();
		*code = stream.str();
	}
	else {
		std::cout << "Error loading Shader: " << path << std::endl;
	}
}

void Shader::set(const char* name, const bool value) const
{
	glUniform1i(glGetUniformLocation(id_, name), static_cast<int>(value));
}

void Shader::set(const char* name, const int value) const
{
	glUniform1i(glGetUniformLocation(id_, name), value);
}

void Shader::set(const char* name, const float value) const
{
	glUniform1f(glGetUniformLocation(id_, name), value);
}

void Shader::set(const char* name, const float value1, const float value2) const {
	glUniform2f(glGetUniformLocation(id_, name), value1, value2);
}

void Shader::set(const char* name, const float value1, const float value2, const float value3) const {
	glUniform3f(glGetUniformLocation(id_, name), value1, value2, value3);
}

void Shader::set(const char* name, const float value1, const float value2, const float value3, const float value4) const {
	glUniform4f(glGetUniformLocation(id_, name), value1, value2, value3, value4);
}

void Shader::set(const char* name, const glm::vec2& value) const {
	glUniform2fv(glGetUniformLocation(id_, name), 1, glm::value_ptr(value));
}

void Shader::set(const char* name, const glm::vec3& value) const {
	glUniform3fv(glGetUniformLocation(id_, name), 1, glm::value_ptr(value));
}

void Shader::set(const char* name, const glm::vec4& value) const {
	glUniform4fv(glGetUniformLocation(id_, name), 1, glm::value_ptr(value));
}

void Shader::set(const char* name, const glm::mat2& value) const {
	glUniformMatrix2fv(glGetUniformLocation(id_, name), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::set(const char* name, const glm::mat3& value) const {
	glUniformMatrix3fv(glGetUniformLocation(id_, name), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::set(const char* name, const glm::mat4& value) const {
	glUniformMatrix4fv(glGetUniformLocation(id_, name), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::checkErrors(const uint32_t shader, const Type type) const {
	const int log_size = 512;
	int success;
	char log[log_size];

	if (type != Type::Program) {
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(shader, log_size, nullptr, log);
			std::cout << "Error compiling Shader: " <<
				((type == Type::Vertex) ? "Vertex" : "Fragment") << std::endl << log << std::endl;
		}
	}
	else {
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(shader, log_size, nullptr, log);
			std::cout << "Error linking Shader Program: " << std::endl << log << std::endl;
		}
	}
}