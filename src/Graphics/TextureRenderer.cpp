#include "TextureRenderer.h"

TextureRenderer::TextureRenderer(Shader* shader)
	: m_Shader(shader)
{
	// configure data vectors for initial capacity
	// update new m_DrawCapacity
	m_DrawCapacity += 10;
	// resize by given value * VERTEX_FLOAT_COUNT values, since every quad is VERTEX_FLOAT_COUNT floats
	ResizeVertexVector(m_DrawCapacity * VERTEX_FLOAT_COUNT);
	// resize by given value * INDEX_UINT_COUNT values, since every quad is INDEX_UINT_COUNT uints
	ResizeIndexVector(m_DrawCapacity * INDEX_UINT_COUNT);

	// generate vao and buffers
	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_EBO);

	// bind vao
	glBindVertexArray(m_VAO);

	// bind and configure vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * m_VertexData.size(), NULL, GL_DYNAMIC_DRAW);
	// screen and texture position
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 8, (void*)(0));
	// color offset
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 8, (void*)(4 * sizeof(GL_FLOAT)));
	// texture index
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 8, (void*)(7 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	// bind and configure index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GL_UNSIGNED_INT) * m_IndexData.size(), NULL, GL_DYNAMIC_DRAW);

	// unbind vao
	glBindVertexArray(0);

	glGenVertexArrays(1, &m_StaticVAO);
	glGenBuffers(1, &m_StaticVBO);
	glGenBuffers(1, &m_StaticEBO);

	glBindVertexArray(m_StaticVAO);
	
	glBindBuffer(GL_ARRAY_BUFFER, m_StaticVBO);
	glBufferData(GL_ARRAY_BUFFER, NULL, NULL, GL_STATIC_DRAW);
	// screen and texture position
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 8, (void*)(0));
	// color offset
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 8, (void*)(4 * sizeof(GL_FLOAT)));
	// texture index
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 8, (void*)(7 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	// bind and configure index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_StaticEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, NULL, NULL, GL_STATIC_DRAW);

	glBindVertexArray(0);

	// specify the texture sampler array with good values
	// currently only 16 texture units are supported
	m_Shader->use();
	auto location = glGetUniformLocation(m_Shader->getID(), "u_Textures");
	int textures[16] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
	glUniform1iv(location, 16, textures);
}

void TextureRenderer::Draw(TextureAtlas* atlas, unsigned int atlasIndex, float x, float y, float width, float height, float rotation, float rotationOffsetX, float rotationOffsetY, float red, float green, float blue)
{
	// get the texture quad
	glm::vec4 quad = atlas->GetQuad(atlasIndex);

	Draw(atlas, quad, x, y, width, height, rotation, rotationOffsetX, rotationOffsetY, red, green, blue);
}

