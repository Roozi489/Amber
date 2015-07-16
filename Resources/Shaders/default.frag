#version 330 core

in vec3 position_Worldspace;
in vec2 uv;
in vec3 normal_Worldspace;

in vec3 position_Modelspace;
in vec3 normal_Modelspace;
in vec3 reflection;

in vec3 cameraDirection_Worldspace;
in vec3 light1Position_Worldspace;
in vec3 light2Position_Worldspace;
in vec3 light3Position_Worldspace;
in vec3 light4Position_Worldspace;

in vec4 matColor;

out vec4 color;

uniform int useTexture;
uniform sampler2D texSampler;

uniform mat4 M;
uniform mat4 V;
uniform float roughness = 0.0;

float PI = 3.14159265358979323846264;

float clamp01(float value)
{
    return clamp(value, 0.0, 1.0);
}

void main()
{
    mat3 normalMatrix = transpose(inverse(mat3(M)));
    vec3 normal = normalize(normalMatrix * normal_Modelspace);

    vec3 fragPosition = vec3(M * vec4(position_Modelspace, 1));
    
    //calculate the vector from this pixels surface to the light source
    vec3 surfaceToLight1 = light1Position_Worldspace - fragPosition;
    vec3 surfaceToLight2 = light2Position_Worldspace - fragPosition;
    vec3 surfaceToLight3 = light3Position_Worldspace - fragPosition;
    vec3 surfaceToLight4 = light4Position_Worldspace - fragPosition;

    //calculate the cosine of the angle of incidence
    float light1Brightness = clamp01(dot(normal, surfaceToLight1) / (length(surfaceToLight1) * length(normal)) * 0.4);
    float light2Brightness = clamp01(dot(normal, surfaceToLight2) / (length(surfaceToLight2) * length(normal)) * 0.4);
    float light3Brightness = clamp01(dot(normal, surfaceToLight3) / (length(surfaceToLight3) * length(normal)) * 0.4);
    float light4Brightness = clamp01(dot(normal, surfaceToLight4) / (length(surfaceToLight4) * length(normal)) * 0.4);

    float lightBrightnessSum = 0.1 + light1Brightness + light2Brightness + light3Brightness + light4Brightness;

    vec3 fragColor = matColor.xyz;

    if (useTexture != 0)
    {
        fragColor = texture(texSampler, uv).rgb;
    }

    color = vec4(lightBrightnessSum * fragColor, matColor.a);
}