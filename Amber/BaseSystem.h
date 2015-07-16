#pragma once
#include "NonCopyable.h"

#include <cstdint>

using SystemID = std::int32_t;

class BaseSystem : public NonCopyable
{
public:
    SystemID systemID;
    
    BaseSystem();
    virtual ~BaseSystem();    

    virtual void configure() = 0;
    virtual void update(float delta) = 0;

    template <class S>
    inline static SystemID getSystemID()
    {
        static SystemID id = Impl::getUniqueSystemID();
        return id;
    }
};

namespace Impl
{
    inline SystemID getUniqueSystemID()
    {
        static SystemID lastID = 0;
        return lastID++;
    }
}
