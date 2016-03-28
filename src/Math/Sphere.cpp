#include "Math/Sphere.h"

namespace Amber
{

Sphere::Sphere(Vector3f origin, float radius)
	: center(origin)
	, radius(radius)
{
}

Sphere::~Sphere()
{
}

}