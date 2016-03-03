#version 330 core

uniform sampler2D diffuse;
uniform sampler2D lighting;

in vec2 texCoord;

out vec4 color;

void main()
{
    vec4 diffuseColor = texture(diffuse, texCoord);
    vec4 lightingColor = texture(lighting, texCoord);

    vec4 finalColor = diffuseColor * lightingColor;
    vec3 gamma = vec3(1.0 / 2.2);

    color = vec4(pow(finalColor.rgb, gamma), 1.0);
}