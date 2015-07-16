#pragma once
#include <string>

template <typename T>
struct Matrix4x4
{
    static const int rows = 4;
    static const int columns = 4;
    static const int dataLength = 16;

    T data[dataLength];

    // Default constructor doesn't initialize values
    Matrix4x4() = default;
    // Fills the _whole_ matrix with value specified 
    Matrix4x4(T value);

    Matrix4x4(const Matrix4x4& other);
    Matrix4x4& operator=(const Matrix4x4& other);
    // Fills the _whole_ matrix with value specified 
    void init(T value = 0);
    // Fills the diagonal with the value, other values are not changed
    void fillDiag(T value);

    T* operator[](int column);
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
    static Matrix4x4 translate(Vector3<T>& v);
    // Translates the specified matrix by vector
    static Matrix4x4 translate(Matrix4x4<T>& matrix, Vector3<T>& v);
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
Matrix4x4<T> operator*(const T& lhs, const Matrix4x4<T>& rhs);

template <typename T>
Matrix4x4<T> operator/(const T& lhs, const Matrix4x4<T>& rhs);

template <typename T>
struct Matrix3x3
{
    static const int rows = 3;
    static const int columns = 3;
    static const int dataLength = 9;

    T data[dataLength];

    Matrix3x3() = default;
    Matrix3x3(const Matrix3x3& other);
    Matrix3x3& operator=(const Matrix3x3& other);
    void init(T value = 0);

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