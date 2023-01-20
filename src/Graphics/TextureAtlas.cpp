#include "TextureAtlas.h"

int TextureAtlas::m_AtlasCount = 0;

TextureAtlas::TextureAtlas(std::string imagePath, int slotWidth, int slotHeight)
	: m_AtlasWidth(slotWidth), m_AtlasHeight(slotHeight)
{
	// assign this atlas it's ID
	m_AtlasID = m_AtlasCount;

	// increment the static atlas count
	m_AtlasCount++;

	// create texture
	m_Texture = Texture(imagePath.c_str(), true, m_AtlasID);

	// copy and store the texture dimentions
	m_TextureWidth = m_Texture.GetWidth();
	m_TextureHeight = m_Texture.GetHeight();

	// calculate the individual cell dimentions from texture and atlas dimentions
	m_CellWidth = m_TextureWidth / m_AtlasWidth;
	m_CellHeight = m_TextureHeight / m_AtlasHeight;
}

glm::vec4 TextureAtlas::GetQuad(unsigned int cellIndex)
{
	// Get texture index in the form of a coordinate
	glm::vec2 coord = glm::vec2(
		cellIndex % m_AtlasWidth,
		cellIndex / m_AtlasWidth
	);

	// Calculate the resulting quad
	glm::vec4 quad = glm::vec4(
		m_CellWidth * coord.x,		// starting x value
		m_CellHeight * coord.y,		// starting y value
		m_CellWidth, m_CellHeight  // quad width, height
	);

	return quad;
}

void TextureAtlas::Bind()
{
	m_Texture.Bind(m_AtlasID);
}

glm::vec2 TextureAtlas::GetAtlasDimentions()
{
	return glm::vec2(m_AtlasWidth, m_AtlasHeight);
}

int TextureAtlas::GetAtlasWidth()
{
	return m_AtlasWidth;
}

int TextureAtlas::GetAtlasHeight()
{
	return m_AtlasHeight;
}

glm::vec2 TextureAtlas::GetTextureDimentions()
{
	return glm::vec2(m_TextureWidth, m_TextureHeight);
}

int TextureAtlas::GetTextureWidth()
{
	return m_TextureWidth;
}

int TextureAtlas::GetTextureHeight()
{
	return m_TextureHeight;
}

int TextureAtlas::GetID()
{
	return m_AtlasID;
}

