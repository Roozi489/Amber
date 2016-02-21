#pragma once
#include "NonCopyable.h"
#include "Time.h"

using SystemID = uint32_t;

enum class UpdateFrequency
{
	Always,
	FixedTimeStep
};

class BaseSystem : public NonCopyable
{
public:
    SystemID systemID;

	UpdateFrequency updateFrequency = UpdateFrequency::Always;
	Time updateFrequencyTime = Time::Zero;
	Time lastUpdateTime = Time::Zero;

	BaseSystem() = default;
    virtual ~BaseSystem();    

    virtual void configure() = 0;
    virtual void update(Time delta);
	virtual void destroy();

	void setUpdateFrequency(UpdateFrequency frequency, Time frequencyTime = Time::Zero);

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
