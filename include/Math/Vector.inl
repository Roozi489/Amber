#include <iostream>

namespace Amber
{

template <typename T>
const Vector3<T> Vector3<T>::Zero = { 0, 0, 0 };

template <typename T>
const Vector3<T> Vector3<T>::Up = { 0, 1, 0 };

template <typename T>
const Vector3<T> Vector3<T>::Down = { 0, -1, 0 };

template <typename T>
const Vector3<T> Vector3<T>::Forward = { 0, 0, -1 };

template <typename T>
const Vector3<T> Vector3<T>::Backward = { 0, 0, 1 };

template <typename T>
const Vector3<T> Vector3<T>::Left = { -1, 0, 0 };

template <typename T>
const Vector3<T> Vector3<T>::Right = { 1, 0, 0 };

template <typename T>
Vector3<T>::Vector3(T value)
	: x(value)
	, y(value)
	, z(value)
{
}

template <typename T>
Vector3<T>::Vector3(T _x, T _y, T _z)
	: x(_x)
	, y(_y)
	, z(_z)
{
}

template <typename T>
std::string Vector3<T>::toString() const
{
	return std::string("x: " + std::to_string(x) + "  y: " + std::to_string(y) + "  z: " + std::to_string(z));
}

template <typename T>
T Vector3<T>::length() const
{
	return std::sqrt(x*x + y*y + z*z);
}

template <typename T>
T Vector3<T>::lengthSquared() const
{
	return x*x + y*y + z*z;
}

template <typename T>
void Vector3<T>::normalize()
{
	T lenght = std::sqrt(x*x + y*y + z*z);
	x /= lenght;
	y /= lenght;
	z /= lenght;
}

template <typename T>
T Vector3<T>::dot(Vector3 vector)
{
	return x * vector.x + y * vector.y + z * vector.z;
}

template <typename T>
Vector3<T> Vector3<T>::cross(Vector3 vector)
{
	return Vector3<T>(y * vector.z - z * vector.y, z * vector.x - x * vector.z, x * vector.y - y * vector.x);
}

template <typename T>
Vector3<T>& Vector3<T>::operator+=(const Vector3& rhs) { return *this = *this + rhs; }

template <typename T>
Vector3<T>& Vector3<T>::operator-=(const Vector3& rhs) { return *this = *this - rhs; }

template <typename T>
Vector3<T> Vector3<T>::operator-() const { return Vector3(-x, -y, -z); }

template <typename T>
Vector3<T> Vector3<T>::operator*(T rhs) const { return Vector3(x * rhs, y * rhs, z * rhs); }

template <typename T>
Vector3<T> Vector3<T>::operator/(T rhs) const { return Vector3(x / rhs, y / rhs, z / rhs); }

template <typename T>
Vector3<T>& Vector3<T>::operator*=(T rhs) { return *this = *this * rhs; }

template <typename T>
Vector3<T>& Vector3<T>::operator/=(T rhs) { return *this = *this / rhs; }

template <typename T>
bool Vector3<T>::operator==(Vector3 const& rhs) { return x == rhs.x && y == rhs.y && z == rhs.z; }

template <typename T>
bool Vector3<T>::operator!=(Vector3 const& rhs) { return x != rhs.x || y != rhs.y || z != rhs.z; }

template <typename T>
Vector3<T> operator-(Vector3<T>&& rhs) { return Vector3<T>(-rhs.x, -rhs.y, -rhs.z); }

template <typename T>
Vector3<T> operator+(const Vector3<T>& lhs, const Vector3<T>& rhs) { return Vector3<T>(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z); }

template <typename T>
Vector3<T> operator-(const Vector3<T>& lhs, const Vector3<T>& rhs) { return Vector3<T>(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z); }

template <typename R, typename T>
Vector3<T> operator*(R lhs, Vector3<T> rhs) { return Vector3<T>(lhs * rhs.x, lhs * rhs.y, lhs * rhs.z); }

template <typename R, typename T>
Vector3<T> operator/(R lhs, Vector3<T> rhs) { return Vector3<T>(T(lhs) / rhs.x, T(lhs) / rhs.y, T(lhs) / rhs.z); }

template <typename T>
bool operator==(const Vector3<T>& lhs, const Vector3<T>& rhs) { return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z; }

template <typename T>
bool operator!=(const Vector3<T>& lhs, const Vector3<T>& rhs) { return lhs.x != rhs.x || lhs.y != rhs.y || lhs.z != rhs.z; }

template <typename T>
T dot(const Vector3<T>& first, const Vector3<T>& second)
{
	return first.x * second.x + first.y * second.y + first.z * second.z;
}

template <typename T>
T getAngleBetwVecs(Vector3<T>& first, Vector3<T>& second)
{
	T result = dot(first, second);
	result /= (first.length() * second.length());
	return std::acos(result);
}

template <typename T>
T getAngleBetwNormVecs(Vector3<T>& first, Vector3<T>& second)
{
	return std::acos(dot(first, second));
}

template <typename T>
T distance(const Vector3<T>& first, const Vector3<T>& second)
{
	T xDist = first.x - second.x;
	T yDist = first.y - second.y;
	T zDist = first.z - second.z;
	return std::sqrt(xDist * xDist + yDist * yDist + zDist * zDist);
}

template <typename T>
T length(const Vector3<T>& v)
{
	return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

template <typename T>
Vector3<T> cross(const Vector3<T>& first, const Vector3<T>& second)
{
	return Vector3<T>(first.y * second.z - first.z * second.y, first.z * second.x - first.x * second.z, first.x * second.y - first.y * second.x);
}

template <typename T>
Vector3<T> normalize(const Vector3<T>& vec)
{
	T lenght = std::sqrt(vec.x*vec.x + vec.y*vec.y + vec.z*vec.z);
	Vector3<T> tmp;
	tmp.x = vec.x / lenght;
	tmp.y = vec.y / lenght;
	tmp.z = vec.z / lenght;
	return tmp;
}

template <typename T>
Vector3<T> inverse(Vector3<T>& vec)
{
	return Vector3<T>(vec.x, vec.y, vec.z) / (vec.length() * vec.length());
}

template <typename T>
Vector3<T> reflect(const Vector3<T>& vector, const Vector3<T>& normal)
{
	Vector3<T> result;
	float dotTimesTwo = 2.f * dot(vector, normal);
	result.x = vector.x - dotTimesTwo * normal.x;
	result.y = vector.y - dotTimesTwo * normal.y;
	result.z = vector.z - dotTimesTwo * normal.z;
	return result;
}


//
//
//	Vector2
//
//

template <typename T>
const Vector2<T> Vector2<T>::Zero = { 0, 0 };

template <typename T>
Vector2<T>::Vector2(T _x, T _y)
	: x(_x)
	, y(_y)
{
}

template <typename T>
void Vector2<T>::print() const
{
	std::cout.setf(std::ios::fixed);
	std::cout << "x: " << x << "  y: " << y << std::endl;
}

template <typename T>
T Vector2<T>::length() const
{
	return std::sqrt(x*x + y*y);
}

template <typename T>
void Vector2<T>::normalize()
{
	auto lenght = std::sqrt(x*x + y*y);
	x /= lenght;
	y /= lenght;
}

template <typename T>
Vector2<T> Vector2<T>::operator+(const Vector2& rhs) { return Vector2(x + rhs.x, y + rhs.y); }

template <typename T>
Vector2<T> Vector2<T>::operator-(const Vector2& rhs) { return Vector2(x - rhs.x, y - rhs.y); }

template <typename T>
Vector2<T> Vector2<T>::operator-() { return Vector2(-x, -y); }

template <typename T>
Vector2<T> Vector2<T>::operator*(T rhs) { return Vector2(rhs * x, rhs * y); }

template <typename T>
Vector2<T> Vector2<T>::operator/(T rhs) { return Vector2(rhs / x, rhs / y); }

template <typename T>
bool Vector2<T>::operator==(Vector2 const& rhs) { return x == rhs.x && y == rhs.y; }

template <typename T>
bool Vector2<T>::operator!=(Vector2 const& rhs) { return x != rhs.x || y != rhs.y; }

template <typename T>
bool operator==(const Vector2<T>& lhs, const Vector2<T>& rhs) { return lhs.x == rhs.x && lhs.y == rhs.y; }

template <typename T>
bool operator!=(const Vector2<T>& lhs, const Vector2<T>& rhs) { return lhs.x != rhs.x || lhs.y != rhs.y; }

template <typename T>
T dot(Vector2<T>& first, Vector2<T>& second)
{
	return first.x * second.x + first.y * second.y;
}

template <typename T>
Vector2<T> normalize(Vector2<T>& vec)
{
	Vector2<T> tmp(vec);
	tmp.normalize();
	return tmp;
}

template <typename T>
Vector2<T> inverse(Vector2<T>& vec)
{
	return vec / (vec.magnitude() * vec.magnitude());
}

template <typename T>
Vector2<T> reflect(const Vector2<T>& vector, const Vector2<T>& normal)
{
	Vector2<T> result;
	float dotTimesTwo = 2.f * dot(vector, normal);
	result.x = vector.x - dotTimesTwo * normal.x;
	result.y = vector.y - dotTimesTwo * normal.y;
	return result;
}

template <typename T>
Vector4<T>::Vector4(T _x, T _y, T _z, T _w)
	: x(_x)
	, y(_y)
	, z(_z)
	, w(_w)
{
}

}

namespace std
{
	using namespace Amber;

	template <class T>
	inline void hash_combine(size_t& seed, const T& v)
	{
		hash<T> hasher;
		seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	}

	template<typename T>
	struct hash<Vector2<T>>
	{
		size_t operator()(Vector2<T> const& vector) const
		{
			size_t h = std::hash<T>()(vector.data[0]);
			hash_combine(h, vector.data[1]);
			return h;
		}
	};

	template<typename T>
	struct hash<Vector3<T>>
	{
		size_t operator()(Vector3<T> const& vector) const
		{
			size_t h = std::hash<T>()(vector.data[0]);
			hash_combine(h, vector.data[1]);
			hash_combine(h, vector.data[2]);
			return h;
		}
	};

	template<typename T>
	struct hash<Vector4<T>>
	{
		size_t operator()(Vector4<T> const& vector) const
		{
			size_t h = std::hash<T>()(vector.data[0]);
			hash_combine(h, vector.data[1]);
			hash_combine(h, vector.data[2]);
			hash_combine(h, vector.data[3]);
			return h;
		}
	};
}