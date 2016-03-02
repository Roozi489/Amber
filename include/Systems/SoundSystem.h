#pragma once
#include "BaseSystem.h"

#include <fmod/fmod.hpp>

#include <unordered_map>

/*
FMOD Studio, copyright © Firelight Technologies Pty, Ltd., 2012-2015.
Audio engine : FMOD Studio by Firelight Technologies
FMOD Studio by Firelight Technologies
*/

namespace Amber
{

class SoundSystem : public BaseSystem
{
public:
	SoundSystem();
	~SoundSystem();

	virtual void init() override;
	virtual void destroy() override;
	virtual void update(Time delta) override;

	void createSound(const std::string& filename);
	void playSound(const std::string& name, bool loop = false, bool playEvenIfAlreadyPlaying = false);

private:
	FMOD::System* m_fmodSystem;
	FMOD::Channel* m_channel = nullptr;

	std::unordered_map<std::string, FMOD::Sound*> m_sounds;
};

}