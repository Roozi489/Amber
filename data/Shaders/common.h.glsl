#ifndef COMMON_H
#define COMMON_H

struct Transform
{
    vec3 position;
    vec4 orientation;
    vec3 scale;
};

float square(float x)
{
    return x * x;
}

float saturate(float x)
{
    return clamp(x, 0.0, 1.0);
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