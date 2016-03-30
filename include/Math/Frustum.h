#pragma once
#include "Vector.h"
#include "Plane.h"
#include "Sphere.h"
#include "AABB.h"

namespace Amber
{

enum FrustumPlanes
{
	Near,
	Far,
	Left,
	Right,
	Top,
	Bottom
};

class Frustum
{
public:
	Vector3f origin;
	Vector3f direction;
	float fovX;
	float nearDist;
	float farDist;
	Plane planes[6];

	Frustum(const Vector3f& origin, const Vector3f& dir, float fovX, float ratio, float near, float far);

	bool containsPoint(const Vector3f& point) const;
	bool containsSphere(const Sphere& sphere) const;

	bool intersectsSphere(const Sphere& sphere) const;
	bool intersectsAABB(const AABB& aabb) const;
};

	
}
