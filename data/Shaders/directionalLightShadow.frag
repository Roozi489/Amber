#version 420 core

#include "common.h"
#include "lighting.h"
#//include "shadow.h"

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

const int sampleCount = 4;
const float spread = 2000.0;

float shadowValue(in vec3 position_ws, in mat4 cameraVpInv, in mat4 shadowVp)
{
    vec4 shadowCoord = shadowVp * vec4(position_ws, 1.0);
    shadowCoord /= shadowCoord.w;
    shadowCoord.xyz = shadowCoord.xyz * vec3(0.5, 0.5, 0.5) + vec3(0.5, 0.5, 0.5);
    
    float result = 0;

    for (int i = 0; i < sampleCount; ++i)
    {
        vec3 tmp = shadowCoord.xyz;
        //int index = int(4.0 * rand01(floor(position_ws * 1000.0), i)) % 4;
        tmp.xy += poissonDisk16[i] / spread;
        result += texture(shadow, tmp);
    }
    return result / sampleCount;
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
    //vec3 specularColor = texture(specular, texCoord).rgb;
    vec3 normalEncoded = texture(normal, texCoord).xyz;

    vec3 normal = normalize(2.0 * normalEncoded - vec3(1.0));

    float depthValue = texture(depth, texCoord).r;
    vec3 position_ws = calculatePositionFromDepth(texCoord, gl_FragCoord.w, depthValue, cameraVpInv);
    color = calculateDirectionalLight(light, normal) * shadowValue(position_ws, cameraVpInv, shadowVp);
}