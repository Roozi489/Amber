#version 420 core

#include "common.h"

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec2 in_texCoord;
layout(location = 2) in vec3 in_normal;
// Instanced attributes
layout(location = 3) in vec4 in_orientation;
layout(location = 4) in mat4 in_mvp;
// location 4-5-6-7 taken by mvp
layout(location = 8) in vec4 in_color;

out vec3 position;
out vec2 texCoord;
out vec3 normal;
out vec4 orientation;
flat out vec4 color;
flat out int instanceID; 

//uniform mat4 mvp;
//uniform Transform transform;

void main()
{
    position = in_position;
    texCoord = in_texCoord;
    normal = in_normal;
    orientation = in_orientation;
    instanceID = gl_InstanceID; 
    //mvp = in_mvp;
    color = in_color;

    gl_Position = in_mvp * vec4(position, 1.0);
}