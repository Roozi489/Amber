#pragma once
#include "Math/Vector.h"
#include "Math/Math.h"
#include "Color.h"

namespace Amber
{

struct Attenuation
{
	// Defaults to Unreals values
	float constant = 1.0f;
	float linear = 0.0f;
	float quadratic = 1.0f;

	float range = 50.0f;
};

struct BaseLight
{
	Color color = Color::White;
	float intensity = 1.f;
	bool isOn = true;

	BaseLight() = default;
	virtual ~BaseLight() = default;
};

using AmbientLight = BaseLight;

struct PointLight : BaseLight
{
	Vector3f position = Vector3f::Zero;
	Attenuation attenuation = {};
};

struct DirectionalLight : BaseLight
{
	Vector3f direction = Vector3f::Down;
};

struct SpotLight : PointLight
{
	Vector3f direction = Vector3f::Down;
	float coneAngle = toRadians(45);
};

}