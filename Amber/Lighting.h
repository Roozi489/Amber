#pragma once
#include "Vector.h"
#include "Color.h"
#include "Utility.h"

class Attenuation
{
public:
	// Defaults to Unreals values
	float constant = 1.0f;
	float linear = 0.0f;
	float quadratic = 1.0f;
	float range = 50.0f;
};

class BaseLight
{
public:
	Color color = Color::White;
	float intensity = 1.f;
	bool isOn = true;

	BaseLight() = default;
    virtual ~BaseLight() = default;
};

using AmbientLight = BaseLight;

class PointLight : public BaseLight
{
public:
	Vector3f position = Vector3f::Zero;
	Attenuation attenuation = {};
};

class DirectionalLight : public BaseLight
{
public:
	Vector3f direction = Vector3f::Down;;
};

class SpotLight : public PointLight
{
public:
	Vector3f direction = Vector3f::Down;
	float coneAngle = toRadians(45);
};