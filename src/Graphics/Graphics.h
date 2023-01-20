#ifndef SIMPLE_GRAPHICS_H
#define SIMPLE_GRAPHICS_H

#include "TextureRenderer.h"
#include <vector>

// ! Graphics Rendering Framework
// Allows for Shape rendering: Polygons, Circles, Lines, Points
// Allows for Image rendering: Text, Images
namespace Graphics {

	// Enum for draw modes - used to alter drawing behavior
	enum DrawMode
	{
		// Shape should be drawn solid and filled in
		// Calls Triangulate on said shape, could be less performant on large shapes
		FILL,
		// Shape should be drawn as an outline only
		// Line allows the shape to be concave
		LINE
	};

	// Wrapper struct for the data used to render
	struct GraphicsData
	{
		//! Cleanup function to clear heap objects
		~GraphicsData();

		//! Constructed Renderer object - used to draw text and images
		TextureRenderer* Renderer = nullptr;

		//! Copied Shader objects - used in rendering
		Shader* ShapeShader = nullptr;		// Shader for shape rendering
		Shader* RenderShader = nullptr;		// Shader for image/text rendering

		//! OpenGL object IDs - used in shape rendering
		unsigned int VAO = 0;					// VAO for all shape rendering opperations
		unsigned int VBO = 0;					// Buffer for vertex data
		unsigned int ColorVBO = 0;				// Buffer for color data

		//! Data vectors for batched drawing - used only for lines and points
		std::vector<float> BatchVector;
		std::vector<float> ColorVector;

		//! Batch state data - used only for lines and points
		bool isBatched = false;					// Flag whether to batch or not
		unsigned int BatchCapacity = 0;		// Size limit of the batch vectors - in terms of a single batch, not singular vertices/values
		unsigned int BatchSize = 0;			// Current size count in the vectors - used for correctly indexing the vectors
		
		//! Font data - used in text loading and rendering
		TextureAtlas* FontAtlas = nullptr;	// Constructed atlas that holds the image texture
		int FontWidths[256] = {};				// Array of all the individual letter widths
		int FontStartingChar = -1;				// Starting character of the image
		int FontCellWidth = -1;					// Texture-specific quad width
		int FontCellHeight = -1;				// Texture-specific quad height
	};

	// Initialize Graphics rendering data
	// Constructs a TextureRenderer object given the renderShader
	void Init(Shader* polygonShader, Shader* renderShader);

	void LoadFont(std::string fontImagePath, std::string fontDataPath);

	TextureRenderer* GetRenderer();

	// ! Basic shape functions

	// Draw a polygon from an array of vertices and a given vertex count and color
	void Polygon(DrawMode mode, float *vertices, unsigned int vertexCount, float red = 1.f, float green = 1.f, float blue = 1.f);

	// Draw a circle of a given radius and color
	// Precision value states how many sides the circle should have
	// * more sides leads to less performance, especially in FILL mode
	void Circle(DrawMode mode, float x, float y, float radius, float red = 1.f, float green = 1.f, float blue = 1.f, unsigned int precision = 16);

	// Draw a rectangle of top left origin given dimentions and color
	// Rotation offset changes the origin of rotation
	// * Rotation in radians
	void Rectangle(DrawMode mode, float x, float y, float width, float height, float rotation = 0.f, float rotOffsetX = 0.f, float rotOffsetY = 0.f, float red = 1.f, float green = 1.f, float blue = 1.f);

	// ! Lines - able to be batched together

	// Enable line batching.
	void BatchLinesPush();

	// Draw a line from (x1, y1) to (x2, y2).
	void Line(float x1, float y1, float x2, float y2, float red = 1.f, float green = 1.f, float blue = 1.f);

	// Disable line batching and issue the draw call for all Graphics::Line() calls after the last Graphics::BatchLinesPush().
	void BatchLinesPop();

	// ! Points - able to be batched together

	// Enable point batching.
	void BatchPointsPush();

	// Draw a point given a size and color
	// * size is basically the radius in pixels, but square
	// * size is unused if batch, set size in the BatchPointsPop() call
	void Point(float x, float y, unsigned int size = 1, float red = 1.f, float green = 1.f, float blue = 1.f);

	// Disable point batching and issue the draw call for all Graphics::Point() calls after the last Graphics::BatchPointsPush().
	// "pointSize" is the pixel size in radius.
	void BatchPointsPop(unsigned int pointSize = 1);

	// ! Text printing

