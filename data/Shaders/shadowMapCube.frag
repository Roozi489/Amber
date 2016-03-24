#version 420 core

in vec3 position;

out float fragDepth;

uniform vec3 lightPosition;

void main()
{
    vec3 dist = position - lightPosition;
    fragDepth = length(dist);
}