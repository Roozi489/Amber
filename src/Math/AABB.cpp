#include "Math/AABB.h"

namespace Amber
{

AABB::AABB(const Vector3f& center, const Vector3f& halfSize)
	: center(center)
	, halfSize(halfSize)
{
}

bool AABB::contains(const Vector3f& v) const
{
	Vector3f distance{ center - v };

	return (abs(distance.x) <= halfSize.x) && (abs(distance.y) <= halfSize.y) && (abs(distance.z) <= halfSize.z);
}

AABB AABB::fromMinMax(const Vector3f& min, const Vector3f& max)
{
	AABB aabb;
	aabb.center = (max + min) / 2;
	aabb.halfSize = max - aabb.center;
	return aabb;
}

}
