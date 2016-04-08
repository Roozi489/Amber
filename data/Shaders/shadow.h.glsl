#ifndef SHADOW_H
#define SHADOW_H

vec2 poissonDisk16[16] = vec2[]
(
   vec2(-0.94201624, -0.39906216), 
   vec2(0.94558609, -0.76890725), 
   vec2(-0.094184101, -0.92938870), 
   vec2(0.34495938, 0.29387760), 
   vec2(-0.91588581, 0.45771432), 
   vec2(-0.81544232, -0.87912464), 
   vec2(-0.38277543, 0.27676845), 
   vec2(0.97484398, 0.75648379), 
   vec2(0.44323325, -0.97511554), 
   vec2(0.53742981, -0.47373420), 
   vec2(-0.26496911, -0.41893023), 
   vec2(0.79197514, 0.19090188), 
   vec2(-0.24188840, 0.99706507), 
   vec2(-0.81409955, 0.91437590), 
   vec2(0.19984126, 0.78641367), 
   vec2(0.14383161, -0.14100790) 
);

const int sampleCount = 16;
// TODO: change this depending on shadowMap size or maybe distance from light
const float spread = 600.0;

float shadowValue(in sampler2DShadow shadowMap, in vec3 position_ws, in mat4 shadowVp)
{
    vec4 shadowCoord = shadowVp * vec4(position_ws, 1.0);
    shadowCoord /= shadowCoord.w;
    shadowCoord.xyz = shadowCoord.xyz * vec3(0.5, 0.5, 0.5) + vec3(0.5, 0.5, 0.5);
    
    float sum = 0;
    for (int i = 0; i < sampleCount; ++i)
    {
        vec3 tmp = shadowCoord.xyz;
        //int index = int(4.0 * rand01(floor(position_ws * 1000.0), i)) % 4;
        tmp.xy += poissonDisk16[i] / spread;
        sum += texture(shadowMap, tmp);
    }
    return sum / sampleCount;
}

vec3 sampleOffsetDirections[20] = vec3[]
(
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
); 

float shadowCubeValue(in samplerCubeShadow shadowMap, in vec3 lightToFrag, in float distanceToLight, in float lightRange, in float cameraFragDist)
{
    float bias = 0.1 * cameraFragDist / 20;
    int samples = 20;
    float diskRadius = (1.0 + (distanceToLight / lightRange)) / 350.0;
    float shadow = 0.0;
    for(int i = 0; i < samples; ++i)
    {
        shadow += texture(shadowMap, vec4(lightToFrag + sampleOffsetDirections[i] * diskRadius, distanceToLight - bias));
    }
    shadow /= float(samples);
    return shadow;
}

#endif