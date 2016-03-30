#version 420 core

#include "common.h"

struct Material
{
    bool hasTexture;
    vec4 color;

    sampler2D diffuseMap;
    sampler2D specularMap;
};

vec3 quaternionRotate(vec4 q, vec3 v)
{
    vec3 t = 2.0 * cross(q.xyz, v);
    return (v + q.w * t + cross(q.xyz, t));
}

//in vec3 position_WS;
in vec3 position;
in vec2 texCoord;
in vec3 normal;

layout(location = 0) out vec4 diffuseColor;
layout(location = 1) out vec4 specularColor;
layout(location = 2) out vec4 normalOut;

uniform vec4 orientation;
uniform Material material;

//uniform vec3 cameraPosition;


void main()
{
    vec3 normal = normalize(quaternionRotate(orientation, normal));

    if (material.hasTexture)
        diffuseColor = texture(material.diffuseMap, texCoord);
    else
        diffuseColor = material.color;
    specularColor = texture(material.specularMap, texCoord);
    normalOut = vec4(0.5 * (normal + vec3(1.0)), 1.0);
}