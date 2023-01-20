#include "Texture.h"

#include <iostream>

Texture::Texture(const char* filepath, bool hasAlpha, int texUnit)
	: m_ID(0), m_Width(0), m_Height(0), m_NumChannels(0), m_TexUnit(texUnit)
{
	// create and bind texture 
	glActiveTexture(GL_TEXTURE0 + m_TexUnit);
	glGenTextures(1, &m_ID);
	glBindTexture(GL_TEXTURE_2D, m_ID);

	// set texture attributes
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// create image data with stb_image
	unsigned char* data = stbi_load(filepath, &m_Width, &m_Height, &m_NumChannels, 0);
	if (data)
	{
		if (hasAlpha)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		else
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_Width, m_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

		glGenerateMipmap(GL_TEXTURE_2D);
		std::cout << "Texture Loaded: " << filepath << std::endl;
	}
	else
		std::cout << "Fatal Error: Failed to load texture: " << filepath << std::endl;

	stbi_image_free(data);
}

void Texture::Bind(int texUnit)
{
	if (texUnit == -1)
	{
		glActiveTexture(GL_TEXTURE0 + m_TexUnit);
		glBindTexture(GL_TEXTURE_2D, m_ID);
	}
	else
	{
		m_TexUnit = texUnit;
		glActiveTexture(GL_TEXTURE0 + m_TexUnit);
		glBindTexture(GL_TEXTURE_2D, m_ID);
	}
}

void Texture::Unbind()
{
	glActiveTexture(GL_TEXTURE0 + m_TexUnit);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::Clean() const
{
	glActiveTexture(GL_TEXTURE0 + m_TexUnit);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDeleteTextures(1, &m_ID);
}

unsigned int Texture::GetID()
{
	return m_ID;
}

int Texture::GetTexUnit()
{
	return m_TexUnit;
}

void Texture::SetTexUnit(int texUnit)
{
	m_TexUnit = texUnit;
}

glm::vec2 Texture::GetDimentions()
{
	return glm::vec2((float)m_Width, (float)m_Height);
}

int Texture::GetWidth()
{
	return m_Width;
}

int Texture::GetHeight()
{
	return m_Height;
}
