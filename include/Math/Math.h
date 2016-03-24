#pragma once
#include "Matrix.h"

namespace Amber
{

constexpr float PI = 3.141592653589793238462643383f;
constexpr float PI_HALF = 1.570796326794896619231321691f;

float toDegrees(float valueInRadians);
float toRadians(float valueInDegrees);

bool getLowestPosRoot(float a, float b, float c, float maxR, float* root);

Matrix4x4f ortho(float left, float right, float bottom, float top, float zNear, float zFar);

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

// TODO: rewrite to use variadic templates
template <typename T, typename R>
auto min(T first, R second)
{
	return first < second ? first : second;
}

template <typename T, typename R, typename S>
auto min(T first, R second, S third)
{
	return min(first, second) < third ? min(first, second) : third;
}

template <typename T, typename R>
auto max(T first, R second)
{
	return first > second ? first : second;
}

template <typename T, typename R, typename S>
auto max(T first, R second, S third)
{
	return max(first, second) > third ? max(first, second) : third;
}

}