	// Internal initial overloaded print function. 
	void PrintInternal(std::string text, float x, float y, float scale = 1.f, float rotation = 0.f, int digitTruncate = -1, float red = 1.f, float green = 1.f, float blue = 1.f);
	
	// ! Print function overloads

	// Print to screen, starting from (x, y)
	// rotation in rotation in radians
	void Print(short value, float x, float y, float scale = 1.f, float rotation = 0.f, float red = 1.f, float green = 1.f, float blue = 1.f);
	// Print to screen, starting from (x, y)
	// rotation in rotation in radians
	void Print(int value, float x, float y, float scale = 1.f, float rotation = 0.f, float red = 1.f, float green = 1.f, float blue = 1.f);
	// Print to screen, starting from (x, y)
	// rotation in rotation in radians
	void Print(long value, float x, float y, float scale = 1.f, float rotation = 0.f, float red = 1.f, float green = 1.f, float blue = 1.f);
	// Print to screen, starting from (x, y)
	// rotation in rotation in radians
	// digitTruncate controls how many digits after the decimal are drawn, set -1 for no truncation
	void Print(float value, float x, float y, float scale = 1.f, float rotation = 0.f, int digitTruncate = -1, float red = 1.f, float green = 1.f, float blue = 1.f);
	// Print to screen, starting from (x, y)
	// rotation in rotation in radians
	// digitTruncate controls how many digits after the decimal are drawn, set -1 for no truncation
	void Print(double value, float x, float y, float scale = 1.f, float rotation = 0.f, int digitTruncate = -1, float red = 1.f, float green = 1.f, float blue = 1.f);
	// Print to screen, starting from (x, y)
	// rotation in rotation in radians
	void Print(const char* text, float x, float y, float scale = 1.f, float rotation = 0.f, float red = 1.f, float green = 1.f, float blue = 1.f);
	// Print to screen, starting from (x, y)
	// rotation in rotation in radians
	void Print(std::string text, float x, float y, float scale = 1.f, float rotation = 0.f, float red = 1.f, float green = 1.f, float blue = 1.f);

	// ! Image rendering

	// Image Draw call
	// Gives the Renderer an image at the location and dimentions
	// * the color input allows for color altering the image, the given values are multiplied to the sampled texture color
	void Draw(TextureAtlas* atlas, unsigned int index, float x, float y, float width, float height, float rotation = 0.f, float rotationOffsetX = 0.f, float rotationOffsetY = 0.f, float red = 1.f, float green = 1.f, float blue = 1.f);
	void Draw(TextureAtlas* atlas, glm::vec4 calculatedQuad, float x, float y, float width, float height, float rotation = 0.f, float rotationOffsetX = 0.f, float rotationOffsetY = 0.f, float red = 1.f, float green = 1.f, float blue = 1.f);

	// Renderer final Draw call
	// Renders all image 'Draw()' calls since the last Render() call
	void Render();

	// Compile a static image to be drawn, loads the quad into a StaticRenderContainer
	void CompileStaticDrawData(StaticRenderContainer& container, TextureAtlas* atlas, glm::vec4 calculatedQuad, float x, float y, float width, float height, float rotation = 0.f, float rotationOffsetX = 0.f, float rotationOffsetY = 0.f, float red = 1.f, float green = 1.f, float blue = 1.f);
	void CompileStaticDrawData(StaticRenderContainer& container, TextureAtlas* atlas, unsigned int index, float x, float y, float width, float height, float rotation = 0.f, float rotationOffsetX = 0.f, float rotationOffsetY = 0.f, float red = 1.f, float green = 1.f, float blue = 1.f);

	// Load data from a StaticRenderContainer to be drawn staticly
	void LoadStaticDrawData(StaticRenderContainer& container);

	// Clear any static data stored in the TextureRenderer
	void ClearStaticDrawData();

	// ! Utility functions

	// Triangulate a SIMPLE, CONVEX polygon
	// * undefined behavior for complex/concave polygons
	// Returns a vector of calculated triangles (6 floats per triangle, 2 floats per coord)
	std::vector<float> TriangulatePolygon(float* vertices, unsigned int vertexCount);

	// Split up a given string based on a seperator char
	// Outputs into an array
	int split(std::string inputString, char seperator, std::string output[], int size);

	// Given a float in string form, truncate x amount of digits from the decimal
	// Pass -1 to not truncate at all
	void TruncateDigits(std::string& text, int digits);

}	// namespace Graphics

#endif