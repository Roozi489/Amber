#version 420 core

#include "common.h"
#include "lighting.h"
#include "shadow.h"

uniform PointLight light;

//uniform sampler2D specular;
uniform sampler2D normal;
uniform sampler2D depth;
uniform samplerCubeShadow shadow;

uniform mat4 cameraVpInv;

in vec2 texCoord;

out vec4 color;

vec4 calculatePointLight(in vec3 surfaceToLight, in float distanceToLight, in vec3 normal)
{
    float diffuseCoefficient = max(0.0, dot(normal, surfaceToLight));

    float falloff = square(saturate(1.0 - square(square(distanceToLight / light.attenuation.range)))) / 
                    (light.attenuation.constant + light.attenuation.linear * distanceToLight + light.attenuation.quadratic * square(distanceToLight));

    vec4 diffuse;
    diffuse.rgb = diffuseCoefficient * light.base.coloredIntensity * falloff;
    diffuse.a = 1.0;

    return diffuse;
}

void main()
{
    // TODO: add specular
    //vec3 specularColor = texture(specular, texCoord).rgb;
    vec3 normalEncoded = texture(normal, texCoord).xyz;
    float depthValue = texture(depth, texCoord).x;

    vec3 position_ws = calculatePositionFromDepth(texCoord, gl_FragCoord.w, depthValue, cameraVpInv);
    vec3 normal = normalize(2.0 * normalEncoded - vec3(1.0));

    vec3 surfaceToLight = normalize(light.position - position_ws);
    float distanceToLight = length(light.position - position_ws);

    float cosTheta = saturate(dot(normal, surfaceToLight));
    color = calculatePointLight(surfaceToLight, distanceToLight, normal) * shadowCubeValue(shadow, -surfaceToLight, distanceToLight, cosTheta);
}