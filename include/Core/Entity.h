#pragma once
#include "Components/BaseComponent.h"

#include <bitset>
#include <array>
//
#include <vector>
#include <memory>
//

namespace Amber
{

// TODO: change to string ID
enum class Tag
{
	Floor,
	Ball,
	Brick,
	Pad
};

static constexpr int MAX_COMPONENTS = 16;

using EntityID = std::uint32_t;
using ComponentMask = std::bitset<MAX_COMPONENTS>;

class Entity
{
public:
	EntityID id;
	Tag tag;

	// TODO: change how components are stored
	std::vector<std::shared_ptr<BaseComponent>> components;
	//

	ComponentMask componentMask;
	std::array<BaseComponent*, MAX_COMPONENTS> componentArray;

	Entity();
	~Entity();

	void setTag(Tag t);

	template <typename C, typename ... Args>
	C* addComponent(Args && ... args)
	{
		C* component = new C(std::forward<Args>(args)...);
		component->parent = this;
		ComponentID id = C::template getComponentTypeID<C>();
		component->id = id;

		// TODO: change this
		components.push_back(std::shared_ptr<BaseComponent>(component));
		//

		componentArray[id] = component;
		componentMask[id] = true;

		return component;
	}

	template <class C>
	bool hasComponent() const
	{
		return componentMask[C::template getComponentTypeID<C>()];
	}

	template <class C>
	C& getComponent()
	{
		auto ptr = componentArray[C::template getComponentTypeID<C>()];
		return *reinterpret_cast<C*>(ptr);
	}

	bool operator==(Entity const& rhs);
	bool operator!=(Entity const& rhs);
};

}