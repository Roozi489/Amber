#pragma once
#include "Matrix.h"

namespace Amber
{

constexpr float PI = 3.141592653589793238462643383f;
constexpr float PI_HALF = 1.570796326794896619231321691f;

float toDegrees(float valueInRadians);
float toRadians(float valueInDegrees);

bool getLowestPosRoot(float a, float b, float c, float maxR, float* root);

class Math
{
public:
	static Matrix4x4f ortho(float left, float right, float bottom, float top, float zNear, float zFar);
	static Matrix4x4f perspectiveFov(float fovX, float ratio, float zNear);
	static Matrix4x4f perspectiveFov(float fovX, float ratio, float zNear, float zFar);
	static Matrix4x4f lookAt(const Vector3f& eye, const Vector3f center, const Vector3f up);

	static Matrix4x4f translate(const Vector3f& v);
	static Matrix4x4f translate(const Matrix4x4f& matrix, const Vector3f& v);
	static Matrix4x4f rotate(float angle, Vector3f axis);
	static Matrix4x4f rotate(const Matrix4x4f& matrix, float angle, Vector3f axis);
};

// TODO: make actual tests, preferably unit tests
void testVectorsMatricesRays();


template <typename T>
T clamp(T value, T min = 0, T max = 1)
{
	if (value < min)
		value = min;
	if (value > max)
		value = max;

	return value;
}

template <typename T>
T min(T a, T b)
{
	return a < b ? a : b;
}

template <typename T, typename ... Args>
T min(T a, T b, Args ... args)
{
	return max(max(a, b), args...);
}

template <typename T>
T max(T a, T b)
{
	return a > b ? a : b;
}

template <typename T, typename ... Args>
T max(T a, T b, Args ... args)
{
	return max(max(a, b), args...);
}

}
