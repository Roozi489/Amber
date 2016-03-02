#include "Systems/SoundSystem.h"
#include "Core/String.h"
#include "Core/Utility.h"
#include "Window/Window.h"

#include <fmod/fmod_common.h>
#include <fmod/fmod_errors.h>

namespace Amber
{

SoundSystem::SoundSystem()
{
}

SoundSystem::~SoundSystem()
{
}

void SoundSystem::init()
{
	if (FMOD::System_Create(&m_fmodSystem) != FMOD_OK)
	{
		criticalError("FMOD: Failed to create system");
		return;
	}

	int driverCount = 0;
	m_fmodSystem->getNumDrivers(&driverCount);

	if (driverCount == 0)
	{
		criticalError("FMOD: driver count = 0");
		return;
	}

	// Initialize our Instance with 36 Channels
	m_fmodSystem->init(36, FMOD_INIT_NORMAL, 0);

	// TODO: automate this
	createSound("Bleep.wav");
	createSound("Blop.wav");
	createSound("Bonk.wav");
	createSound("victory.wav");
	createSound("defeat.wav");
}

void SoundSystem::destroy()
{
	for (auto& pair : m_sounds)
		pair.second->release();

	m_fmodSystem->close();
	m_fmodSystem->release();
}

void SoundSystem::update(Time delta)
{
	BaseSystem::update(delta);

	FMOD_RESULT result = m_fmodSystem->update();
	if (result != FMOD_OK)
	{
		std::string message = "FMOD::update error - ";
		message += FMOD_ErrorString(result);
		log(message);
	}
}

void SoundSystem::createSound(const std::string& filename)
{
	FMOD::Sound* sound;
	std::string path = "Sounds/" + filename;
	FMOD_RESULT result = m_fmodSystem->createSound(path.c_str(), FMOD_DEFAULT, 0, &sound);
	if (result != FMOD_OK)
		criticalError("Error creating sound " + filename + "\nReason: " + FMOD_ErrorString(result));

	std::string soundName = split(toLower(filename), '.')[0];
	m_sounds.insert(std::make_pair(soundName, sound));
}

void SoundSystem::playSound(const std::string& name, bool loop, bool playEvenIfAlreadyPlaying)
{
	std::string nameLower = toLower(name);
	assert_amber(m_sounds.find(nameLower) != m_sounds.end(), "sound:" + name + " not found. Probably just wasn't created beforehand.");
	FMOD::Sound* sound = m_sounds[nameLower];
	if (!loop)
	{
		sound->setMode(FMOD_LOOP_OFF);
	}
	else
	{
		sound->setMode(FMOD_LOOP_NORMAL);
		sound->setLoopCount(-1);
	}

	FMOD::Sound* currentSound;
	m_channel->getCurrentSound(&currentSound);

	// Check whether to play
	if (playEvenIfAlreadyPlaying || currentSound != sound)
		m_fmodSystem->playSound(sound, 0, false, &m_channel);
}

}