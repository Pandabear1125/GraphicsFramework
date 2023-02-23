#ifndef TEXTURE_RENDERER_H
#define TEXTURE_RENDERER_H

#include "Shader.h"
#include "TextureAtlas.h"

#include <vector>

const int VERTEX_FLOAT_COUNT = 32;
const int INDEX_UINT_COUNT = 6;
const int VERTICES_PER_QUAD = 4;


struct CompiledRenderData
{
	CompiledRenderData() = default;

	CompiledRenderData(int initialSize)
	{
		Vertices.resize(initialSize * VERTEX_FLOAT_COUNT);
		Indices.resize(initialSize * INDEX_UINT_COUNT);
		Capacity = initialSize;
	}

	CompiledRenderData(std::vector<float> vertexVector, std::vector<unsigned int> indexVector, unsigned int count)
	{
		Vertices = vertexVector;
		Indices = indexVector;
		Count = count;
	}

	inline void Add(CompiledRenderData& data)
	{
		int oldCount = Count;
		Count += data.Count;

		Vertices.resize(Count * VERTEX_FLOAT_COUNT);
		for (unsigned int i = 0; i < data.Count * VERTEX_FLOAT_COUNT; i++)
		{
			Vertices[oldCount * VERTEX_FLOAT_COUNT + i] = data.Vertices[i];
		}

		Indices.resize(Count * INDEX_UINT_COUNT);
		for (unsigned int i = 0; i < data.Count * INDEX_UINT_COUNT; i++)
		{
			Indices[oldCount * INDEX_UINT_COUNT + i] = data.Indices[i] + 4 * oldCount;
		}

		Capacity = Count;
	}

	void Set(CompiledRenderData& data, unsigned int start)
	{
		for (unsigned int i = 0; i < data.Count * VERTEX_FLOAT_COUNT; i++)
		{
			Vertices[i + start * VERTEX_FLOAT_COUNT] = data.Vertices[i];
		}

		for (unsigned int i = 0; i < data.Count * INDEX_UINT_COUNT; i++)
		{
			Indices[i + start * INDEX_UINT_COUNT] = data.Indices[i] + VERTICES_PER_QUAD * start;
		}
	}

	inline CompiledRenderData operator+ (CompiledRenderData b)
	{
		CompiledRenderData c;
		c.Add(*(this));
		c.Add(b);
		return c;
	}

	inline void operator+= (CompiledRenderData b)
	{
		this->Add(b);
	}

	std::vector<float> Vertices;
	std::vector<unsigned int> Indices;
	unsigned int Count = 0;
	unsigned int Capacity = 0;
};

// renderer for drawing textures from TextureAtlas objects
class TextureRenderer
{
public:
	TextureRenderer() = default;
	// initialize a renderer instance using a valid shader
	TextureRenderer(Shader* shader);

	// add an image to be drawn
	void Draw(TextureAtlas* atlas, glm::vec4 calculatedQuad, float x, float y, float width, float height, float rotation = 0.f, float rotationOffsetX = 0.f, float rotationOffsetY = 0.f, float red = 1.f, float green = 1.f, float blue = 1.f);
	void Draw(TextureAtlas* atlas, unsigned int atlasIndex, float x, float y, float width, float height, float rotation = 0.f, float rotationOffsetX = 0.f, float rotationOffsetY = 0.f, float red = 1.f, float green = 1.f, float blue = 1.f);
	// adds all compiled draw data to the renderer
	void Draw(CompiledRenderData& container);
	
	// add image data to a CompiledRenderData to be drawn when loaded
	void CompileStatic(CompiledRenderData& container, TextureAtlas* atlas, glm::vec4 calculatedQuad, float x, float y, float width, float height, float rotation = 0.f, float rotationOffsetX = 0.f, float rotationOffsetY = 0.f, float red = 1.f, float green = 1.f, float blue = 1.f);
	void CompileStatic(CompiledRenderData& container, TextureAtlas* atlas, unsigned int index, float x, float y, float width, float height, float rotation = 0.f, float rotationOffsetX = 0.f, float rotationOffsetY = 0.f, float red = 1.f, float green = 1.f, float blue = 1.f);
	
	// load all draw data in a CompiledRenderData to be drawn
	void LoadStaticData(CompiledRenderData& container);
	// clear any loaded static draw data
	void ClearStaticData();

	// render all added images/quads to the screen
	void Render();

private:
	// private helper functions
	void ResizeVertexVector(int newSize);
	void ResizeIndexVector(int newSize);
	void TryToResize(int changeInSize);

	void ResetVectors();

private:
	Shader* m_Shader = nullptr;

	// opengl specific members
	unsigned int m_VAO, m_VBO, m_EBO;
	unsigned int m_StaticVAO, m_StaticVBO, m_StaticEBO;

	static const int VERTEX_FLOAT_COUNT = 32;
	static const int INDEX_UINT_COUNT = 6;

	// data storage members
	std::vector<float> m_VertexData;
	std::vector<unsigned int> m_IndexData;

	std::vector<float> m_StaticVertices;
	std::vector<unsigned int> m_StaticIndices;

	int m_DrawCount = 0;
	int m_DrawCapacity = 0;

	int m_StaticCount = 0;
	int m_StaticCapacity = 0;
};

#endif