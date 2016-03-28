#include "Math/Quaternion.h"

namespace Amber
{

const Quaternion Quaternion::Identity = { 0, 0, 0, 1 };

Quaternion::Quaternion()
{
}

Quaternion::Quaternion(float x, float y, float z, float w)
	: x(x)
	, y(y)
	, z(z)
	, w(w)
{

}

Matrix4x4f Quaternion::toMatrix() const
{
	Matrix4x4f mat = Matrix4x4f::Identity;
	Quaternion a = normalize(*this);

	float xx = a.x * a.x;
	float yy = a.y * a.y;
	float zz = a.z * a.z;
	float xy = a.x * a.y;
	float xz = a.x * a.z;
	float yz = a.y * a.z;
	float wx = a.w * a.x;
	float wy = a.w * a.y;
	float wz = a.w * a.z;

	mat[0][0] = 1.0f - 2.0f * (yy + zz);
	mat[0][1] = 2.0f * (xy + wz);
	mat[0][2] = 2.0f * (xz - wy);

	mat[1][0] = 2.0f * (xy - wz);
	mat[1][1] = 1.0f - 2.0f * (xx + zz);
	mat[1][2] = 2.0f * (yz + wx);

	mat[2][0] = 2.0f * (xz + wy);
	mat[2][1] = 2.0f * (yz - wx);
	mat[2][2] = 1.0f - 2.0f * (xx + yy);

	return mat;
}

float Quaternion::angle() const
{
	return 2.f * acos(w);
}

Vector3f Quaternion::axis() const
{
	// 1 - cos(theta/2)*cos(theta/2) = sin(theta/2)*sin(theta/2)
	float s2 = 1.f - w * w;
	if (s2 <= 0.f)
		return Vector3f(0, 0, 1.f);

	float invs2 = 1.f / sqrt(s2);

	return xyz * invs2;
}

float Quaternion::roll() const
{
	return atan2(2.0f * x * y + z * w, x * x + w * w - y * y - z * z);
}

float Quaternion::pitch() const
{
	return atan2(2.0f * y * z + w * x, w * w - x * x - y * y + z * z);
}

float Quaternion::yaw() const
{
	return asin(-2.0f * (x * z - w * y));
}

Quaternion operator-(const Quaternion& lhs)
{
	return Quaternion(-lhs.x, -lhs.y, -lhs.z, -lhs.w);
}

Quaternion operator+(const Quaternion& lhs, const Quaternion& rhs)
{
	return Quaternion(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w);
}

Quaternion operator-(const Quaternion& lhs, const Quaternion& rhs)
{
	return Quaternion(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w);
}

Quaternion operator*(const Quaternion& lhs, const Quaternion& rhs)
{
	return Quaternion(lhs.w * rhs.x + lhs.x * rhs.w + lhs.y * rhs.z - lhs.z * rhs.y, lhs.w * rhs.y - lhs.x * rhs.z + lhs.y * rhs.w + lhs.z * rhs.x,
		lhs.w * rhs.z + lhs.x * rhs.y - lhs.y * rhs.x + lhs.z * rhs.w, lhs.w * rhs.w - lhs.x * rhs.x - lhs.y * rhs.y - lhs.z * rhs.z);
}

Quaternion operator*(const Quaternion& lhs, float rhs)
{
	return Quaternion(lhs.x * rhs, lhs.y * rhs, lhs.z * rhs, lhs.w * rhs);
}

Quaternion operator/(const Quaternion& lhs, float rhs)
{
	return Quaternion(lhs.x / rhs, lhs.y / rhs, lhs.z / rhs, lhs.w / rhs);
}

bool operator==(const Quaternion& lhs, const Quaternion& rhs)
{
	return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z && lhs.w == rhs.w;
}

bool operator!=(const Quaternion& lhs, const Quaternion& rhs)
{
	return lhs.x != rhs.x || lhs.y != rhs.y || lhs.z != rhs.z || lhs.w != rhs.w;
}

Quaternion operator*(float s, const Quaternion& rhs)
{
	return rhs * s;
}

float dot(const Quaternion& a, const Quaternion& b)
{
	return dot(a.xyz, b.xyz) + a.w * b.w;
}

Quaternion cross(const Quaternion& a, const Quaternion& b)
{
	return Quaternion(a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y, a.w * b.y + a.y * b.w + a.z * b.x - a.x * b.z,
		a.w * b.z + a.z * b.w + a.x * b.y - a.y * b.x, a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z);
}

float lengthSquared(const Quaternion& q)
{
	return dot(q, q);
}

float length(const Quaternion& q)
{
	return sqrt(lengthSquared(q));
}

Quaternion normalize(const Quaternion& q)
{
	return q * (1.f * length(q));
}

Quaternion conjugate(const Quaternion& q)
{
	return Quaternion(-q.x, -q.y, -q.z, q.w);
}

Quaternion inverse(const Quaternion& q)
{
	return conjugate(q) / lengthSquared(q);
}

Vector3f operator*(const Quaternion& q, const Vector3f& v)
{
	// return q * Quaternion(v, 0) * conjugate(q); // More Expensive, more cross products
	const Vector3f t = 2.0f * cross(q.xyz, v);
	return (v + q.w * t + cross(q.xyz, t));
}

Quaternion angleAxis(float angle, const Vector3f& axis)
{
	Quaternion q;

	Vector3f a = normalize(axis);
	float s = sin(0.5f * angle);

	q.xyz = a * s;
	q.w = cos(0.5f * angle);

	return q;
}

Matrix4x4f quaternionToMatrix4x4f(const Quaternion& q)
{
	return q.toMatrix();
}

Quaternion matrix4x4fToQuaternion(const Matrix4x4f& m)
{
	float fourXSquaredMinus1 = m[0][0] - m[1][1] - m[2][2];
	float fourYSquaredMinus1 = m[1][1] - m[0][0] - m[2][2];
	float fourZSquaredMinus1 = m[2][2] - m[0][0] - m[1][1];
	float fourWSquaredMinus1 = m[0][0] + m[1][1] + m[2][2];

	int biggestIndex = 0;
	float fourBiggestSquaredMinus1{ fourWSquaredMinus1 };
	if (fourXSquaredMinus1 > fourBiggestSquaredMinus1)
	{
		fourBiggestSquaredMinus1 = fourXSquaredMinus1;
		biggestIndex = 1;
	}
	if (fourYSquaredMinus1 > fourBiggestSquaredMinus1)
	{
		fourBiggestSquaredMinus1 = fourYSquaredMinus1;
		biggestIndex = 2;
	}
	if (fourZSquaredMinus1 > fourBiggestSquaredMinus1)
	{
		fourBiggestSquaredMinus1 = fourZSquaredMinus1;
		biggestIndex = 3;
	}

	float biggestVal = sqrt(fourBiggestSquaredMinus1 + 1.0f) * 0.5f;
	float mult = 0.25f / biggestVal;

	Quaternion q = Quaternion{ 0, 0, 0, 1 };

	switch (biggestIndex)
	{
	case 0:
		q.w = biggestVal;
		q.x = (m[1][2] - m[2][1]) * mult;
		q.y = (m[2][0] - m[0][2]) * mult;
		q.z = (m[0][1] - m[1][0]) * mult;
		break;
	case 1:
		q.w = (m[1][2] - m[2][1]) * mult;
		q.x = biggestVal;
		q.y = (m[0][1] + m[1][0]) * mult;
		q.z = (m[2][0] + m[0][2]) * mult;
		break;
	case 2:
		q.w = (m[2][0] - m[0][2]) * mult;
		q.x = (m[0][1] + m[1][0]) * mult;
		q.y = biggestVal;
		q.z = (m[1][2] + m[2][1]) * mult;
		break;
	case 3:
		q.w = (m[0][1] - m[1][0]) * mult;
		q.x = (m[2][0] + m[0][2]) * mult;
		q.y = (m[1][2] + m[2][1]) * mult;
		q.z = biggestVal;
		break;
	default: // Should never happen.
		break;
	}

	return q;
}

EulerAngles quaternionToEulerAngles(const Quaternion& q)
{
	return{ q.pitch(), q.yaw(), q.roll() };
}

Quaternion eulerAnglesToQuaternion(const EulerAngles& e, const Vector3f& xAxis, const Vector3f& yAxis, const Vector3f& zAxis)
{
	const Quaternion p = angleAxis(e.roll, xAxis);
	const Quaternion y = angleAxis(e.pitch, yAxis);
	const Quaternion r = angleAxis(e.yaw, zAxis);

	return y * p * r;
}

Quaternion eulerAnglesToQuaternion(const float& pitch, const float& yaw, const float& roll, const Vector3f& xAxis, const Vector3f& yAxis, const Vector3f& zAxis)
{
	return eulerAnglesToQuaternion({ pitch, yaw, roll }, xAxis, yAxis, zAxis);
}

}