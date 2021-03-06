#version 330 core

layout(location = 0) in vec3 vertexPosition;

out vec2 texCoord;

void main()
{
    texCoord = (vertexPosition.xy + vec2(1, 1)) / 2.0;
    gl_Position = vec4(vertexPosition, 1);
}