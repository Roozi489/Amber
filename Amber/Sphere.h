#pragma once
#include "Vector.h"

class Sphere
{
public:
    Vector3f origin;
    float radius;

    Sphere() = default;
    Sphere(Vector3f origin, float radius);
    ~Sphere();
};

