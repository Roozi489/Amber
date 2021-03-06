#version 420 core

#include "common.h"
#include "lighting.h"
#include "shadow.h"

uniform SpotLight light;

//uniform sampler2D specular;
uniform sampler2D normal;
uniform sampler2D depth;
uniform sampler2DShadow shadow;

uniform mat4 cameraVpInv;
uniform mat4 shadowVp;

in vec2 texCoord;

out vec4 color;

vec4 calculateSpotLight(in float lightToSurfaceAngle, in vec3 surfaceToLight, in float distanceToLight, in vec3 normal)
{
    float diffuseCoefficient = max(0.0, dot(normal, surfaceToLight));

    float falloff = square(saturate(1.0 - square(square(distanceToLight / light.base.attenuation.range)))) / 
                    (light.base.attenuation.constant + light.base.attenuation.linear * distanceToLight +
                     light.base.attenuation.quadratic * square(distanceToLight));

    falloff *= square(saturate(1.0 - pow((lightToSurfaceAngle / light.coneAngle), 10.0)));
    
    vec4 diffuse;
    diffuse.rgb = diffuseCoefficient * light.base.base.coloredIntensity * falloff;
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

    vec3 surfaceToLight = normalize(light.base.position - position_ws);
    float distanceToLight = length(light.base.position - position_ws);
    float lightToSurfaceAngle = acos(dot(-surfaceToLight, normalize(light.direction)));

    color = calculateSpotLight(lightToSurfaceAngle, surfaceToLight, distanceToLight, normal) * shadowValue(shadow, position_ws, shadowVp);
}