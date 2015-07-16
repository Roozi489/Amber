#pragma once
#include "Vector.h"

class Triangle
{
public:
    Vector3f p1, p2, p3;
    Vector3f n1, n2, n3;

    Triangle() = default;
    Triangle(Vector3f p1, Vector3f p2, Vector3f p3);
    Triangle(Vector3f p1, Vector3f p2, Vector3f p3, Vector3f n1, Vector3f n2, Vector3f n3);
    ~Triangle();
};

bool checkPointInTriangle(const Vector3f& point, const Triangle& triangle);