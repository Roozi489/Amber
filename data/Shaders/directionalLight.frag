#version 420 core

#include "common.h"
#include "lighting.h"

uniform DirectionalLight light;

// TODO: add specular
uniform sampler2D specular;
uniform sampler2D normal;

in vec2 texCoord;

out vec4 color;

vec4 calculateDirectionalLight(in DirectionalLight light, in vec3 normal)
{
    float diffuseCoefficient = max(0.0, dot(normal, -light.direction));

    vec4 diffuse;
    diffuse.rgb = diffuseCoefficient * light.base.coloredIntensity;
    diffuse.a = 1.0;

    return diffuse;
}

void main()
{
    vec3 specularColor = texture(specular, texCoord).rgb;
    vec3 normalEncoded = texture(normal, texCoord).xyz;

    vec3 normal = normalize(2.0 * normalEncoded - vec3(1.0));

    color = calculateDirectionalLight(light, normal);
}