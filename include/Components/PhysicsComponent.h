#pragma once
#include "BaseComponent.h"
#include "Math/Vector.h"
#include "Core/Mesh.h"

namespace Amber
{
enum class BodyType
{
	Static,
	Dynamic,
	Kinematic
};

class PhysicsComponent : public BaseComponent
{
public:
	BodyType bodyType;
	//float mass = 1.f;
	Vector3f velocity;

	Mesh* collisionMesh;

	PhysicsComponent() = default;
	PhysicsComponent(BodyType bodyType);
	PhysicsComponent(BodyType bodyType, const std::string& collisionMeshName);
	PhysicsComponent(BodyType type, Vector3f velocity);
	PhysicsComponent(BodyType type, Vector3f velocity, const std::string& collisionMeshName);
	~PhysicsComponent();
};
}