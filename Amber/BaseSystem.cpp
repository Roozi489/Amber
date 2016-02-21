#include "BaseSystem.h"

BaseSystem::~BaseSystem()
{
}

void BaseSystem::update(Time delta)
{
	lastUpdateTime = Time::now();
}

void BaseSystem::destroy()
{
}

void BaseSystem::setUpdateFrequency(UpdateFrequency frequency, Time frequencyTime)
{
	updateFrequency = frequency;
	updateFrequencyTime = frequencyTime;
}
