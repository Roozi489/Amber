#pragma once
#include <cstdint>

class Entity;

using ComponentID = std::uint32_t;

namespace Impl
{
	inline ComponentID getUniqueComponentID()
	{
		static ComponentID lastID = 0;
		return lastID++;
	}
}

class BaseComponent
{
public:
    ComponentID id;
    Entity* parent;

    BaseComponent();
    virtual ~BaseComponent();

    template <class C>
    inline static ComponentID getComponentTypeID()
    {
        static ComponentID typeID = Impl::getUniqueComponentID();
        return typeID;
    }
};
