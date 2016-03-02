#pragma once
#include "Core/Types.h"

namespace Amber
{

class Entity;

using ComponentID = uint32;


class BaseComponent
{
public:
	ComponentID id;
	Entity* parent;

	BaseComponent() = default;
	virtual ~BaseComponent() = default;

	template <class C>
	inline static ComponentID getComponentTypeID()
	{
		static ComponentID typeID = Impl::getUniqueComponentID();
		return typeID;
	}
};

namespace Impl
{
inline ComponentID getUniqueComponentID()
{
	static ComponentID lastID = 0;
	return lastID++;
}
}

}