#ifndef TEXTURE_ATLAS_H
#define TEXTURE_ATLAS_H

#include "Texture.h"

#include <string>

// Texture Atlas which holds and manages a texture which can be split into a uniform grid of cells
class TextureAtlas
{
public:
	// static variable holding the total count of atlases, used to assign texture units
	static int m_AtlasCount;

	TextureAtlas() = default;

	// create a texture atlas with a given path to an image, and the atlas dimentions
	// atlasWidth and atlasHeight are in cell units. 
	// ex: a 2x3 atlas would have 2 and 3 as dimentions
	TextureAtlas(std::string imagePath, int atlasWidth = 1, int atlasHeight = 1);

	// get texture coordinates and dimentions for a certain atlas cell.
	// return quad is in (x, y, width, height) format
	glm::vec4 GetQuad(unsigned int cellIndex = 0);

	// bind the stored texture to it's stored texture unit
	// the creation of the texture atlas itself calls Texture::Bind()
	void Bind();

public:
	// gets the texture dimentions in cells
	glm::vec2 GetAtlasDimentions();
	// gets the texture width in cells
	int GetAtlasWidth();
	// gets the texture height in cells
	int GetAtlasHeight();

	// gets the texture dimentions in pixels
	glm::vec2 GetTextureDimentions();
	// gets the texture width in pixels
	int GetTextureWidth();
	// gets the texture height in pixels
	int GetTextureHeight();

	// return the stored ID/texture unit
	int GetID();

private:
	// reference to the actual image texture
	Texture m_Texture;
	// this ID acts as the atlas' assigned texture slot
	int m_AtlasID = -1;

	// dimentions of the atlas in cells
	int m_AtlasWidth = 0;
	int m_AtlasHeight = 0;

	// dimentions of the cells in pixels
	int m_CellWidth = 0;
	int m_CellHeight = 0;

	// dimentions of the atlas in pixels
	int m_TextureWidth = 0;
	int m_TextureHeight = 0;
};

#endif