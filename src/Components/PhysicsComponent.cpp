#include "Components/PhysicsComponent.h"
#include "Core/Globals.h"

namespace Amber
{

PhysicsComponent::PhysicsComponent(BodyType bodyType)
	: bodyType(bodyType)
	, velocity(Vector3f::Zero)
	, collisionMesh(nullptr)
{

}

PhysicsComponent::PhysicsComponent(BodyType bodyType, const std::string& collisionMeshName)
	: bodyType(bodyType)
	, velocity(Vector3f::Zero)
{
	collisionMesh = g_meshManager.getMesh(collisionMeshName);

}

PhysicsComponent::PhysicsComponent(BodyType bodyType, Vector3f velocity)
	: bodyType(bodyType)
	, velocity(velocity)
	, collisionMesh(nullptr)
{
}

PhysicsComponent::PhysicsComponent(BodyType type, Vector3f velocity, const std::string& collisionMeshName)
	: bodyType(bodyType)
	, velocity(velocity)
{
	collisionMesh = g_meshManager.getMesh(collisionMeshName);
}

PhysicsComponent::~PhysicsComponent()
{
}

}