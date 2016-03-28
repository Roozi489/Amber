#include "Math/Math.h"
#include "Math/Matrix.h"
#include "Math/Ray.h"

#include <cmath>
// Only needed for test
#include <iostream>

namespace Amber
{

float toDegrees(float valueInRadians)
{
	return 180.f / PI * valueInRadians;
}

float toRadians(float valueInDegrees)
{
	return PI / 180.f * valueInDegrees;
}

bool getLowestPosRoot(float a, float b, float c, float maxR, float* root)
{
	float determinant = b * b - 4 * a * c;

	if (determinant < 0.f)
		return false;

	float sqrtD = sqrt(determinant);
	float x1 = (-b - sqrtD) / (2 * a);
	float x2 = (-b + sqrtD) / (2 * a);

	if (x1 > x2)
	{
		float temp = x2;
		x2 = x1;
		x1 = temp;
	}

	if (x1 > 0 && x1 < maxR)
	{
		*root = x1;
		return true;
	}

	if (x2 > 0 && x2 < maxR)
	{
		*root = x2;
		return true;
	}

	return false;
}

Matrix4x4f Math::ortho(float left, float right, float bottom, float top, float near, float far)
{
	Matrix4x4f result = Matrix4x4f::Identity;

	result[0][0] = 2.0f / (right - left);
	result[1][1] = 2.0f / (top - bottom);
	result[2][2] = -2.0f / (far - near);
	result[3][0] = -(right + left) / (right - left);
	result[3][1] = -(top + bottom) / (top - bottom);
	result[3][2] = -(far + near) / (far - near);

	return result;
}

Matrix4x4f Math::perspectiveFov(float fovX, float ratio, float zNear)
{
	auto e = 1.f / tan(fovX / 2);
	Matrix4x4f result(0.f);
	result[0][0] = e;
	result[1][1] = e / ratio;
	result[2][2] = -1;
	result[2][3] = -1;
	result[3][2] = -2 * zNear;
	return result;
}

Matrix4x4f Math::perspectiveFov(float fovX, float ratio, float zNear, float zFar)
{
	auto e = 1.f / tan(fovX / 2);
	Matrix4x4f result(0.f);
	result[0][0] = e;
	result[1][1] = e / ratio;
	result[2][2] = -zFar / (zFar - zNear);
	result[2][3] = -1;
	result[3][2] = -(zFar * zNear) / (zFar - zNear);
	return result;
}

Matrix4x4f Math::lookAt(const Vector3f& eye, const Vector3f center, const Vector3f up)
{
	Vector3f f(normalize(center - eye));
	Vector3f s(normalize(cross(f, up)));
	Vector3f u(cross(s, f));

	Matrix4x4f result(0);
	result.fillDiag(1);

	result[0][0] = s.x;
	result[1][0] = s.y;
	result[2][0] = s.z;
	result[0][1] = u.x;
	result[1][1] = u.y;
	result[2][1] = u.z;
	result[0][2] = -f.x;
	result[1][2] = -f.y;
	result[2][2] = -f.z;
	result[3][0] = -dot(s, eye);
	result[3][1] = -dot(u, eye);
	result[3][2] = dot(f, eye);
	return result;
}

Matrix4x4f Math::translate(const Vector3f& v)
{
	return translate(Matrix4x4f::Identity, v);
}

Matrix4x4f Math::translate(const Matrix4x4f& matrix, const Vector3f& v)
{
	Matrix4x4f result(matrix);
	result[3][0] = matrix[0][0] * v.x + matrix[1][0] * v.y + matrix[2][0] * v.z + matrix[3][0];
	result[3][1] = matrix[0][1] * v.x + matrix[1][1] * v.y + matrix[2][1] * v.z + matrix[3][1];
	result[3][2] = matrix[0][2] * v.x + matrix[1][2] * v.y + matrix[2][2] * v.z + matrix[3][2];
	result[3][3] = matrix[0][3] * v.x + matrix[1][3] * v.y + matrix[2][3] * v.z + matrix[3][3];
	return result;
}

Matrix4x4f Math::rotate(float angle, Vector3f axis)
{
	return rotate(Matrix4x4f::Identity, angle, axis);
}

Matrix4x4f Math::rotate(const Matrix4x4f& matrix, float angle, Vector3f axis)
{
	float c = cos(angle);
	float s = sin(angle);

	axis.normalize();

	Vector3f temp = (1 - c) * axis;

	Matrix4x4f rot(0);
	rot[0][0] = c + temp.x * axis.x;
	rot[0][1] = 0 + temp.x * axis.y + s * axis.z;
	rot[0][2] = 0 + temp.x * axis.z - s * axis.y;

	rot[1][0] = 0 + temp.y * axis.x - s * axis.z;
	rot[1][1] = c + temp.y * axis.y;
	rot[1][2] = 0 + temp.y * axis.z + s * axis.x;

	rot[2][0] = 0 + temp.z * axis.x + s * axis.y;
	rot[2][1] = 0 + temp.z * axis.y - s * axis.x;
	rot[2][2] = c + temp.z * axis.z;

	Matrix4x4f result(0.f);
	result[0][0] = matrix[0][0] * rot[0][0] + matrix[1][0] * rot[0][1] + matrix[2][0] * rot[0][2];
	result[0][1] = matrix[0][1] * rot[0][0] + matrix[1][1] * rot[0][1] + matrix[2][1] * rot[0][2];
	result[0][2] = matrix[0][2] * rot[0][0] + matrix[1][2] * rot[0][1] + matrix[2][2] * rot[0][2];
	result[0][3] = matrix[0][3] * rot[0][0] + matrix[1][3] * rot[0][1] + matrix[2][3] * rot[0][2];
	result[1][0] = matrix[0][0] * rot[1][0] + matrix[1][0] * rot[1][1] + matrix[2][0] * rot[1][2];
	result[1][1] = matrix[0][1] * rot[1][0] + matrix[1][1] * rot[1][1] + matrix[2][1] * rot[1][2];
	result[1][2] = matrix[0][2] * rot[1][0] + matrix[1][2] * rot[1][1] + matrix[2][2] * rot[1][2];
	result[1][3] = matrix[0][3] * rot[1][0] + matrix[1][3] * rot[1][1] + matrix[2][3] * rot[1][2];
	result[2][0] = matrix[0][0] * rot[2][0] + matrix[1][0] * rot[2][1] + matrix[2][0] * rot[2][2];
	result[2][1] = matrix[0][1] * rot[2][0] + matrix[1][1] * rot[2][1] + matrix[2][1] * rot[2][2];
	result[2][2] = matrix[0][2] * rot[2][0] + matrix[1][2] * rot[2][1] + matrix[2][2] * rot[2][2];
	result[2][3] = matrix[0][3] * rot[2][0] + matrix[1][3] * rot[2][1] + matrix[2][3] * rot[2][2];
	result[3][0] = matrix[3][0];
	result[3][1] = matrix[3][1];
	result[3][2] = matrix[3][2];
	result[3][3] = matrix[3][3];
	return result;
}

void testVectorsMatricesRays()
{
	std::cout << "-----Vectors-----" << std::endl;
	Vector3f v1(1.f, 1.f, 1.f);
	Vector3f v2(0.f, -3.f, 8.f);

	std::cout << "Vector v1" << std::endl;
	v1.toString();
	std::cout << "Vector v2" << std::endl;
	v2.toString();

	std::cout << "\nNormalization of vector (0,-3,8)" << std::endl;
	std::cout << normalize(v2).toString();

	std::cout << "\nMagnitude of vector (0,-3,8)" << std::endl;
	std::cout << "Result: " << v2.length() << std::endl;

	std::cout << "\nInversion of vector (0,-3,8)" << std::endl;
	std::cout << inverse(v2).toString();

	std::cout << "\nAddition of (1,1,1) & (0,-3,8)" << std::endl;
	(v1 + v2).toString();

	std::cout << "\nSubtraction of (1,1,1) & (0,-3,8)" << std::endl;
	(v1 - v2).toString();

	std::cout << "\nDot product of (1,1,1) & (0,-3,8)" << std::endl;
	std::cout << "Result: " << dot(v1, v2) << std::endl;

	std::cout << "\nCross product of (1,1,1) & (0,-3,8)" << std::endl;
	cross(v1, v2).toString();

	std::cout << "\nScaling of vector (1,1,1) 5 times" << std::endl;
	(5 * v1).toString();
	(v1 * 5).toString();

	std::cout << "\nEquality of vector (1,1,1) & (0,-3,8)" << std::endl;
	if (v1 == v2)
		std::cout << "(1,1,1) = (0,-3,8)" << std::endl;
	else
		std::cout << "(1,1,1) != (0,-3,8)" << std::endl;

	std::cout << "\nDot product of vector (0,-3,8) and its inverse" << std::endl;
	std::cout << "Result: " << dot(v2, inverse(v2)) << std::endl;

	std::cout << "\n-----Matrices-----" << std::endl;
	Matrix3x3f m1;
	m1.data[0] = 1.f;
	m1.data[1] = 4.f;
	m1.data[2] = 7.f;
	m1.data[3] = 2.f;
	m1.data[4] = 5.f;
	m1.data[5] = 8.f;
	m1.data[6] = 3.f;
	m1.data[7] = 6.f;
	m1.data[8] = 10.f;
	Matrix3x3f m2;
	m2.data[0] = 1.f;
	m2.data[1] = 1.f;
	m2.data[2] = 1.f;
	m2.data[3] = 1.f;
	m2.data[4] = 1.f;
	m2.data[5] = 1.f;
	m2.data[6] = 1.f;
	m2.data[7] = 1.f;
	m2.data[8] = 1.f;

	std::cout << "Matrix m1" << std::endl;
	m1.toString();
	std::cout << "Matrix m2" << std::endl;
	m2.toString();

	std::cout << "\nAddition of m1 and m2" << std::endl;
	(m1 + m2).toString();

	std::cout << "\nSubtraction of m1 and m2" << std::endl;
	(m1 - m2).toString();

	std::cout << "\nMatrix multiplication m1*m2" << std::endl;
	(m1 * m2).toString();

	std::cout << "\nMatrix multiplication m2*m1" << std::endl;
	(m2 * m1).toString();

	std::cout << "\nTransposed matrix 1" << std::endl;
	Matrix3x3f m1_tmp = m1;
	m1_tmp.transpose();
	m1_tmp.toString();

	std::cout << "\nInversed matrix 1" << std::endl;
	m1_tmp = m1;
	m1_tmp.inverse();
	m1_tmp.toString();

	std::cout << "\nDeterminant of matrix 1" << std::endl;
	std::cout << "Result: " << m1.determinant() << std::endl;

	std::cout << "\n-----Rays-----" << std::endl;
	Rayf r1(Vector3f(0, 2, -1), Vector3f(1, 1, 2));
	Rayf r2(Vector3f(1, 0, -1), Vector3f(1, 1, 3));

	std::cout << "\nDistance from r1 to r2" << std::endl;
	std::cout << "Result: " << distance(r1, r2) << std::endl;

	std::cout << "\nDistance from r1 to v1" << std::endl;
	std::cout << "Result: " << distance(r1, v1) << std::endl;
}

}