#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class UBO
{
public:
	UBO(unsigned int size, unsigned int binding, const char* name);

	void SetData(float* dataPointer);

public:
	unsigned int m_ID = 0;
	unsigned int m_BindingSite = 0;
	const char* m_Name;
	unsigned int m_Size = 0;
};

class Shader
{
public:
	Shader() = default;
	Shader(const char* vertexPath, const char* fragmentPath);
	unsigned int getID();

	void use();

	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;
	void setMat4(const std::string& name, glm::mat4 matrix) const;
	void setVec2(const std::string& name, glm::vec2 vec2) const;
	void setVec3(const std::string& name, glm::vec3 vec3) const;
	void setVec4(const std::string& name, glm::vec4 vec4) const;
	void setUBO(const std::string& name, unsigned int bindingNumber) const;
	void setUBO(UBO& ubo) const;

private:
	unsigned int m_ID;
};

#endif