#pragma once
#include "NonCopyable.h"

#include <cstdint>

class Entity;

using ComponentID = std::int32_t;

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

namespace Impl
{
    inline ComponentID getUniqueComponentID()
    {
        static ComponentID lastID = 0;
        return lastID++;
    }
}

