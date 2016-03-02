#include "Systems/BaseSystem.h"

namespace Amber
{

BaseSystem::~BaseSystem()
{
}

void BaseSystem::destroy()
{
}

void BaseSystem::update(Time delta)
{
	lastUpdateTime = Time::now();
}

void BaseSystem::setUpdateFrequency(UpdateFrequency frequency, Time frequencyTime)
{
	updateFrequency = frequency;
	updateFrequencyTime = frequencyTime;
}

}