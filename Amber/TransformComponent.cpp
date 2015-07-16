#include "TransformComponent.h"


TransformComponent::TransformComponent()
    : position(0.f, 0.f, 0.f)
    , scale(1.f, 1.f, 1.f)
    , angle(0.f)
    , axis(0.f, 1.f, 0.f)
{
}

TransformComponent::TransformComponent(Vector3f pos, Vector3f scale)
    : position(pos)
    , scale(scale)
    , angle(0.f)
    , axis(0.f, 1.f, 0.f)
{

}

TransformComponent::TransformComponent(Vector3f pos, Vector3f scale, float angle, Vector3f axis)
    : position(pos)
    , scale(scale)
    , angle(angle)
    , axis(axis)
{

}

TransformComponent::~TransformComponent()
{
}
