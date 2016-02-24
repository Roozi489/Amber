#pragma once
#include "Vector.h"

#include <string>

template <typename T>
struct Matrix4x4
{
	static const int rowsCount = 4;
	static const int columnsCount = 4;
	static const int dataLength = 16;

	static const Matrix4x4<T> Identity;
	
	union
	{
		struct
		{
			Vector4<T> x;
			Vector4<T> y;
			Vector4<T> z;
			Vector4<T> w;
		};
		Vector4<T> columnVectors[columnsCount];
		T data[dataLength];
	};

	// Default constructor doesn't initialize values
	Matrix4x4();
	// Fills the _whole_ matrix with value specified 
	explicit Matrix4x4(T value);

	Matrix4x4(const Matrix4x4& other);
	Matrix4x4& operator=(const Matrix4x4& other);
	Matrix4x4(T v0, T v1, T v2, T v3, T v4, T v5, T v6, T v7, T v8, T v9, T v10, T v11, T v12, T v13, T v14, T v15);
	// Fills the _whole_ matrix with value specified 
	void init(T value = 0);
	// Fills the diagonal with the value, other values are not changed
	void fillDiag(T value);

	T* operator[](int column);
	const T* operator[](int column) const;
	T getValue(int column, int row) const;
	void setValue(int column, int row, T value);

	void transpose();
	// Computes determinant using Laplace expansion
	T determinant();
	void isInvertible();
	// Inverts matrix using Gaussian elimination
	void inverse();
	std::string toString() const;

	Matrix4x4 operator+(const Matrix4x4& rhs);
	Matrix4x4 operator-(const Matrix4x4& rhs);
	Matrix4x4 operator*(const Matrix4x4& rhs);
	Matrix4x4 operator*(const T& rhs);
	Vector3<T> operator*(const Vector3<T>& rhs);
	Matrix4x4 operator/(const T& rhs);

	// Returns identity matrix (diagonal values are 1, others are 0)
	static Matrix4x4 identity();
	// Creates translate matrix
	static Matrix4x4 translate(const Vector3<T>& v);
	// Translates the specified matrix by vector
	static Matrix4x4 translate(const Matrix4x4<T>& matrix, const Vector3<T>& v);
	// Creates rotation matrix
	static Matrix4x4 rotate(T angle, Vector3<T> axis);
	// Rotates the specified matrix by the angle (in radians) around the axis
	static Matrix4x4 rotate(Matrix4x4& matrix, T angle, Vector3<T> axis);
	// Returns perspective projection matrix from the values specified
	static Matrix4x4 perspectiveFov(float fov, float width, float height, float zNear, float zFar);
	// Returns view matrix from the values specified
	static Matrix4x4 lookAt(Vector3<T>& eye, Vector3<T> center, Vector3<T> up);
};

template <typename T>
Matrix4x4<T> inverse(const Matrix4x4<T>& matrix);

template <typename T>
Matrix4x4<T> transpose(const Matrix4x4<T>& matrix);

template <typename T>
Matrix4x4<T> operator*(const Matrix4x4<T>& lhs, const Matrix4x4<T>& rhs);

template <typename T>
Matrix4x4<T> operator*(const T& lhs, const Matrix4x4<T>& rhs);

template <typename T>
Matrix4x4<T> operator/(const T& lhs, const Matrix4x4<T>& rhs);

template <typename T>
struct Matrix3x3
{
	static const int rowsCount = 3;
	static const int columnsCount = 3;
	static const int dataLength = 9;

	static const Matrix3x3<T> Identity;

	union
	{
		struct
		{
			Vector3<T> x;
			Vector3<T> y;
			Vector3<T> z;
		};
		Vector3<T> columnVectors[columnsCount];
		T data[dataLength];
	};

	Matrix3x3() = default;
	Matrix3x3(T v0, T v1, T v2, T v3, T v4, T v5, T v6, T v7, T v8);
	Matrix3x3(const Matrix3x3& other);
	Matrix3x3& operator=(const Matrix3x3& other);
	void init(T value = 0);
	// Fills the diagonal with the value, other values are not changed
	void fillDiag(T value);

	T* operator[](int column);
	T getValue(int column, int row) const;
	void setValue(int column, int row, T value);

	void transpose();
	// Computes determinant using Sarrus rule
	T determinant();
	void isInvertible();
	// Inverts matrix using Gaussian elimination
	void inverse();
	std::string toString() const;

	Matrix3x3 operator+(const Matrix3x3& rhs);
	Matrix3x3 operator-(const Matrix3x3& rhs);
	Matrix3x3 operator*(const Matrix3x3& rhs);
	Matrix3x3 operator*(const T& rhs);
	Matrix3x3 operator/(const T& rhs);

	// Returns identity matrix (diagonal values are 1, others are 0)
	static Matrix3x3 identity();
};

template <typename T>
Matrix3x3<T> operator*(const T& lhs, const Matrix3x3<T>& rhs);

template <typename T>
Matrix3x3<T> operator/(const T& lhs, const Matrix3x3<T>& rhs);


using Matrix4x4f = Matrix4x4<float>;
using Matrix4x4d = Matrix4x4<double>;

using Matrix3x3f = Matrix3x3<float>;
using Matrix3x3d = Matrix3x3<double>;

#include "Matrix.inl"