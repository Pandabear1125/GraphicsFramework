#include "Shader.h"

bool verifyProgramSuccess(unsigned int programID);
void verifyShaderSuccess(unsigned int shaderID);



const char* SHADER_SHAPE_VERT = "#version 330 core\nlayout(location = 0) in vec2 V_Position;\nlayout(location = 1) in vec3 V_Color;\nlayout(std140) uniform Matrices\n{\n	mat4 VP;\n};\nout vec3 color;\nvoid main()\n{\n	gl_Position = VP * vec4(V_Position, 0, 1);\n	color = V_Color;\n}";

const char* SHADER_SHAPE_FRAG = "#version 330 core\nout vec4 FragColor;\nin vec3 color;\nvoid main()\n{\nFragColor = vec4(color, 1.0);\n}";

const char* SHADER_TEXTURE_RENDER_VERT = "#version 330 core\nlayout(location = 0) in vec4 v_ScreenPosition;\nlayout(location = 1) in vec3 v_ColorOffset;\nlayout(location = 2) in float v_TextureIndex;\nout vec2 f_TextureCoord;\nout vec3 f_ColorOffset;\nflat out int f_TextureIndex;\nlayout(std140) uniform Matrices\n{\n	mat4 vp;\n};\nvoid main()\n{\n	gl_Position = vp * vec4(v_ScreenPosition.xy, 0, 1);\n	f_TextureCoord = v_ScreenPosition.zw;\n	f_TextureIndex = int(v_TextureIndex);\n	f_ColorOffset = v_ColorOffset;\n}";

const char* SHADER_TEXTURE_RENDER_FRAG = "#version 330 core\nin vec2 f_TextureCoord;\nin vec3 f_ColorOffset;\nflat in int f_TextureIndex;\nout vec4 r_FragColor;\nuniform sampler2D u_Textures[16];\nvoid main()\n{\n	int index = f_TextureIndex;\n	if (index == 0)\n		r_FragColor = texture(u_Textures[0], f_TextureCoord);\n	else if (index == 1)\n		r_FragColor = texture(u_Textures[1], f_TextureCoord);\n	else if (index == 2)\n		r_FragColor = texture(u_Textures[2], f_TextureCoord);\n	else if (index == 3)\n		r_FragColor = texture(u_Textures[3], f_TextureCoord);\n	else if (index == 4)\n		r_FragColor = texture(u_Textures[4], f_TextureCoord);\n	else if (index == 5)\n		r_FragColor = texture(u_Textures[5], f_TextureCoord);\n	else if (index == 6)\n		r_FragColor = texture(u_Textures[6], f_TextureCoord);\n	else if (index == 7)\n		r_FragColor = texture(u_Textures[7], f_TextureCoord);\n	else if (index == 8)\n		r_FragColor = texture(u_Textures[8], f_TextureCoord);\n	else if (index == 9)\n		r_FragColor = texture(u_Textures[9], f_TextureCoord);\n	else if (index == 10)\n		r_FragColor = texture(u_Textures[10], f_TextureCoord);\n	else if (index == 11)\n		r_FragColor = texture(u_Textures[11], f_TextureCoord);\n	else if (index == 12)\n		r_FragColor = texture(u_Textures[12], f_TextureCoord);\n	else if (index == 13)\n		r_FragColor = texture(u_Textures[13], f_TextureCoord);\n	else if (index == 14)\n		r_FragColor = texture(u_Textures[14], f_TextureCoord);\n	else if (index == 15)\n		r_FragColor = texture(u_Textures[15], f_TextureCoord);\n	else\n		r_FragColor = vec4(1, 0, 1, 1);\n	r_FragColor = r_FragColor * vec4(f_ColorOffset, 1);\n}";



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

Shader::Shader(ShaderType premadeType)
	: m_ID(0)
{
	const char* vShaderCode;
	const char* fShaderCode;

	// read and store data
	switch (premadeType)
	{
	case SHAPE:
	{
		vShaderCode = SHADER_SHAPE_VERT;
		fShaderCode = SHADER_SHAPE_FRAG;
		break;
	}
	case TEXTURE_RENDERER:
	{
		vShaderCode = SHADER_TEXTURE_RENDER_VERT;
		fShaderCode = SHADER_TEXTURE_RENDER_FRAG;
		break;
	}
	default:
		break;
	}

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
		std::cout << "Premade Shader Loaded: " << (int)premadeType << std::endl;
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
