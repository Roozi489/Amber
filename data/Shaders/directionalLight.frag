#version 330 core

#include "common.h"
#include "lighting.h"

uniform DirectionalLight light;

// TODO: add specular
uniform sampler2D specular;
uniform sampler2D normal;
uniform sampler2D depth;
uniform sampler2D shadow;

uniform mat4 cameraVpInv;
uniform mat4 shadowVp;

in vec2 texCoord;

out vec4 color;

float readShadowMap()
{
    float depthValue = texture(depth, texCoord).x;
    vec3 position_ws = calculatePositionFromDepth(texCoord, gl_FragCoord.w, depthValue, cameraVpInv);

    vec4 shadowCoord = shadowVp * vec4(position_ws, 1.0);
    shadowCoord /= shadowCoord.w;

    shadowCoord.xyz = shadowCoord.xyz * vec3(0.5f, 0.5f, 0.5f) + vec3(0.5f, 0.5f, 0.5f);

    const float bias = 0.0005;
    if (texture(shadow, shadowCoord.xy).x < shadowCoord.z - bias)
        return 0.1;
    return 1.0;
}

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

    //readShadowMap();
    color = calculateDirectionalLight(light, normal) * readShadowMap();
}