void TextureRenderer::CompileStatic(StaticRenderContainer& container, TextureAtlas* atlas, glm::vec4 calculatedQuad, float x, float y, float width, float height, float rotation, float rotationOffsetX, float rotationOffsetY, float red, float green, float blue)
{
	// get the textureID to be added to the vertex data
	float textureID = (float)atlas->GetID();

	// get the texture quad
	glm::vec4 quad = calculatedQuad;

	// assemble the vertex data
	float vertices[VERTEX_FLOAT_COUNT] = {
		//	|x|					|y|						|u|															|v|																	 |color|					|tex unit|	
			x - width / 2, y - height / 2,	(quad.x) / atlas->GetTextureWidth(),	(quad.y) / atlas->GetTextureHeight(),	 red, green, blue,	textureID,		// 0 : top left
			x + width / 2, y - height / 2,	(quad.x + quad.z) / atlas->GetTextureWidth(),	(quad.y) / atlas->GetTextureHeight(),	 red, green, blue,	textureID,		// 1 : top right
			x - width / 2, y + height / 2,	(quad.x) / atlas->GetTextureWidth(),	(quad.y + quad.w) / atlas->GetTextureHeight(),	 red, green, blue,	textureID,		// 2 : bottom left
			x + width / 2, y + height / 2,	(quad.x + quad.z) / atlas->GetTextureWidth(),	(quad.y + quad.w) / atlas->GetTextureHeight(),	 red, green, blue,	textureID		// 3 : bottom right
	};

	// rotate vertex data if rotation is provided
	if (rotation != 0.f)
	{
		float cosX = cosf(rotation);
		float sinX = sinf(rotation);

		// rotate vertices
		for (int i = 0; i < VERTEX_FLOAT_COUNT; i += 8)
		{
			float origX = vertices[i];
			float origY = vertices[i + 1];
			vertices[i] = cosX * (origX - x - rotationOffsetX) - sinX * (origY - y - rotationOffsetY) + x + rotationOffsetX;
			vertices[i + 1] = sinX * (origX - x - rotationOffsetX) + cosX * (origY - y - rotationOffsetY) + y + rotationOffsetY;
		}
	}

	// assemble the indices
	unsigned int indices[INDEX_UINT_COUNT] = {
		0, 1, 3,		// top right triangle
		3, 2, 0		// bottom left triangle
	};

	if (container.Count >= container.Capacity)
	{
		container.Capacity += 100;
		container.Vertices.resize(container.Capacity * VERTEX_FLOAT_COUNT);
		container.Indices.resize(container.Capacity * INDEX_UINT_COUNT);
	}

	// store the vertex data
	for (int i = 0; i < VERTEX_FLOAT_COUNT; i++)
	{
		container.Vertices[container.Count * VERTEX_FLOAT_COUNT + i] = vertices[i];
	}

	// store the index data
	for (int i = 0; i < INDEX_UINT_COUNT; i++)
	{
		container.Indices[container.Count * INDEX_UINT_COUNT + i] = indices[i] + (container.Count * VERTICES_PER_QUAD);
	}

	container.Count++;
}

void TextureRenderer::CompileStatic(StaticRenderContainer& container, TextureAtlas* atlas, unsigned int index, float x, float y, float width, float height, float rotation, float rotationOffsetX, float rotationOffsetY, float red, float green, float blue)
{
	glm::vec4 quad = atlas->GetQuad(index);
	CompileStatic(container, atlas, quad, x, y, width, height, rotation, rotationOffsetX, rotationOffsetY, red, green, blue);
}

void TextureRenderer::Draw(TextureAtlas* atlas, glm::vec4 calculatedQuad, float x, float y, float width, float height, float rotation, float rotationOffsetX, float rotationOffsetY, float red, float green, float blue)
{
	// get the textureID to be added to the vertex data
	float textureID = (float)atlas->GetID();

	// get the texture quad
	glm::vec4 quad = calculatedQuad;

	// assemble the vertex data
	float vertices[VERTEX_FLOAT_COUNT] = {
	//	|x|					|y|						|u|															|v|																	 |color|					|tex unit|	
		x - width / 2, y - height / 2,	(quad.x			 )	/ atlas->GetTextureWidth(),	(quad.y			 ) / atlas->GetTextureHeight(),	 red, green, blue,	textureID,		// 0 : top left
		x + width / 2, y - height / 2,	(quad.x + quad.z) / atlas->GetTextureWidth(),	(quad.y			 ) / atlas->GetTextureHeight(),	 red, green, blue,	textureID,		// 1 : top right
		x - width / 2, y + height / 2,	(quad.x			 )	/ atlas->GetTextureWidth(),	(quad.y + quad.w) / atlas->GetTextureHeight(),	 red, green, blue,	textureID,		// 2 : bottom left
		x + width / 2, y + height / 2,	(quad.x + quad.z) / atlas->GetTextureWidth(),	(quad.y + quad.w) / atlas->GetTextureHeight(),	 red, green, blue,	textureID		// 3 : bottom right
	};
	
	// rotate vertex data if rotation is provided
	if (rotation != 0.f)
	{
		float cosX = cosf(rotation);
		float sinX = sinf(rotation);

		// rotate vertices
		for (int i = 0; i < VERTEX_FLOAT_COUNT; i += 8)
		{
			float origX = vertices[i];
			float origY = vertices[i + 1];
			vertices[i] = cosX * (origX - x - rotationOffsetX) - sinX * (origY - y - rotationOffsetY) + x + rotationOffsetX;
			vertices[i + 1] = sinX * (origX - x - rotationOffsetX) + cosX * (origY - y - rotationOffsetY) + y + rotationOffsetY;
		}
	}

	// assemble the indices
	unsigned int indices[INDEX_UINT_COUNT] = {
		0, 1, 3,		// top right triangle
		3, 2, 0		// bottom left triangle
	};

	// call resize on the vectors if vectors are about to overflow
	TryToResize(10);

	// store the vertex data
	for (int i = 0; i < VERTEX_FLOAT_COUNT; i++)
	{
		m_VertexData[m_DrawCount * VERTEX_FLOAT_COUNT + i] = vertices[i];
	}
	
	// store the index data
	for (int i = 0; i < INDEX_UINT_COUNT; i++)
	{
		m_IndexData[m_DrawCount * INDEX_UINT_COUNT + i] = indices[i] + (m_DrawCount * VERTICES_PER_QUAD);
	}

	m_DrawCount++;
}

