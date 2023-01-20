#version 330 core

in vec2 TexCoord;
in vec3 TextColor;

out vec4 color;

uniform sampler2D text;

void main()
{
	color = texture(text, TexCoord) * vec4(TextColor, 1.0);
}