#pragma once

#include <string>
#include <cmath>

template <typename T>
struct Vector3
{
    T x;
    T y;
    T z;

    Vector3() = default;
    Vector3(T value);
    Vector3(T _x, T _y, T _z);

    std::string toString() const;
    T length() const;
    T lengthSquared() const;
    void normalize();
    void inverse();

    T dot(Vector3 vector);
    Vector3 cross(Vector3 vector);

    static Vector3 zero();

    Vector3 operator+(const Vector3& rhs);
    Vector3 operator-(const Vector3& rhs);
    Vector3 operator-();
    Vector3 operator*(T rhs);
    Vector3 operator/(T rhs);
    Vector3& operator+=(const Vector3& rhs);
    Vector3& operator-=(const Vector3& rhs);
    Vector3& operator*=(T rhs);
    Vector3& operator/=(T rhs);
    bool operator==(Vector3 const& rhs);
    bool operator!=(Vector3 const& rhs);
};

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
Vector3<T> cross(Vector3<T>& first, Vector3<T>& second);

template <typename T>
Vector3<T> normalize(const Vector3<T>& vec);

template <typename T>
Vector3<T> inverse(Vector3<T>& vec);

template <typename T>
Vector3<T> reflect(const Vector3<T>& vector, const Vector3<T>& normal);

template <typename T>
struct Vector2
{
    T x;
    T y;

    Vector2() = default;
    Vector2(T _x, T _y);

    void print() const;
    T magnitude() const;
    void normalize();
    void inverse();

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
Vector2<T> inverse(Vector2<T>& vec);

template <typename T>
Vector2<T> reflect(const Vector2<T>& vector, const Vector2<T>& normal);

using Vector3f = Vector3<float>;
using Vector3d = Vector3<double>;

using Vector2f = Vector2<float>;
using Vector2d = Vector2<double>;

#include "Vector.inl"