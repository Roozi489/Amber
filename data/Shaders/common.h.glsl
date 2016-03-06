#ifndef COMMON_H
#define COMMON_H

struct Transform
{
    vec3 position;
    vec4 orientation;
    vec3 scale;
};

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

float square(float x)
{
    return x * x;
}

float saturate(float x)
{
    return clamp(x, 0.0, 1.0);
}

float rand01(vec4 seed4)
{
    float dotProduct = dot(seed4, vec4(12.9898, 78.233, 45.164, 94.673));
    return fract(sin(dotProduct) * 43758.5453);
}

float rand01(vec3 seed, int i)
{
    vec4 seed4 = vec4(seed,i);
    return rand01(seed4);
}

vec3 calculatePositionFromDepth(in vec2 st, in float w, in float depthValue, in mat4 inverseCameraMatrix)
{
    float x_ss = 2.0 * st.x - 1.0;
    float y_ss = 2.0 * st.y - 1.0;
    float z_ss = 2.0 * depthValue - 1.0;

    vec4 position_ss = vec4(x_ss, y_ss, z_ss, 1.0) / w;

    vec4 position_ws = inverseCameraMatrix * position_ss;

    return position_ws.xyz / position_ws.w;
}

#endif