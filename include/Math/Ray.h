#pragma once

namespace Amber
{

#include "Vector.h"

template <typename T>
struct Ray
{
	Vector3<T> origin;
	Vector3<T> direction;

	Ray() = default;
	Ray(Vector3<T> orig, Vector3<T> dir);

	void print();

};

template <typename T>
T distance(Ray<T>& first, Ray<T>& second);
template <typename T>
T distance(Ray<T>& ray, Vector3<T>& point);

using Rayf = Ray<float>;
using Rayd = Ray<double>;

}

#include "Ray.inl"