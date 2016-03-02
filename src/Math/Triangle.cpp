#include "Math/Triangle.h"

namespace Amber
{

Triangle::Triangle(Vector3f p1, Vector3f p2, Vector3f p3)
	: p1(p1)
	, p2(p2)
	, p3(p3)
{

}

Triangle::Triangle(Vector3f p1, Vector3f p2, Vector3f p3, Vector3f n1, Vector3f n2, Vector3f n3)
	: p1(p1)
	, p2(p2)
	, p3(p3)
	, n1(n1)
	, n2(n2)
	, n3(n3)
{

}

Triangle::~Triangle()
{
}

bool checkPointInTriangle(const Vector3f& point, const Triangle& triangle)
{
	Vector3f v0 = triangle.p3 - triangle.p1;
	Vector3f v1 = triangle.p2 - triangle.p1;
	Vector3f v2 = point - triangle.p1;

	float dot00 = dot(v0, v0);
	float dot01 = dot(v0, v1);
	float dot02 = dot(v0, v2);
	float dot11 = dot(v1, v1);
	float dot12 = dot(v1, v2);

	// Compute barycentric coordinates
	float invDenom = 1.f / (dot00 * dot11 - dot01 * dot01);
	float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
	float v = (dot00 * dot12 - dot01 * dot02) * invDenom;

	// Check if point is in triangle
	return (u >= 0) && (v >= 0) && (u + v < 1);

	// Faster version
	// Thanks to Keidy from Mr-Gamemaker
	/*Vector3f e10 = triangle.p2 - triangle.p1;
	Vector3f e20 = triangle.p3 - triangle.p1;
	float a = dot(e10, e10);
	float b = dot(e10, e20);
	float c = dot(e20, e20);
	float ac_bb = (a*c) - (b*b);
	Vector3f vp(point.x - triangle.p1.x, point.y - triangle.p1.y, point.z - triangle.p1.z);
	float d = vp.dot(e10);
	float e = vp.dot(e20);
	float x = (d*c) - (e*b);
	float y = (e*a) - (d*b);
	float z = x + y - ac_bb;

	// WTF???
	typedef unsigned int uint32;
	return ((((uint32&)z)& ~(((uint32&)x) | ((uint32&)y))) & 0x80000000);*/
}

}