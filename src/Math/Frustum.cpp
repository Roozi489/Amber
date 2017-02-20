#include "Math/Frustum.h"
#include "Core/Utility.h"

namespace Amber
{
Frustum::Frustum(const Vector3f& origin, const Vector3f& dir, float fovX, float ratioHeightToWidth, float near, float far)
	: origin(origin)
	, direction(normalize(dir))
	, fovX(fovX)
	, nearDist(near)
	, farDist(far)
{
	float nearWidth = 2 * tan(fovX / 2) * nearDist;
	float nearHeight = nearWidth * ratioHeightToWidth;

	float farWidth = 2 * tan(fovX / 2) * farDist;
	float farHeight = farWidth * ratioHeightToWidth;

	assert_amber(direction != Vector3f::Up, "Frustum direction vector is the same as global Up vector.");

	Vector3f nearCenter = origin + direction * nearDist;
	Vector3f farCenter = origin + direction * farDist;
	Vector3f right = normalize(cross(direction, Vector3f::Up));
	Vector3f up = normalize(cross(right, direction));
	Vector3f pointToRightEdge = normalize((nearCenter + right * nearWidth / 2) - origin);
	Vector3f pointToLeftEdge = normalize((nearCenter - right * nearWidth / 2) - origin);
	Vector3f pointToTopEdge = normalize((nearCenter + up * nearHeight / 2) - origin);
	Vector3f pointToBottomEdge = normalize((nearCenter - up * nearHeight / 2) - origin);

	// normals point inside the frustum
	Vector3f normalRight = cross(up, pointToRightEdge);
	Vector3f normalLeft = -cross(up, pointToLeftEdge);
	Vector3f normalTop = -cross(right, pointToTopEdge);
	Vector3f normalBottom = cross(right, pointToBottomEdge);

	planes[FrustumPlanes::Near] = { nearCenter, direction };
	planes[FrustumPlanes::Far] = { farCenter, -direction };
	planes[FrustumPlanes::Right] = { origin, normalRight };
	planes[FrustumPlanes::Left] = { origin, normalLeft };
	planes[FrustumPlanes::Top] = { origin, normalTop };
	planes[FrustumPlanes::Bottom] = { origin, normalBottom };
}

bool Frustum::containsPoint(const Vector3f& point) const
{
	for (const auto& plane : planes)
	{
		if (!plane.isFrontFacingTo(point))
			return false;
	}

	return true;
}

bool Frustum::containsSphere(const Sphere& sphere) const
{
	for (const auto& plane : planes)
	{
		float dist = plane.signedDistanceTo(sphere.center);
		if (dist < sphere.radius)
			return false;
	}

	return true;
}

bool Frustum::intersectsAABB(const AABB& aabb) const
{
	for (const auto& plane : planes)
	{
		Vector3f positive{ aabb.minCoord().x, aabb.minCoord().y, aabb.minCoord().z };
		if (plane.normal.x >= 0)
			positive.x = aabb.maxCoord().x;
		if (plane.normal.y >= 0)
			positive.y = aabb.maxCoord().y;
		if (plane.normal.z >= 0)
			positive.z = aabb.maxCoord().z;

		if (plane.signedDistanceTo(positive) < 0)
			return false;
	}
	return true;
}

bool Frustum::intersectsSphere(const Sphere& sphere) const
{
	for (const auto& plane : planes)
	{
		float dist = plane.signedDistanceTo(sphere.center);
		if (dist < -sphere.radius)
			return false;
	}

	return true;
}

}