void TextureRenderer::ClearStaticData()
{
	glBindVertexArray(m_StaticVAO);
	
	glBindBuffer(GL_ARRAY_BUFFER, m_StaticVBO);
	glBufferData(GL_ARRAY_BUFFER, NULL, NULL, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_StaticEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, NULL, NULL, GL_STATIC_DRAW);

	m_StaticCount = 0;
}

void TextureRenderer::LoadStaticData(StaticRenderContainer& container)
{
	m_StaticCount = container.Count;

	glBindVertexArray(m_StaticVAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_StaticVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * container.Count * VERTEX_FLOAT_COUNT, container.Vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_StaticEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GL_UNSIGNED_INT) * container.Count * INDEX_UINT_COUNT, container.Indices.data(), GL_STATIC_DRAW);
}

void TextureRenderer::Render()
{
	// bind shader
	m_Shader->use();

	// bind static vao
	glBindVertexArray(m_StaticVAO);

	// bind static buffers
	glBindBuffer(GL_ARRAY_BUFFER, m_StaticVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_StaticEBO);

	// draw static data
	glDrawElements(GL_TRIANGLES, m_StaticCount * INDEX_UINT_COUNT, GL_UNSIGNED_INT, 0);

	// bind vao
	glBindVertexArray(m_VAO);

	// bind and fill vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GL_FLOAT) * VERTEX_FLOAT_COUNT * m_DrawCount, m_VertexData.data());

	// bind and fill index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(GL_UNSIGNED_INT) * INDEX_UINT_COUNT * m_DrawCount, m_IndexData.data());

	// issue draw call
	glDrawElements(GL_TRIANGLES, m_DrawCount * INDEX_UINT_COUNT, GL_UNSIGNED_INT, 0);

	// clean up vectors
	ResetVectors();
}

void TextureRenderer::ResizeVertexVector(int newSize)
{
	m_VertexData.resize(newSize);
}

void TextureRenderer::ResizeIndexVector(int newSize)
{
	m_IndexData.resize(newSize);
}

void TextureRenderer::TryToResize(int changeInSize)
{
	if (m_DrawCount >= m_DrawCapacity)
	{
		// bind vao
		glBindVertexArray(m_VAO);

		// update new m_DrawCapacity
		m_DrawCapacity += changeInSize;

		// resize by given value * VERTEX_FLOAT_COUNT values, since every quad is VERTEX_FLOAT_COUNT floats
		ResizeVertexVector(m_DrawCapacity * VERTEX_FLOAT_COUNT);
		// resize vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * m_DrawCapacity * VERTEX_FLOAT_COUNT, NULL, GL_DYNAMIC_DRAW);

		// resize by given value * INDEX_UINT_COUNT values, since every quad is INDEX_UINT_COUNT uints
		ResizeIndexVector(m_DrawCapacity * INDEX_UINT_COUNT);
		// resize index buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GL_UNSIGNED_INT) * m_DrawCapacity * INDEX_UINT_COUNT, NULL, GL_DYNAMIC_DRAW);
	}
}

void TextureRenderer::ResetVectors()
{
	m_VertexData.clear();
	m_VertexData.resize(m_DrawCapacity * VERTEX_FLOAT_COUNT);

	m_IndexData.clear();
	m_IndexData.resize(m_DrawCapacity * INDEX_UINT_COUNT);
	m_DrawCount = 0;
}
