#version 420 core

layout(location = 0) in vec3 in_position;

out vec3 position_ws;

uniform mat4 mvp;
uniform mat4 model;

void main()
{
	gl_Position = vec4(in_position, 1.0);
    position_ws = (model * gl_Position).xyz;
	//vec4 pos4 = vec4(in_position, 1.0);
    //gl_Position = mvp * pos4;
    //position_ws = (model * pos4).xyz;
}