#include "Plane.h"

Plane::Plane(Vector3f point, Vector3f normal)
: origin(point)
, normal(normalize(normal))
{
    equation[0] = normal.x;
    equation[1] = normal.y;
    equation[2] = normal.z;
    equation[3] = -(normal.x * origin.x + normal.y * origin.y + normal.z * origin.z);
}

Plane::Plane(Vector3f p1, Vector3f p2, Vector3f p3)
{
    normal = normalize((p2 - p1).cross(p3 - p1));
    origin = p1;

    equation[0] = normal.x;
    equation[1] = normal.y;
    equation[2] = normal.z;
    equation[3] = -(normal.x * origin.x + normal.y * origin.y + normal.z * origin.z);
}

Plane::Plane(Triangle triangle)
    : Plane(triangle.p1, triangle.p2, triangle.p3)
{

}

bool Plane::isFrontFacingTo(const Vector3f& direction) const
{
    return dot(normal, direction) <= 0;
}

float Plane::signedDistanceTo(const Vector3f& point) const
{
    return dot(point, normal) + equation[3];
}

