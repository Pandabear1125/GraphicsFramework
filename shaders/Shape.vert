#version 330 core

layout(location = 0) in vec2 V_Position;
layout(location = 1) in vec3 V_Color;

layout (std140) uniform Matrices
{
	mat4 VP;
};

out vec3 color;

void main()
{
	gl_Position = VP * vec4(V_Position, 0, 1);
	color = V_Color;
}