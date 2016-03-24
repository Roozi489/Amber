#version 420 core

layout(location = 0) in vec3 in_position;

out vec3 position;

uniform mat4 mvp;
uniform mat4 model;

void main()
{
	vec4 pos4 = vec4(in_position, 1.0);
    gl_Position = mvp * pos4;
    position = (model * pos4).xyz;
}