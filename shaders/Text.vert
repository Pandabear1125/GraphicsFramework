#version 330 core
layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>
layout (location = 1) in vec3 color;

out vec2 TexCoord;
out vec3 TextColor;

layout (std140) uniform Matrices
{
	mat4 VP;
};

void main()
{
	gl_Position = VP * vec4(vertex.xy, 0.0, 1.0);
	TexCoord = vertex.zw;
	TextColor = color;
}