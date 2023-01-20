#version 330 core

layout (location = 0) in vec4 v_ScreenPosition;
layout (location = 1) in vec3 v_ColorOffset;
layout (location = 2) in float v_TextureIndex;

out vec2 f_TextureCoord;
out vec3 f_ColorOffset;
out flat int f_TextureIndex;

layout (std140) uniform Matrices
{
	mat4 vp;
};

void main()
{
	gl_Position = vp * vec4(v_ScreenPosition.xy, 0, 1);
	f_TextureCoord = v_ScreenPosition.zw;
	f_TextureIndex = int(v_TextureIndex);
	f_ColorOffset = v_ColorOffset;
}