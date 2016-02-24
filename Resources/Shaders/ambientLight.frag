#version 330 core

struct BaseLight
{
    vec3 coloredIntensity; // color * intensity
};

uniform BaseLight light;

out vec4 color;

void main()
{
    color = vec4(light.coloredIntensity, 1.0);
}