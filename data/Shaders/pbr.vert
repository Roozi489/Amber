#version 420 core

layout(location = 0) in vec3 vertexPosition_Modelspace;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormal_Modelspace;

out vec3 position_Worldspace;
out vec2 uv;
out vec3 normal_Worldspace;

out vec3 position_Modelspace;
out vec3 normal_Modelspace;

uniform mat4 MVP;
uniform mat4 V;
uniform mat4 M;

void main()
{
    uv = vertexUV;
	vec3 vertexPosition_Cameraspace = (V * vec4(vertexPosition_Modelspace, 1.0)).xyz;
	//eyeDirection_Cameraspace = vec3(0.0, 0.0, 0.0) - vertexPosition_Cameraspace;

	position_Modelspace = vertexPosition_Modelspace;
	normal_Modelspace = vertexNormal_Modelspace;

	position_Worldspace = (M * vec4(vertexPosition_Modelspace, 1.0)).xyz;
	normal_Worldspace = (M * vec4(vertexNormal_Modelspace, 1.0)).xyz;
    
	gl_Position = MVP * vec4(vertexPosition_Modelspace, 1.0);
}

