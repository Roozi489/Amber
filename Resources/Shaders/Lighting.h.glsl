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

struct DirectionalLight
{
    BaseLight base;

    vec3 direction;
};

struct PointLight
{
    BaseLight base;

    vec3 position;
    Attenuation attenuation;
};

struct SpotLight
{
    PointLight base;

    vec3 direction;
    float coneAngle;
};