#version 420 core

in vec3 texcoords;

uniform samplerCube cubeTexture;

out vec4 color;

void main() 
{
	color = texture(cubeTexture, texcoords);
}