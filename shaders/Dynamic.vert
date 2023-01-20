#version 330 core

layout (location = 0) in vec4 vertex;

out vec2 texCoord;

layout (std140) uniform Matrices
{
	mat4 vp;
};

void main()
{
	gl_Position = vp * vec4(vertex.xy, 0.0, 1.0);
	texCoord = vertex.zw; // texture uv pre-calculated
}