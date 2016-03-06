#ifndef SHADOW_H
#define SHADOW_H

const int sampleCount = 4;
const float spread = 700.0;
const vec2 poissonDisk[4] = vec2[]
(
  vec2(-0.94201624, -0.39906216),
  vec2(0.94558609, -0.76890725),
  vec2(-0.094184101, -0.92938870),
  vec2(0.34495938, 0.29387760)
);

float shadowValue(vec2 texCoord, mat4 cameraVpInv, mat4 shadowVp)
{
    float depthValue = texture(depth, texCoord).r;
    vec3 position_ws = calculatePositionFromDepth(texCoord, gl_FragCoord.w, depthValue, cameraVpInv);

    vec4 shadowCoord = shadowVp * vec4(position_ws, 1.0);
    shadowCoord /= shadowCoord.w;
    shadowCoord.xyz = shadowCoord.xyz * vec3(0.5f, 0.5f, 0.5f) + vec3(0.5f, 0.5f, 0.5f);
    
    float result = 0;

    for (int i = 0; i < sampleCount; ++i)
    {
        vec3 tmp = shadowCoord.xyz;
        tmp.xy += poissonDisk[i] / spread;
        result += texture(shadow, tmp) / sampleCount;
    }
    return result;
}

#endif