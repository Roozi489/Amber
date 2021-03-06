#pragma once
#include "NonCopyable.h"
#include "Managers/EntityManager.h"
#include "Systems/BaseSystem.h"
#include "Graphics/Color.h"
#include "Math/Vector.h"
#include "Math/AABB.h"

#include <unordered_map>
#include <memory>

namespace Amber
{

class World : public NonCopyable
{
public:
	EntityManager entityManager;

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
	AABB computeAABB();

	/*std::unordered_map<ComponentID, Component*> components;
	template <typename C>
	void registerComponent()
	{
	C* c = new C[MaxEntities];

	for (int m_i = 0; m_i < MaxEntities; ++m_i)
	c[m_i].id = C::getComponentID<C>();

	components.insert(std::make_pair(C::getComponentID<C>(), c));
	}

	template <typename C>
	void newComponent(C* c)
	{
	components[C::getComponentID<C>()].addNew(c);
	}*/

	template <typename S, typename ... Args>
	void addSystem(Args && ... args)
	{
		std::unique_ptr<S> s(new S(std::forward<Args>(args) ...));
		s->systemID = S::template getSystemID<S>();
		m_systems.insert(std::make_pair(s->systemID, std::move(s)));
	}

	template <typename S>
	S* getSystem()
	{
		return static_cast<S*>(m_systems[S::template getSystemID<S>()].get());
	}

	void initSystems();
	void update(Time delta);

private:
	std::unordered_map<SystemID, std::unique_ptr<BaseSystem>> m_systems;
};

}