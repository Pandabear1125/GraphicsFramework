#include "Shader.h"

bool verifyProgramSuccess(unsigned int programID);
void verifyShaderSuccess(unsigned int shaderID);

UBO::UBO(unsigned int size, unsigned int binding, const char* name)
	: m_BindingSite(binding), m_Name(name), m_Size(size)
{
	// create the UBO
	glGenBuffers(1, &m_ID);

	// set the UBO and initialize it
	glBindBuffer(GL_UNIFORM_BUFFER, m_ID);
	glBufferData(GL_UNIFORM_BUFFER, m_Size, NULL, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	glBindBufferRange(GL_UNIFORM_BUFFER, m_BindingSite, m_ID, 0, m_Size);
}

void UBO::SetData(float* dataPointer)
{
	// bind ubo and fill it with 'dataPointer'
	glBindBuffer(GL_UNIFORM_BUFFER, m_ID);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, m_Size, dataPointer);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}



Shader::Shader(const char* vertexPath, const char* fragmentPath)
	: m_ID(0)
{
	// get the source code from each filepath
	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;

	// enable exception throwing
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	// read and store data
	try
	{
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		std::stringstream vShaderStream, fShaderStream;

		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();

		vShaderFile.close();
		fShaderFile.close();

		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "Fatal Error: Shader read unsuccessful. Files: " << vertexPath << " | " << fragmentPath << std::endl;
	}

	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();

	// compile shaders
	unsigned int vertexID, fragmentID;

	vertexID = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexID, 1, &vShaderCode, NULL);
	glCompileShader(vertexID);
	verifyShaderSuccess(vertexID);

	fragmentID = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentID, 1, &fShaderCode, NULL);
	glCompileShader(fragmentID);
	verifyShaderSuccess(fragmentID);

	// link program
	m_ID = glCreateProgram();
	glAttachShader(m_ID, vertexID);
	glAttachShader(m_ID, fragmentID);
	glLinkProgram(m_ID);
	if (verifyProgramSuccess(m_ID))
	{
		std::cout << "Shader loaded: " << vertexPath << " | " << fragmentPath << std::endl;
	}
	else
		abort();

	glDeleteShader(vertexID);
	glDeleteShader(fragmentID);
}

unsigned int Shader::getID()
{
	return m_ID;
}

void Shader::use()
{
	glUseProgram(m_ID);
}

void Shader::setBool(const std::string& name, bool value) const
{
	int location = glGetUniformLocation(m_ID, name.c_str());
	if (location < 0)
	{
		std::cout << "Error: Uniform '" << name << "' not found!" << std::endl;
	}
	glUniform1i(location, (int)value);
}

void Shader::setInt(const std::string& name, int value) const
{
	int location = glGetUniformLocation(m_ID, name.c_str());
	if (location < 0)
	{
		std::cout << "Error: Uniform '" << name << "' not found!" << std::endl;
	}
	glUniform1i(location, value);
}

void Shader::setFloat(const std::string& name, float value) const
{
	int location = glGetUniformLocation(m_ID, name.c_str());
	if (location < 0)
	{
		std::cout << "Error: Uniform '" << name << "' not found!" << std::endl;
	}
	glUniform1f(location, value);
}

void Shader::setMat4(const std::string& name, glm::mat4 matrix) const
{
	int location = glGetUniformLocation(m_ID, name.c_str());
	if (location < 0)
	{
		std::cout << "Error: Uniform '" << name << "' not found!" << std::endl;
	}
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::setVec2(const std::string& name, glm::vec2 vec2) const
{
	int location = glGetUniformLocation(m_ID, name.c_str());
	if (location < 0)
	{
		std::cout << "Error: Uniform '" << name << "' not found!" << std::endl;
	}
	glUniform2f(location, vec2.x, vec2.y);
}

void Shader::setVec3(const std::string& name, glm::vec3 vec3) const
{
	int location = glGetUniformLocation(m_ID, name.c_str());
	if (location < 0)
	{
		std::cout << "Error: Uniform '" << name << "' not found!" << std::endl;
	}
	glUniform3f(location, vec3.x, vec3.y, vec3.z);
}

void Shader::setVec4(const std::string& name, glm::vec4 vec4) const
{
	int location = glGetUniformLocation(m_ID, name.c_str());
	if (location < 0)
	{
		std::cout << "Error: Uniform '" << name << "' not found!" << std::endl;
	}
	glUniform4f(location, vec4.x, vec4.y, vec4.z, vec4.w);
}

void Shader::setUBO(const std::string& name, unsigned int bindingNumber) const
{
	unsigned int u_Location = glGetUniformBlockIndex(m_ID, name.c_str());
	if (u_Location == GL_INVALID_INDEX)
		std::cout << "Error: Uniform Buffer '" << name << "' not found!" << std::endl;

	glUniformBlockBinding(m_ID, u_Location, bindingNumber);
}

void Shader::setUBO(UBO& ubo) const
{
	unsigned int u_Location = glGetUniformBlockIndex(m_ID, ubo.m_Name);
	if (u_Location == GL_INVALID_INDEX)
		std::cout << "Error: Uniform Buffer '" << ubo.m_Name << "' not found! UBO class likely malformed." << std::endl;

	glUniformBlockBinding(m_ID, u_Location, ubo.m_BindingSite);
}

void verifyShaderSuccess(unsigned int shaderID)
{
	int success;
	int shaderType;
	char infoLog[512];
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
	glGetShaderiv(shaderID, GL_SHADER_TYPE, &shaderType);

	if (!success)
	{
		glGetShaderInfoLog(shaderID, 512, NULL, infoLog);
		std::cout << "Fatal Error: ";
		switch (shaderType)
		{
		case GL_VERTEX_SHADER:
			std::cout << "Vertex Shader Failed to Compile. (" << infoLog << ")" << std::endl;
			break;
		case GL_FRAGMENT_SHADER:
			std::cout << "Fragment Shader Failed to Compile. (" << infoLog << ")" << std::endl;
			break;
		default:
			std::cout << "Shader failed. Unknown Type. (" << infoLog << std::endl;
			break;
		}
	}
}

bool verifyProgramSuccess(unsigned int programID)
{
	int success;
	char infoLog[512];
	glGetProgramiv(programID, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(programID, 512, NULL, infoLog);
		std::cout << "Fatal Error: Shader Program Link Failure. (" << infoLog << ")" << std::endl;
		return false;
	}
	else
		return true;
}
