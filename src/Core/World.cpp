#include "Core/World.h"
#include "Math/Math.h"
#include "Components/TransformComponent.h"
#include "Components/RenderComponent.h"
#include "Components/PhysicsComponent.h"

namespace Amber
{

World::World()
{
}

World::~World()
{
}

void World::init()
{
	entityManager.init();
}

void World::destroy()
{
	for (auto& pair : m_systems)
		pair.second->destroy();
}

Entity* World::addEntity()
{
	return entityManager.addEntity();
}

Entity* World::addFloor()
{
	Entity* floor = addEntity();
	floor->setTag(Tag::Floor);
	floor->addComponent<TransformComponent>(Vector3f::Zero, Vector3f(1.f));
	floor->addComponent<PhysicsComponent>(BodyType::Static);
	RenderComponent* renderComp = floor->addComponent<RenderComponent>("floor.obj");
	renderComp->mesh->setTexture("marble2.jpg");
	//renderComp->material.color = Color::fromNormalizedRGB(0.5f, 0.5f, 0.5f);

	return floor;
}

Entity* World::addBall(Vector3f position)
{
	Entity* ball = addEntity();
	ball->setTag(Tag::Ball);
	ball->addComponent<TransformComponent>(position, Vector3f(1.f));
	ball->addComponent<PhysicsComponent>(BodyType::Kinematic, Vector3f::Zero);
	ball->addComponent<RenderComponent>("sphere_lowPoly_smooth.obj");
	ball->getComponent<RenderComponent>().material.color = Color::Cyan;

	return ball;
}

Entity* World::addPad(Vector3f position)
{
	Entity* pad = addEntity();
	pad->setTag(Tag::Pad);
	pad->addComponent<TransformComponent>(position, Vector3f(1.f));
	pad->addComponent<PhysicsComponent>(BodyType::Kinematic, "pad_smooth.obj");
	RenderComponent* renderComp = pad->addComponent<RenderComponent>("pad.obj");
	pad->getComponent<RenderComponent>().material.color = Color::Green;

	return pad;
}

Entity* World::addBrick(Vector3f position, Color color)
{
	Entity* brick = addEntity();
	brick->setTag(Tag::Brick);
	brick->addComponent<TransformComponent>(position, Vector3f(1.f));
	brick->addComponent<PhysicsComponent>(BodyType::Kinematic, "brick_smooth.obj");
	brick->addComponent<RenderComponent>("brick.obj");
	brick->getComponent<RenderComponent>().material.color = color;

	return brick;
}

void World::setupLevel()
{
	/*
	Entity* sponza = addEntity();
	sponza->addComponent<TransformComponent>(Vector3f::Zero, Vector3f(1.f));
	sponza->addComponent<RenderComponent>("sponza.obj");
	sponza->getComponent<RenderComponent>().material.color = Color::White;*/

	addFloor();
	addBall(Vector3f(0.f, 1.f, 23.1f));

	for (int row = 0; row < 3; ++row)
	{
		for (int i = 0; i < 12; ++i)
		{
			Color color;
			if ((((row % 2) + i) % 2) == 0)
				color = Color::Yellow;
			else
				color = Color::Red;
			Entity* e = addBrick(Vector3f(sinf(toRadians(30.f * i)) * 5, (float)row * 2.2f, cosf(toRadians(30.f * i)) * 5), color);
			e->getComponent<TransformComponent>().orientation = angleAxis(toRadians(30.f * i - 90.f), Vector3f::Up);
		}
	}

	for (int i = 0; i < 3; ++i)
	{
		Entity* e = addPad(Vector3f(sinf(toRadians(120.f * i)) * 25, 0.f, cosf(toRadians(120.f * i)) * 25));
		e->getComponent<TransformComponent>().orientation = angleAxis(toRadians(180.f + 120.f * i), Vector3f::Up);
	}
}

AABB World::computeAABB()
{
	Vector3f minValues { 9999.f, 9999.f, 9999.f };
	Vector3f maxValues { -9999.f, -9999.f, -9999.f };

	TransformComponent transformComp;
	RenderComponent renderComp;
	for (const Entity& entity : entityManager.entities_with_components(transformComp, renderComp))
	{
		Matrix4x4f modelMatrix = Math::translate(transformComp.position) * quaternionToMatrix4x4f(transformComp.orientation);
		auto mesh = renderComp.mesh;

		for (size_t i = 0; i < mesh->indices.size(); ++i)
		{
			Vector3f vertex = modelMatrix * mesh->vertices[mesh->indices[i]];
			if (vertex.x < minValues.x)
				minValues.x = vertex.x;
			if (vertex.y < minValues.y)
				minValues.y = vertex.y;
			if (vertex.z < minValues.z)
				minValues.z = vertex.z;
			if (vertex.x > maxValues.x)
				maxValues.x = vertex.x;
			if (vertex.y > maxValues.y)
				maxValues.y = vertex.y;
			if (vertex.z > maxValues.z)
				maxValues.z = vertex.z;
		}
	}
	return AABB::fromMinMax(minValues, maxValues);
}

void World::initSystems()
{
	for (auto& pair : m_systems)
		pair.second->init();
}

void World::update(Time delta)
{
	for (auto& pair : m_systems)
	{
		BaseSystem* system = pair.second.get();

		// Update if updateFrequency is set to always or when it's the first update
		if (system->updateFrequency == UpdateFrequency::Always || system->lastUpdateTime == Time::Zero)
		{
			system->update(delta);
		}
		// or when enough time passed
		else
		{
			Time lag = Time::now() - system->lastUpdateTime;
			while (lag >= system->updateFrequencyTime)
			{
				lag -= system->updateFrequencyTime;
				system->update(system->updateFrequencyTime);
			}
		}
	}
}

}