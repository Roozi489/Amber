#pragma once
#include "Vector.h"

namespace Amber
{

class Sphere
{
public:
	Vector3f center;
	float radius;

	Sphere() = default;
	Sphere(Vector3f origin, float radius);
	~Sphere();
};

}