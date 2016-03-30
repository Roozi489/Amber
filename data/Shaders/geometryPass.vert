#version 420 core

#include "common.h"

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec2 in_texCoord;
layout(location = 2) in vec3 in_normal;

//out vec3 position_WS;
out vec3 position;
out vec2 texCoord;
out vec3 normal;

uniform mat4 mvp;

//uniform Transform transform;

vec3 quaternionRotate(vec4 q, vec3 v)
{
    vec3 t = 2.0 * cross(q.xyz, v);
    return (v + q.w * t + cross(q.xyz, t));
}

void main()
{
    position = in_position;
    texCoord = in_texCoord;
    normal = in_normal;

    //position_WS = transform.position + quaternionRotate(transform.orientation, transform.scale * position);

    gl_Position = mvp * vec4(position, 1.0);
}