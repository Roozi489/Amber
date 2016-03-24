#pragma once
#include <string>

namespace Amber
{

// TODO: use concepts
template <typename T>
struct Vector2
{
	union
	{
		struct
		{
			T x;
			T y;
		};
		T data[2];
	};

	static const Vector2<T> Zero;

	Vector2() = default;
	Vector2(T _x, T _y);

	void print() const;
	T magnitude() const;
	void normalize();

	Vector2 operator+(const Vector2& rhs);
	Vector2 operator-(const Vector2& rhs);
	Vector2 operator-();
	Vector2 operator*(T rhs);
	Vector2 operator/(T rhs);
	bool operator==(Vector2 const& rhs);
	bool operator!=(Vector2 const& rhs);
};

template <typename T>
T dot(Vector2<T>& first, Vector2<T>& second);

template <typename T>
Vector2<T> normalize(Vector2<T>& vec);

template <typename T>
Vector2<T> reflect(const Vector2<T>& vector, const Vector2<T>& normal);

template <typename T>
struct Vector3
{
	union
	{
		struct
		{
			T x;
			T y;
			T z;
		};
		Vector2<T> xy;
		T data[3];
	};

	static const Vector3<T> Zero;
	static const Vector3<T> Up;
	static const Vector3<T> Down;
	static const Vector3<T> Forward;
	static const Vector3<T> Backward;
	static const Vector3<T> Left;
	static const Vector3<T> Right;

	Vector3() = default;
	explicit Vector3(T value);
	Vector3(T _x, T _y, T _z);

	std::string toString() const;
	T length() const;
	T lengthSquared() const;
	void normalize();

	T dot(Vector3 vector);
	Vector3 cross(Vector3 vector);

	Vector3 operator-() const;
	Vector3 operator*(T rhs) const;
	Vector3 operator/(T rhs) const;
	Vector3& operator+=(const Vector3& rhs);
	Vector3& operator-=(const Vector3& rhs);
	Vector3& operator*=(T rhs);
	Vector3& operator/=(T rhs);
	bool operator==(Vector3 const& rhs);
	bool operator!=(Vector3 const& rhs);
};

template <typename T>
Vector3<T> operator-(Vector3<T>&& rhs);

template <typename T>
Vector3<T> operator+(const Vector3<T>& lhs, const Vector3<T>& rhs);

template <typename T>
Vector3<T> operator-(const Vector3<T>& lhs, const Vector3<T>& rhs);

template <typename R, typename T>
Vector3<T> operator*(R lhs, Vector3<T> rhs);

template <typename R, typename T>
Vector3<T> operator/(R lhs, Vector3<T> rhs);

template <typename T>
T dot(const Vector3<T>& first, const Vector3<T>& second);

template <typename T>
T getAngleBetwVecs(Vector3<T>& first, Vector3<T>& second);

template <typename T>
T getAngleBetwNormVecs(Vector3<T>& first, Vector3<T>& second);

template <typename T>
T distance(const Vector3<T>& first, const Vector3<T>& second);

template <typename T>
Vector3<T> cross(const Vector3<T>& first, const Vector3<T>& second);

template <typename T>
Vector3<T> normalize(const Vector3<T>& vec);

template <typename T>
Vector3<T> reflect(const Vector3<T>& vector, const Vector3<T>& normal);

template <typename T>
struct Vector4
{
	union
	{
		struct
		{
			T x;
			T y;
			T z;
			T w;
		};
		Vector3<T> xyz;
		T data[4];
	};

	Vector4(T _x, T _y, T _z, T _w);
};

using Vector2f = Vector2<float>;
using Vector2d = Vector2<double>;

using Vector3f = Vector3<float>;
using Vector3d = Vector3<double>;

using Vector4f = Vector4<float>;
using Vector4d = Vector4<double>;

}

#include "Vector.inl"