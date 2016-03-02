#include "Components/TransformComponent.h"

namespace Amber
{

TransformComponent::TransformComponent()
	: position(0.f, 0.f, 0.f)
	, orientation(Quaternion::Identity)
	, scale(1.f)
{
}

TransformComponent::TransformComponent(Vector3f pos, Vector3f scale)
	: position(pos)
	, orientation(Quaternion::Identity)
	, scale(scale)
{
}

TransformComponent::TransformComponent(Vector3f pos, Quaternion orient, Vector3f scale)
	: position(pos)
	, orientation(orient)
	, scale(scale)
{
}

TransformComponent::TransformComponent(Vector3f pos, float angle, Vector3f axis, Vector3f scale)
	: position(pos)
	, orientation(angleAxis(angle, axis))
	, scale(scale)
{
}

TransformComponent::~TransformComponent()
{
}

}