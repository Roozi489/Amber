#include "World.h"

#include "TransformComponent.h"
#include "RenderComponent.h"
#include "PhysicsComponent.h"

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
	for (auto& pair : systems)
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
    floor->addComponent<TransformComponent>(Vector3f::zero(), Vector3f(1.f));
    floor->addComponent<PhysicsComponent>(BodyType::Static);
    RenderComponent* renderComp = floor->addComponent<RenderComponent>("default", "floor.obj");
    renderComp->mesh->setTexture("marble2.jpg");

    return floor;
}

Entity* World::addBall(Vector3f position)
{
    Entity* ball = addEntity();
    ball->setTag(Tag::Ball);
    ball->addComponent<TransformComponent>(position, Vector3f(1.f));
    ball->addComponent<PhysicsComponent>(BodyType::Kinematic, Vector3f::zero());
    ball->addComponent<RenderComponent>("default", "sphere_lowPoly_smooth.obj");
    ball->getComponent<RenderComponent>().color = Color::Cyan;

    return ball;
}

Entity* World::addPad(Vector3f position)
{
    Entity* pad = addEntity();
    pad->setTag(Tag::Pad);
    pad->addComponent<TransformComponent>(position, Vector3f(1.f));
    pad->addComponent<PhysicsComponent>(BodyType::Kinematic, "pad_smooth.obj");
    RenderComponent* renderComp = pad->addComponent<RenderComponent>("default", "pad.obj");
    Color color = Color::Green;
    color.a = 0.6f;
    pad->getComponent<RenderComponent>().color = color;

    return pad;
}

Entity* World::addBrick(Vector3f position, Color color)
{
    Entity* brick = addEntity();
    brick->setTag(Tag::Brick);
    brick->addComponent<TransformComponent>(position, Vector3f(1.f));
    brick->addComponent<PhysicsComponent>(BodyType::Kinematic, "brick_smooth.obj");
    brick->addComponent<RenderComponent>("default", "brick.obj");
    brick->getComponent<RenderComponent>().color = color;

    return brick;
}

void World::setupLevel()
{
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
            color.a = 0.88f;
            Entity* e = addBrick(Vector3f(sinf(toRadians(30.f * i)) * 5, (float)row * 2.2f, cosf(toRadians(30.f * i)) * 5), color);
			e->getComponent<TransformComponent>().orientation = angleAxis(toRadians(30.f * i - 90.f), Vector3f::Up);
        }
    }

    for (int i = 0; i < 3; ++i)
    {
        Entity* e = addPad(Vector3f(sinf(toRadians(120.f * i)) * 25, 0.5f, cosf(toRadians(120.f * i)) * 25));
		e->getComponent<TransformComponent>().orientation = angleAxis(toRadians(180.f + 120.f * i), Vector3f::Up);
	}
}

void World::configureSystems()
{
    for (auto& pair : systems)
        pair.second->configure();
}

void World::update(Time delta)
{
    for (auto& pair : systems)
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
