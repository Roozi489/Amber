#version 330
 
in vec2 texcoord;

out vec4 color;

uniform sampler2D tex;
uniform vec4 fontColor;
 
void main(void)
{
    color = vec4(1, 1, 1, texture(tex, texcoord).r) * fontColor;
}