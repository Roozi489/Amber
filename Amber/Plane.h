#pragma once
#include "Vector.h"
#include "Triangle.h"

class Plane
{
public:
    float equation[4];
    Vector3f origin;
    Vector3f normal;
    
    Plane(Vector3f point, Vector3f normal);
    Plane(Vector3f p1, Vector3f p2, Vector3f p3);
    Plane(Triangle triangle);

    bool isFrontFacingTo(const Vector3f& direction) const;    float signedDistanceTo(const Vector3f& point) const;
};

