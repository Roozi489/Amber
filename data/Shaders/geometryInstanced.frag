#version 420 core

#include "common.h"

struct Material
{
    sampler2D diffuseMap;
    sampler2D specularMap;
};

in vec3 position;
in vec2 texCoord;
in vec3 normal;
in vec4 orientation;
flat in vec4 color;
flat in int instanceID;

layout(location = 0) out vec4 diffuseColor;
layout(location = 1) out vec4 specularColor;
layout(location = 2) out vec4 normalOut;

//uniform vec4 orientation;
//uniform vec3 cameraPosition;
uniform Material material;

void main()
{
    vec3 normal = normalize(quaternionRotate(orientation, normal));
    
    diffuseColor = texture(material.diffuseMap, texCoord) * color;
    //specularColor = texture(material.specularMap, texCoord);
    normalOut = vec4(0.5 * (normal + vec3(1.0)), 1.0);
}