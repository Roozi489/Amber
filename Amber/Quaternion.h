#pragma once
#include "Vector.h"
#include "Matrix.h"

struct Quaternion
{
	union
	{
		struct
		{
			float x;
			float y;
			float z;
			float w;
		};
		Vector3f xyz;
		float data[4];
	};

	static const Quaternion Identity;

	Quaternion();
	Quaternion(float x, float y, float z, float w);

	Vector3f getXYZVec();
};

Quaternion operator-(const Quaternion& lhs);
Quaternion operator+(const Quaternion& lhs, const Quaternion& rhs);
Quaternion operator-(const Quaternion& lhs, const Quaternion& rhs);

Quaternion operator*(const Quaternion& lhs, const Quaternion& rhs);
Quaternion operator*(const Quaternion& lhs, float rhs);
Quaternion operator/(const Quaternion& lhs, float rhs);

Quaternion operator*(float s, const Quaternion& rhs);

bool operator==(const Quaternion& lhs, const Quaternion& rhs);
bool operator!=(const Quaternion& lhs, const Quaternion& rhs);

float dot(const Quaternion& a, const Quaternion& b);
Quaternion cross(const Quaternion& a, const Quaternion& b);

float lengthSquared(const Quaternion& q);
float length(const Quaternion& q);

Quaternion normalize(const Quaternion& q);

Quaternion conjugate(const Quaternion& q);
Quaternion inverse(const Quaternion& q);

// Rotates v by q
Vector3f operator*(const Quaternion& q, const Vector3f& v);

float angle(const Quaternion& q);
Vector3f axis(const Quaternion& q);
Quaternion angleAxis(float angle, const Vector3f& axis);

Matrix4x4f quaternionToMatrix4x4f(const Quaternion& q);
// Assumes matrix is only a rotational matrix and has no shear applied
Quaternion matrix4x4fToQuaternion(const Matrix4x4f& m);

struct EulerAngles
{
	float pitch;
	float yaw;
	float roll;
};

float roll(const Quaternion& q);
float pitch(const Quaternion& q);
float yaw(const Quaternion& q);

EulerAngles quaternionToEulerAngles(const Quaternion& q);
Quaternion eulerAnglesToQuaternion(const EulerAngles& e, const Vector3f& xAxis = { 1, 0, 0 }, const Vector3f& yAxis = { 0, 1, 0 }, const Vector3f& zAxis = { 0, 0, 1 });
Quaternion eulerAnglesToQuaternion(const float& pitch, const float& yaw, const float& roll,
                                   const Vector3f& xAxis = {1, 0, 0}, const Vector3f& yAxis = {0, 1, 0}, const Vector3f& zAxis = {0, 0, 1});