#version 330 core

#include "lighting.h"

uniform BaseLight light;

out vec4 color;

void main()
{
    color = vec4(light.coloredIntensity, 1.0);
}