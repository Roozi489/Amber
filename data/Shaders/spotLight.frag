#version 330 core

#include "common.h"
#include "lighting.h"

uniform sampler2D specular;
uniform sampler2D normal;
uniform sampler2D depth;

uniform SpotLight light;
uniform mat4 vpInverse;

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
    vec3 specularColor = texture(specular, texCoord).rgb;
    vec3 normalEncoded = texture(normal, texCoord).xyz;
    float depthValue = texture(depth, texCoord).x;

    vec3 position = calculatePositionFromDepth(texCoord, gl_FragCoord.w, depthValue, vpInverse);
    vec3 normal = normalize(2.0 * normalEncoded - vec3(1.0));

    vec3 surfaceToLight = normalize(light.base.position - position);
    float distanceToLight = length(light.base.position - position);
    float lightToSurfaceAngle = acos(dot(-surfaceToLight, normalize(light.direction)));

    color = calculateSpotLight(lightToSurfaceAngle, surfaceToLight, distanceToLight, normal);
}