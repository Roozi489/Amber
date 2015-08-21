#version 330 core
 
in vec2 UV;
 
out vec3 color;
 
uniform sampler2D tex;
 
void main()
{
	float c = pow(texture(tex, UV).r , 256);
    color = vec3(c, c, c);
}