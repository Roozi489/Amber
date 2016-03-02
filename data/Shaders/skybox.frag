#version 400

in vec3 texcoords;

uniform samplerCube cubeTexture;

out vec4 color;

void main() 
{
	color = texture(cubeTexture, texcoords);
}