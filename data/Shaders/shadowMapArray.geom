#version 420 core

uniform mat4 transformations[6];
 
layout(triangles) in;
layout(triangle_strip, max_vertices = 18) out;

in vec3 position_ws[3];
out vec3 position;

void main(void)
{
    for(int layer = 0; layer < 6; ++layer)
    {
        gl_Layer = layer;
        for(int i= 0; i < 3; ++i)
        {
            gl_Position = transformations[layer] * gl_in[i].gl_Position;
            position = position_ws[i];//gl_in[i].gl_Position.xyz;
            EmitVertex();
        }
        EndPrimitive();
    }
}