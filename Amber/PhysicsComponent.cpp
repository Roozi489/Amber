#include "PhysicsComponent.h"
#include "Globals.h"


PhysicsComponent::PhysicsComponent(BodyType bodyType)
    : bodyType(bodyType)
    , velocity(Vector3f::zero())
    , collisionMesh(nullptr)
{

}

PhysicsComponent::PhysicsComponent(BodyType bodyType, const std::string& collisionMeshName)
    : bodyType(bodyType)
    , velocity(Vector3f::zero())
{
    collisionMesh = gMeshManager.getMesh(collisionMeshName);

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
    collisionMesh = gMeshManager.getMesh(collisionMeshName);
}

PhysicsComponent::~PhysicsComponent()
{
}
