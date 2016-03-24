#version 420 core

#include "common.h"
#include "lighting.h"
#include "shadow.h"

uniform DirectionalLight light;

// TODO: add specular
//uniform sampler2D specular;
uniform sampler2D normal;
uniform sampler2D depth;
uniform sampler2DShadow shadow;

uniform mat4 cameraVpInv;
uniform mat4 shadowVp;

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
    //vec3 specularColor = texture(specular, texCoord).rgb;
    vec3 normalEncoded = texture(normal, texCoord).xyz;

    vec3 normal = normalize(2.0 * normalEncoded - vec3(1.0));

    float depthValue = texture(depth, texCoord).r;
    vec3 position_ws = calculatePositionFromDepth(texCoord, gl_FragCoord.w, depthValue, cameraVpInv);
    color = calculateDirectionalLight(light, normal) * shadowValue(shadow, position_ws, shadowVp);
}