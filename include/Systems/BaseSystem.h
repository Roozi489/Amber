#pragma once
#include "Core/NonCopyable.h"
#include "Core/Time.h"

namespace Amber
{

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

	virtual void init() = 0;
	virtual void destroy();
	virtual void update(Time delta);

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

}