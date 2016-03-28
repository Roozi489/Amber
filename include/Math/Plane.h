#pragma once
#include "Vector.h"
#include "Triangle.h"

namespace Amber
{

class Plane
{
public:
	Vector3f origin;
	Vector3f normal;
	float equation[4];

	Plane() = default;
	Plane(Vector3f point, Vector3f normal);
	Plane(Vector3f p1, Vector3f p2, Vector3f p3);
	Plane(Triangle triangle);

	bool isFrontFacingTo(const Vector3f& point) const;
	float signedDistanceTo(const Vector3f& point) const;
};

}