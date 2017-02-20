#pragma once
#include "Math/Vector.h"

namespace Amber
{

class AABB
{
public:
	Vector3f center;
	Vector3f halfSize;


	AABB(const Vector3f& center, const Vector3f& halfSize);

	AABB() = default;
	~AABB() = default;

	inline Vector3f minCoord() const { return center - halfSize; };
	inline Vector3f maxCoord() const { return center + halfSize; };

	inline float right() const { return (center + halfSize).x; };
	inline float left() const { return (center - halfSize).x; };
	inline float top() const { return (center + halfSize).y; };
	inline float bottom() const { return (center - halfSize).y; };
	inline float front() const { return (center + halfSize).z; };
	inline float back() const { return (center - halfSize).z; };

	bool contains(const Vector3f& v) const;

	static AABB fromMinMax(const Vector3f & min, const Vector3f & max);
};

}