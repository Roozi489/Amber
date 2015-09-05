#include "TransformComponent.h"


TransformComponent::TransformComponent()
    : position(0.f, 0.f, 0.f)
    , scale(1.f)
	, orientation(Quaternion::Identity)
{
}

TransformComponent::TransformComponent(Vector3f pos, Vector3f scale)
    : position(pos)
    , scale(scale)
	, orientation(Quaternion::Identity)
{
}

TransformComponent::TransformComponent(Vector3f pos, Vector3f scale, Quaternion orient)
	: position(pos)
	, scale(scale)
	, orientation(orient)
{
}

TransformComponent::TransformComponent(Vector3f pos, Vector3f scale, float angle, Vector3f axis)
    : position(pos)
    , scale(scale)
	, orientation(angleAxis(angle, axis))
{
}

TransformComponent::~TransformComponent()
{
}
