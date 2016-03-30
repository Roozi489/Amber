#include "Math/Plane.h"

namespace Amber
{

Plane::Plane(Vector3f point, Vector3f normal)
	: origin(point)
	, normal(normalize(normal))
{
	a = normal.x;
	b = normal.y;
	c = normal.z;
	d = -(normal.x * origin.x + normal.y * origin.y + normal.z * origin.z);
}

Plane::Plane(Vector3f p1, Vector3f p2, Vector3f p3)
{
	normal = normalize((p2 - p1).cross(p3 - p1));
	origin = p1;

	a = normal.x;
	b = normal.y;
	c = normal.z;
	d = -(normal.x * origin.x + normal.y * origin.y + normal.z * origin.z);
}

Plane::Plane(Triangle triangle)
	: Plane(triangle.p1, triangle.p2, triangle.p3)
{

}

bool Plane::isFrontFacingTo(const Vector3f& point) const
{
	return dot(point, normal) + d > 0;
}

float Plane::signedDistanceTo(const Vector3f& point) const
{
	return dot(point, normal) + d;
}

}