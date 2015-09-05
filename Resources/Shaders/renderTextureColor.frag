#version 330 core
 
in vec2 UV;
 
out vec4 color;
 
uniform sampler2D tex;
 
void main()
{
    color = vec4(texture(tex, UV).xyz, 1.0);
}