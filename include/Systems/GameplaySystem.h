#pragma once
#include "BaseSystem.h"
#include "Math/Vector.h"
#include "Math/Sphere.h"
#include "Core/Utility.h"
#include "Math/Triangle.h"
#include "Components/TransformComponent.h"

//
#include "SoundSystem.h"
//

namespace Amber
{

class Mesh;

enum class GameState
{
	Start,
	Playing,
	Victory,
	Defeat
};

class GameplaySystem : public BaseSystem
{
public:
	int score = 0;
	int lives = 5;
	GameState gameState = GameState::Start;

	SoundSystem* soundSystem;

	float startingSpeed = 10.f;
	float speedIncrease = 1.05f;

	GameplaySystem();
	~GameplaySystem();

	virtual void init() override;
	virtual void update(Time delta) override;

	float distFromCenter(Vector3f position);
	float angleFromCenter(Vector3f position);

	CollisionResult sphereMeshCollisionFast(const Sphere& sphere, TransformComponent& transformComp, Mesh* mesh);
	CollisionResult movingSphereMeshCollision(const Sphere& sphere, const Vector3f& sphereVelocity, TransformComponent& transformComp, Mesh* mesh);
	CollisionResult movingSphereTriangleCollision(const Sphere& sphere, const Vector3f& sphereVelocity, const Triangle& triangle);
};

}