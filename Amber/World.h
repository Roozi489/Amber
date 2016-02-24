#pragma once
#include "NonCopyable.h"
#include "EntityManager.h"
#include "BaseSystem.h"
#include "Color.h"
#include "Vector.h"

#include <unordered_map>
#include <memory>

class World : public NonCopyable
{
public:
    EntityManager entityManager;

    std::unordered_map<SystemID, std::shared_ptr<BaseSystem>> systems;

    World();
    ~World();

    void init();
	void destroy();

    Entity* addEntity();

    Entity* addFloor();
    Entity* addBall(Vector3f position);
    Entity* addPad(Vector3f position);
    Entity* addBrick(Vector3f position, Color color);

    void setupLevel();

    /*std::unordered_map<ComponentID, Component*> components;
    template <typename C>
    void registerComponent()
    {
    C* c = new C[MAX_ENTITIES];

    for (int i = 0; i < MAX_ENTITIES; ++i)
    c[i].id = C::getComponentID<C>();

    components.insert(std::make_pair(C::getComponentID<C>(), c));
    }

    template <typename C>
    void newComponent(C* c)
    {
    components[C::getComponentID<C>()].addNew(c);
    }*/

    template <typename S, typename ... Args>
    std::shared_ptr<S> addSystem(Args && ... args)
    {
        std::shared_ptr<S> s(new S(std::forward<Args>(args) ...));
        s->systemID = S::template getSystemID<S>();
        systems.insert(std::make_pair(s->systemID, std::shared_ptr<BaseSystem>(s)));
        return s;
    }

    template <typename S>
    S* getSystem()
    {
        return static_cast<S*>(systems[S::template getSystemID<S>()].get());
    }

    void initSystems();
    void update(Time delta);
};
