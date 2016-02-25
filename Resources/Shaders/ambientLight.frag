#version 330 core

#include "Lighting.h"

uniform BaseLight light;

out vec4 color;

void main()
{
    color = vec4(light.coloredIntensity, 1.0);
}