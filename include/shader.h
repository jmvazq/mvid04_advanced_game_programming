#ifndef __SHADER_H__
#define __SHADER_H_ 1

#include <glm/glm.hpp>
#include <string>
#include <vector>

class Shader {
	enum class Type
	{
		Vertex,
		Fragment,
		Geometry,
		Program
	};

public:
	Shader() = delete;
	Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr);
	~Shader();

	void use() const;

	void set(const char* name, const bool value) const;
	void set(const char* name, const int value) const;
	void set(const char* name, const float value) const;
	void set(const char* name, const float value1, const float value2) const;
	void set(const char* name, const float value1, const float value2, const float value3) const;
	void set(const char* name, const float value1, const float value2, const float value3, const float value4) const;
	void set(const char* name, const glm::vec2 &value) const;
	void set(const char* name, const glm::vec3 &value) const;
	void set(const char* name, const glm::vec4 &value) const;
	void set(const char* name, const glm::mat2 &value) const;
	void set(const char* name, const glm::mat3 &value) const;
	void set(const char* name, const glm::mat4 &value) const;
private:
	void checkErrors(const uint32_t shader, const Type type) const;
	void loadShader(const char* path, std::string* code) const;

	std::vector<Shader> list_;
	uint32_t id_;
};

#endif