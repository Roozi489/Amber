#pragma once
#include "Vector.h"
#include "Color.h"
#include "OpenGL.h"

class BaseLight
{
public:
	Color color = Color::White;
	float brightness = 1.f;
	bool isOn = true;

	GLuint positionID;
	GLuint directionID;
	GLuint powerID;

	BaseLight() = default;
    virtual ~BaseLight();
};

class PointLight : public BaseLight
{
	Vector3f position;
};

class DirectionalLight : public BaseLight
{
	Vector3f direction;
};

class SpotLight : public BaseLight
{
	Vector3f position;
	Vector3f direction;
};