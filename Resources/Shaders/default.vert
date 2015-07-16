#version 330 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormal;

out vec3 position_Modelspace;
out vec2 uv;
out vec3 normal_Modelspace;

out vec3 cameraDirection_Worldspace;
out vec3 light1Position_Worldspace;
out vec3 light2Position_Worldspace;
out vec3 light3Position_Worldspace;
out vec3 light4Position_Worldspace;

out vec3 reflection;

out vec4 matColor;

uniform vec3 cameraPosition;
uniform vec3 cameraDirection;

uniform mat4 MVP;
uniform mat4 V;
uniform mat4 M;

uniform vec4 color;

float PI = 3.14159265358979323846264;

void main()
{
    cameraDirection_Worldspace = cameraDirection;

    position_Modelspace = vertexPosition;
    uv = vertexUV;
    normal_Modelspace = vertexNormal;

    matColor = color;

    light1Position_Worldspace = vec3(0.0, 30.0, -30.0);
    light2Position_Worldspace = vec3(30.0, 30.0, 0.0);
    light3Position_Worldspace = vec3(0.0, 30.0, 30.0);
    light4Position_Worldspace = vec3(-30.0, 30.0, 0.0);
    
    reflection = reflect(-normalize(cameraPosition - vertexPosition), normalize(vertexNormal));
    
    gl_Position = MVP * vec4(vertexPosition, 1.0);
}