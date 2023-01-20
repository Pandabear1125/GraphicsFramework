#version 330 core

in vec2 f_TextureCoord;
in vec3 f_ColorOffset;
in flat int f_TextureIndex;

out vec4 r_FragColor;

uniform sampler2D u_Textures[16];

void main()
{
	int index = f_TextureIndex;

	if (index == 0)
		r_FragColor = texture(u_Textures[0], f_TextureCoord);
	else if (index == 1)
		r_FragColor = texture(u_Textures[1], f_TextureCoord);
	else if (index == 2)
		r_FragColor = texture(u_Textures[2], f_TextureCoord);
	else if (index == 3)
		r_FragColor = texture(u_Textures[3], f_TextureCoord);
	else if (index == 4)
		r_FragColor = texture(u_Textures[4], f_TextureCoord);
	else if (index == 5)
		r_FragColor = texture(u_Textures[5], f_TextureCoord);
	else if (index == 6)
		r_FragColor = texture(u_Textures[6], f_TextureCoord);
	else if (index == 7)
		r_FragColor = texture(u_Textures[7], f_TextureCoord);
	else if (index == 8)
		r_FragColor = texture(u_Textures[8], f_TextureCoord);
	else if (index == 9)
		r_FragColor = texture(u_Textures[9], f_TextureCoord);
	else if (index == 10)
		r_FragColor = texture(u_Textures[10], f_TextureCoord);
	else if (index == 11)
		r_FragColor = texture(u_Textures[11], f_TextureCoord);
	else if (index == 12)
		r_FragColor = texture(u_Textures[12], f_TextureCoord);
	else if (index == 13)
		r_FragColor = texture(u_Textures[13], f_TextureCoord);
	else if (index == 14)
		r_FragColor = texture(u_Textures[14], f_TextureCoord);
	else if (index == 15)
		r_FragColor = texture(u_Textures[15], f_TextureCoord);
	else 
		r_FragColor = vec4(1, 0, 1, 1);

	r_FragColor = r_FragColor * vec4(f_ColorOffset, 1);
}