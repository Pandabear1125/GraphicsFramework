#include "Graphics.h"

namespace Graphics {

	// basic vertices that are reused/transformed
	static const float QuadVertices[16] = {
		 0.f,  0.f,  0.0f,  0.0f, // 0
		 0.f,  1.f,  0.0f,  1.0f, // 1
		 1.f,  0.f,  1.0f,  0.0f, // 2
		 1.f,  1.f,  1.0f,  1.0f  // 3
	};

	GraphicsData Data;

	void Init(Shader* shapeShader, Shader* renderShader)
	{
		Data.RenderShader = renderShader;
		Data.Renderer = new TextureRenderer(renderShader);

		// Init shader
		Data.ShapeShader = shapeShader;
		
		// Create vertex array
		glGenVertexArrays(1, &Data.VAO);

		// Create vbo
		glGenBuffers(1, &Data.VBO);

		// Bind vertex array
		glBindVertexArray(Data.VAO);

		// Bind and set up vbo
		glBindBuffer(GL_ARRAY_BUFFER, Data.VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * 2, NULL, GL_DYNAMIC_DRAW);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 2, (void*)0);
		glEnableVertexAttribArray(0);

		// Create color vbo
		glGenBuffers(1, &Data.ColorVBO);

		// bind and set up color vbo
		glBindBuffer(GL_ARRAY_BUFFER, Data.ColorVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * 3, NULL, GL_DYNAMIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT) * 3, (void*)0);
		glEnableVertexAttribArray(1);

		// Clean up and unbind everything
		glBindVertexArray(0);
	}

	void LoadFont(std::string fontImagePath, std::string fontDataPath)
	{
		int ImageWidth = 0;
		int ImageHeight = 0;

		// read from font files and assemble Data values
		std::ifstream fin;
		fin.open(fontDataPath);
		std::string line;
		std::string output[5];
		std::string suboutput[5];

		while (std::getline(fin, line)) // read through whole file
		{
			split(line, ',', output, 5); // process line

			// assign the result to the correct Data value
			if (output[0] == "Cell Width")
			{
				Data.FontCellWidth = std::stoi(output[1]);
				continue;
			}
			else if (output[0] == "Cell Height")
			{
				Data.FontCellHeight = std::stoi(output[1]);
				continue;
			}
			else if (output[0] == "Start Char")
			{
				Data.FontStartingChar = std::stoi(output[1]);
				continue;
			}
			else if (output[0] == "Image Width")
			{
				ImageWidth = std::stoi(output[1]);
				continue;
			}
			else if (output[0] == "Image Height")
			{
				ImageHeight = std::stoi(output[1]);
				continue;
			}
			// if this line is a char width field, process again
			split(output[0], ' ', suboutput, 5);
			if (suboutput[0] == "Char" && suboutput[2] == "Base")
			{
				Data.FontWidths[std::stoi(suboutput[1])] = std::stoi(output[1]);
			}
		}

		Data.FontAtlas = new TextureAtlas(fontImagePath, ImageWidth / Data.FontCellWidth, ImageHeight / Data.FontCellHeight);
	}

	TextureRenderer* GetRenderer()
	{
		return Data.Renderer;
	}

	void Polygon(DrawMode mode, float *vertices, unsigned int vertexCount, float red, float green, float blue)
	{
		unsigned int triangleCount;
		// Bind vao
		glBindVertexArray(Data.VAO);

		// Fill vbo with vertex data
		glBindBuffer(GL_ARRAY_BUFFER, Data.VBO);
		if (mode == FILL)
		{	// FILL mode needs to triangulate the polygon
			// get vertex data from given polygon by triangulating it
			std::vector<float> vertexData = TriangulatePolygon(vertices, vertexCount);
			triangleCount = (unsigned int)vertexData.size() / 6;
			// resize and fill vert buffer with triangulated vertex data
			glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * vertexData.size(), vertexData.data(), GL_DYNAMIC_DRAW);
			// bind and turn on vertex attrib divisor
			glBindBuffer(GL_ARRAY_BUFFER, Data.ColorVBO);
			glVertexAttribDivisor(1, 1);

			// assign and fill color buffer
			float color[3] = { red, green, blue };
			glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * 3, color, GL_DYNAMIC_DRAW);
		}
		else
		{ // LINE mode only plugs in the vertex data as is
			// bind vbo and fill data
			glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * vertexCount * 2, vertices, GL_DYNAMIC_DRAW);
			// bind and turn on vertex attrib divisor
			glBindBuffer(GL_ARRAY_BUFFER, Data.ColorVBO);
			glVertexAttribDivisor(1, 1);

			float color[3] = { red, green, blue };
			glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * 3, color, GL_DYNAMIC_DRAW);
		}

		// Bind shader
		Data.ShapeShader->use();

		// Call draw command
		if (mode == FILL)
		{
			glDrawArrays(GL_TRIANGLES, 0, triangleCount*3);
		}
		else 
			glDrawArrays(GL_LINE_LOOP, 0, vertexCount);
	}

	void Circle(DrawMode mode, float x, float y, float radius, float red, float green, float blue, unsigned int precision)
	{
		// Create needed array of floats with correct size
		float* vertices = new float[precision * 2];
		
		// Initialize needed variables
		unsigned int count = 0;
		float dx = 0.f;
		float dy = 0.f;
		float angle = 0.f;
		// angle increment calculated through number of degrees per side
		float angleInc = glm::radians(360.f / (float)precision);

		// Loop through all points needed to be created
		for (unsigned int i = 0; i < precision; i++)
		{
			// increase the angle
			angle += angleInc;
			// find the value of x and y
			dx = cos(angle) * radius + x;
			dy = sin(angle) * radius + y;

			// add x and y to the vertex array at the correct place
			vertices[count] = dx;
			count++;
			vertices[count] = dy;
			count++;
		}

		// use polygon to draw
		Polygon(mode, vertices, precision, red, green, blue);

		// clean up new allocated vertex array
		delete[] vertices;
	}

	void Rectangle(DrawMode mode, float x, float y, float width, float height, float rotation, float rotOffsetX, float rotOffsetY, float red, float green, float blue)
	{
		// Assemble vertex positions
		float Vertices[8] = {
			x			, y,			  // top left
			x + width, y,			  // top right
			x + width, y + height, // bottom right
			x			, y + height  // bottom left
		};

		// Rotate vertex positions
		if (rotation != 0)
		{
			// initialize sin, cos values for performace
			float sinX = sin(rotation);
			float cosX = cos(rotation);
			
			for (int i = 0; i < 8; i+=2)
			{
				float origX = Vertices[i];
				float origY = Vertices[i + 1];
				// rotate vertices
				Vertices[i] = cosX * (origX - x - rotOffsetX) - sinX * (origY - y - rotOffsetY) + x + rotOffsetX;
				Vertices[i + 1] = sinX * (origX - x - rotOffsetX) + cosX * (origY - y - rotOffsetY) + y + rotOffsetY;
			}
		}

		// use polygon to draw
		Polygon(mode, Vertices, 4, red, green, blue);
	}

	void BatchLinesPush()
	{
		// reset Batch state
		Data.BatchSize = 0;
		Data.isBatched = true;

		glBindVertexArray(Data.VAO);
		// bind color vbo
		glBindBuffer(GL_ARRAY_BUFFER, Data.ColorVBO);
		glVertexAttribDivisor(1, 1);

		if (Data.BatchCapacity == 0)
		{
			// resize batch capacity and vector size
			Data.BatchCapacity += 10;
			Data.BatchVector.resize(Data.BatchCapacity * 4);
			Data.ColorVector.resize(Data.BatchCapacity * 3);

			// resize vertex buffer
			glBindBuffer(GL_ARRAY_BUFFER, Data.VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * Data.BatchCapacity * 4, NULL, GL_DYNAMIC_DRAW);
			// resize color buffer
			glBindBuffer(GL_ARRAY_BUFFER, Data.ColorVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * Data.BatchCapacity * 3, NULL, GL_DYNAMIC_DRAW);
		}

		// reset vert vector
		Data.BatchVector.clear();
		Data.BatchVector.resize(Data.BatchCapacity * 4);

		// reset color vector
		Data.ColorVector.clear();
		Data.ColorVector.resize(Data.BatchCapacity * 3);
	}

	void Line(float x1, float y1, float x2, float y2, float red, float green, float blue)
	{
		float verData[4] = {
			x1, y1, x2, y2
		};

		float color[3] = {
			red, green, blue
		};

		if (Data.isBatched)
		{
			if (Data.BatchSize >= Data.BatchCapacity)
			{	// resize vector/buffer
				Data.BatchCapacity += 10;
				// vector resize
				Data.BatchVector.resize(Data.BatchCapacity * 4);
				Data.ColorVector.resize(Data.BatchCapacity * 3);

				// vbo resize
				glBindBuffer(GL_ARRAY_BUFFER, Data.VBO);
				glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * 4 * Data.BatchCapacity, NULL, GL_DYNAMIC_DRAW);
				// color vbo resize
				glBindBuffer(GL_ARRAY_BUFFER, Data.ColorVBO);
				glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * 3 * Data.BatchCapacity, NULL, GL_DYNAMIC_DRAW);
			}

			// verify that the buffer size is still valid
			// multiple other Graphics calls use the same vertex and color buffer, resizing it
			// check the actual size of the buffer, if it's too small than expected
			// full resize both buffers
			int bufferSize = 0;
			glBindBuffer(GL_ARRAY_BUFFER, Data.VBO);
			// get buffer size
			glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &bufferSize);
			if (bufferSize < sizeof(GL_FLOAT) * 4 * Data.BatchCapacity)
			{	// resize
				glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * 4 * Data.BatchCapacity, NULL, GL_DYNAMIC_DRAW);

				glBindBuffer(GL_ARRAY_BUFFER, Data.ColorVBO);
				glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * 3 * Data.BatchCapacity, NULL, GL_DYNAMIC_DRAW);
			}

			// add vertex data
			for (int i = 0; i < 4; i++)
			{
				Data.BatchVector[Data.BatchSize * 4 + i] = verData[i];
			}
			// add color data
			for (int i = 0; i < 3; i++)
			{
				Data.ColorVector[Data.BatchSize * 3 + i] = color[i];
			}

			Data.BatchSize++;
		}
		else
		{
			glBindVertexArray(Data.VAO);
			// bind and fill vert data
			glBindBuffer(GL_ARRAY_BUFFER, Data.VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * 4, verData, GL_DYNAMIC_DRAW);

			// bind and fill color data
			glBindBuffer(GL_ARRAY_BUFFER, Data.ColorVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * 3, color, GL_DYNAMIC_DRAW);
			glVertexAttribDivisor(1, 1);

			// bind shader
			Data.ShapeShader->use();

			// issue draw call
			glDrawArrays(GL_LINES, 0, 2);

		}
	}

	void BatchLinesPop()
	{
		if (Data.isBatched)
		{
			// bind vbo and fill data
			glBindBuffer(GL_ARRAY_BUFFER, Data.VBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GL_FLOAT) * 4 * Data.BatchSize, Data.BatchVector.data());

			// bind color vbo and fill data
			glBindBuffer(GL_ARRAY_BUFFER, Data.ColorVBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GL_FLOAT) * 3 * Data.BatchSize, Data.ColorVector.data());
			glVertexAttribDivisor(1, 1);

			// bind shader
			Data.ShapeShader->use();

			// issue draw call for BatchSize points
			glDrawArrays(GL_LINES, 0, Data.BatchSize * 2);
		}
		else
		{
			// Push was not called before this
			assert(false);
		}

		Data.isBatched = false;
	}

	void BatchPointsPush()
	{
		// reset Batch state
		Data.BatchSize = 0;
		Data.isBatched = true;

		glBindVertexArray(Data.VAO);
		// bind color vbo and set it to not use vertex attrib division
		// vertex attrib division doesn't seem to play nice with batched points
		glBindBuffer(GL_ARRAY_BUFFER, Data.ColorVBO);
		glVertexAttribDivisor(1, 0);

		if (Data.BatchCapacity == 0)
		{
			// resize batch capacity and vector size
			Data.BatchCapacity += 10;
			Data.BatchVector.resize(Data.BatchCapacity * 2);
			Data.ColorVector.resize(Data.BatchCapacity * 3);

			// resize vertex buffer
			glBindBuffer(GL_ARRAY_BUFFER, Data.VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * Data.BatchCapacity * 2, NULL, GL_DYNAMIC_DRAW);
			// resize color buffer
			glBindBuffer(GL_ARRAY_BUFFER, Data.ColorVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * 3 * Data.BatchCapacity, NULL, GL_DYNAMIC_DRAW);
		}
		
		// reset vert vector
		Data.BatchVector.clear();
		Data.BatchVector.resize(Data.BatchCapacity * 2);
		// reset color vector
		Data.ColorVector.clear();
		Data.ColorVector.resize(Data.BatchCapacity * 3);
	}

	void Point(float x, float y, unsigned int size, float red, float green, float blue)
	{
		// bind vao
		glBindVertexArray(Data.VAO);

		if (Data.isBatched)
		{
			if (Data.BatchSize >= Data.BatchCapacity)
			{	// resize vector/buffer
				Data.BatchCapacity += 10;
				// vector resize
				Data.BatchVector.resize(Data.BatchCapacity * 2);
				Data.ColorVector.resize(Data.BatchCapacity * 3);

				// vbo resize
				glBindBuffer(GL_ARRAY_BUFFER, Data.VBO);
				glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * 2 * Data.BatchCapacity, NULL, GL_DYNAMIC_DRAW);
				// color vbo resize
				glBindBuffer(GL_ARRAY_BUFFER, Data.ColorVBO);
				glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * 3 * Data.BatchCapacity, NULL, GL_DYNAMIC_DRAW);
			}

			// verify that the buffer size is still valid
			// multiple other Graphics calls use the same vertex and color buffer, resizing it
			// check the actual size of the buffer, if it's too small than expected
			// full resize both buffers
			int size = 0;
			glBindBuffer(GL_ARRAY_BUFFER, Data.VBO);
			// get buffer size
			glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
			if (size < sizeof(GL_FLOAT) * 2 * Data.BatchCapacity)
			{	// resize
				glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * 2 * Data.BatchCapacity, NULL, GL_DYNAMIC_DRAW);
				
				glBindBuffer(GL_ARRAY_BUFFER, Data.ColorVBO);
				glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * 3 * Data.BatchCapacity, NULL, GL_DYNAMIC_DRAW);
			}

			// add vertex data
			Data.BatchVector[Data.BatchSize * 2] = x;
			Data.BatchVector[Data.BatchSize * 2 + 1] = y;
			// add color data
			Data.ColorVector[Data.BatchSize * 3] = red;
			Data.ColorVector[Data.BatchSize * 3 + 1] = green;
			Data.ColorVector[Data.BatchSize * 3 + 2] = blue;

			Data.BatchSize++;
		}
		else
		{
			float point[2] = { x, y };
			// bind and fill vert data
			glBindBuffer(GL_ARRAY_BUFFER, Data.VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * 2, point, GL_DYNAMIC_DRAW);

			float color[3] = { red, green, blue };
			// bind and fill color data
			glBindBuffer(GL_ARRAY_BUFFER, Data.ColorVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * 3, color, GL_DYNAMIC_DRAW);

			// set point size
			glPointSize((float)size);
			// bind shader
			Data.ShapeShader->use();
			// issue draw call
			glDrawArrays(GL_POINTS, 0, 1);
		}
	}

	void BatchPointsPop(unsigned int pointSize)
	{
		// bind vao
		glBindVertexArray(Data.VAO);

		if (Data.isBatched)
		{
			// bind vbo and fill data
			glBindBuffer(GL_ARRAY_BUFFER, Data.VBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GL_FLOAT) * 2 * Data.BatchSize, Data.BatchVector.data());

			// bind color vbo and fill data
			glBindBuffer(GL_ARRAY_BUFFER, Data.ColorVBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GL_FLOAT) * 3 * Data.BatchSize, Data.ColorVector.data());

			// set point size
			glPointSize((float)pointSize);
			// bind shader
			Data.ShapeShader->use();
			// issue draw call for BatchSize points
			glDrawArrays(GL_POINTS, 0, Data.BatchSize);
		}
		else
		{
			// Push was not called before this
			assert(false);
		}

		Data.isBatched = false;
	}

	void PrintInternal(std::string text, float x, float y, float scale, float rotation, int digitTruncate, float red, float green, float blue)
	{
		TruncateDigits(text, digitTruncate);

		float offsetX = Data.FontCellWidth / 2.f * scale;
		float offsetY = Data.FontCellHeight / 2.f * scale;

		for (int i = 0; i < text.size(); i++)
		{
			unsigned char ch = text[i];

			Data.Renderer->Draw(
				Data.FontAtlas, ch - Data.FontStartingChar,
				x + offsetX, y + offsetY,
				Data.FontCellWidth * scale, Data.FontCellHeight * scale, 
				rotation, -offsetX, 0.f, 
				red, green, blue
			);

			offsetX += Data.FontWidths[ch] * scale;
		}
	}

	void Print(short value, float x, float y, float scale, float rotation, float red, float green, float blue)
	{
		PrintInternal(std::to_string(value), x, y, scale, rotation, -1, red, green, blue);
	}

	void Print(bool value, float x, float y, float scale, float rotation, float red, float green, float blue)
	{
		if (value)
			PrintInternal("True", x, y, scale, rotation, -1, red, green, blue);
		else
			PrintInternal("False", x, y, scale, rotation, -1, red, green, blue);
	}

	void Print(int value, float x, float y, float scale, float rotation, float red, float green, float blue)
	{
		PrintInternal(std::to_string(value), x, y, scale, rotation, -1, red, green, blue);
	}

	void Print(long value, float x, float y, float scale, float rotation, float red, float green, float blue)
	{
		PrintInternal(std::to_string(value), x, y, scale, rotation, -1, red, green, blue);
	}

	void Print(float value, float x, float y, float scale, float rotation, int digitTruncate, float red, float green, float blue)
	{
		PrintInternal(std::to_string(value), x, y, scale, rotation, digitTruncate, red, green, blue);
	}

	void Print(double value, float x, float y, float scale, float rotation, int digitTruncate, float red, float green, float blue)
	{
		PrintInternal(std::to_string(value), x, y, scale, rotation, digitTruncate, red, green, blue);
	}

	void Print(const char* text, float x, float y, float scale, float rotation, float red, float green, float blue)
	{
		PrintInternal(std::string(text), x, y, scale, rotation, -1, red, green, blue);
	}

	void Print(std::string text, float x, float y, float scale, float rotation, float red, float green, float blue)
	{
		PrintInternal(text, x, y, scale, rotation, -1, red, green, blue);
	}

	void Draw(TextureAtlas* atlas, unsigned int index, float x, float y, float width, float height, float rotation, float rotationOffsetX, float rotationOffsetY, float red, float green, float blue)
	{
		Data.Renderer->Draw(atlas, index, x, y, width, height, rotation, rotationOffsetX, rotationOffsetY, red, green, blue);
	}

	void Draw(TextureAtlas* atlas, glm::vec4 calculatedQuad, float x, float y, float width, float height, float rotation, float rotationOffsetX, float rotationOffsetY, float red, float green, float blue)
	{
		Data.Renderer->Draw(atlas, calculatedQuad, x, y, width, height, rotation, rotationOffsetX, rotationOffsetY, red, green, blue);
	}

	void Render()
	{
		Data.Renderer->Render();
	}

	void CompileStaticDrawData(StaticRenderContainer& container, TextureAtlas* atlas, glm::vec4 calculatedQuad, float x, float y, float width, float height, float rotation, float rotationOffsetX, float rotationOffsetY, float red, float green, float blue)
	{
		Data.Renderer->CompileStatic(container, atlas, calculatedQuad, x, y, width, height, rotation, rotationOffsetX, rotationOffsetY, red, green, blue);
	}

	void CompileStaticDrawData(StaticRenderContainer& container, TextureAtlas* atlas, unsigned int index, float x, float y, float width, float height, float rotation, float rotationOffsetX, float rotationOffsetY, float red, float green, float blue)
	{
		glm::vec4 calculatedQuad = atlas->GetQuad(index);
		Data.Renderer->CompileStatic(container, atlas, calculatedQuad, x, y, width, height, rotation, rotationOffsetX, rotationOffsetY, red, green, blue);
	}

	void LoadStaticDrawData(StaticRenderContainer& container)
	{
		Data.Renderer->LoadStaticData(container);
	}

	void ClearStaticDrawData()
	{
		Data.Renderer->ClearStaticData();
	}

	std::vector<float> TriangulatePolygon(float *vertices, unsigned int vertexCount)
	{
		std::vector<float> triangleList;
		if (vertexCount < 3)
			return triangleList;

		if (vertexCount == 3)
		{
			triangleList.push_back(vertices[0]); // vertex 0
			triangleList.push_back(vertices[1]);
			triangleList.push_back(vertices[2]); // vertex 1
			triangleList.push_back(vertices[3]);
			triangleList.push_back(vertices[4]); // vertex 2
			triangleList.push_back(vertices[5]);
			return triangleList;
		}

		for (unsigned int i = 1; i <= vertexCount - 2; i++)
		{
			triangleList.push_back(vertices[0]);		// core point
			triangleList.push_back(vertices[1]);
			triangleList.push_back(vertices[i * 2]);		// next point according to the triangle index i
			triangleList.push_back(vertices[i * 2 + 1]);
			triangleList.push_back(vertices[i * 2 + 2]);	// next clockwise point from index i
			triangleList.push_back(vertices[i * 2 + 3]);
		}

		return triangleList;
	}

	int split(std::string inputString, char seperator, std::string output[], int size)
	{
		if (inputString.length() == 0)
		{
			return 0;
		}
		int count = 0;
		char word[100] = {}; // word array, init'd to an arbitrary size of 100
		int wordCount = 0;

		for (int i = 0; i < inputString.length(); i++) // loop through inputString
		{
			if (inputString[i] == seperator)
			{
				if (count + 1 > size)
				{
					return -1; // check if output array is full
				}
				output[count] = std::string(word); // add word to output array
				count++;
				for (int j = 0; j < wordCount; j++)
				{
					word[j] = 0; // reset word
				}
				wordCount = 0; // reset word length counter
			}
			else
			{
				word[wordCount] = inputString[i]; // add char to word
				wordCount++;
			}
		}

		// add the last element
		if (count + 1 <= size) // check if array is not full
		{
			output[count] = std::string(word); // add word to output array
			count++;
		}
		else
		{
			return -1; // if full
		}

		return count;
	}

	void TruncateDigits(std::string& text, int digits)
	{
		if (digits < 0) // if digits is negative, dont do anything
			return;

		// loop through string
		for (unsigned int i = 0; i < text.length(); i++)
		{
			if (text[i] == '.') // find the point
			{
				if (digits == 0) // if input is 0, strip the point
					text.erase(text.begin() + i, text.end());
				else
				{	// if not, strip everything after the (point + digits)
					text.erase(text.begin() + i + 1 + digits, text.end());
				}
			}
		}
	}

	GraphicsData::~GraphicsData()
	{
		delete Renderer;
		delete FontAtlas;
	}

} // end graphics namespace