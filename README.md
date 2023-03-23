# GraphicsFramework
A homemade, c++, 2D graphics renderer powered by OpenGL. 

# Features
* Texture rendering, with multi-texture support and automatic batching
* Basic shape/primitive rendering: Circles, Polygons, Lines, Points
  * Allows outlined and filled in shapes
* BMP text rendering

# Dependencies
[GLFW](https://github.com/glfw/glfw)
* Used for window, rendering context, and input management
* Used as a static library

[GLEW](https://github.com/nigels-com/glew)
* Used for OpenGL function bindings and everything OpenGL related
* Needs to link against OpenGL32.lib
* Used as a static library

[STB_Image](https://github.com/nothings/stb/blob/master/stb_image.h)
* Used for loading and unloading image files into usable data
* Single header only

[GLM](https://github.com/g-truc/glm)
* Used for math and matrix manipulation
* Header only library
