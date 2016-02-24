#version 330 core

struct Attenuation
{
    float constant;
    float linear;
    float quadratic;
    float range;
};

struct BaseLight
{
    vec3 coloredIntensity; // color * intensity
};

struct PointLight
{
    BaseLight base;

    vec3 position;
    Attenuation attenuation;
};

uniform sampler2D specular;
uniform sampler2D normal;
uniform sampler2D depth;

uniform PointLight light;
uniform mat4 vpInverse;

in vec2 texCoord;

out vec4 color;

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

vec4 calculatePointLight(in vec3 surfaceToLight, in float distanceToLight, in vec3 normal)
{
    float diffuseCoefficient = max(0.0, dot(normal, surfaceToLight));

    float falloff = saturate(square(1.0 - square(square(distanceToLight / light.attenuation.range)))) / 
                    (light.attenuation.constant + light.attenuation.linear * distanceToLight + light.attenuation.quadratic * square(distanceToLight));

    if (distanceToLight > light.attenuation.range)
        return vec4(0.0, 0.0, 0.0, 1.0);

    vec4 diffuse;
    diffuse.rgb = diffuseCoefficient * light.base.coloredIntensity * falloff;
    diffuse.a = 1.0;

    return diffuse;
}

void main()
{
    // TODO: add specular
    vec3 specularColor = texture(specular, texCoord).rgb;
    vec3 normalEncoded = texture(normal, texCoord).xyz;
    float depthValue = texture(depth, texCoord).x;

    vec3 position = calculatePositionFromDepth(texCoord, gl_FragCoord.w, depthValue, vpInverse);
    vec3 normal = normalize(2.0 * normalEncoded - vec3(1.0));

    vec3 surfaceToLight = normalize(light.position - position);
    float distanceToLight = length(light.position - position);

    color = calculatePointLight(surfaceToLight, distanceToLight, normal);
}