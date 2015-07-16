#include "SoundSystem.h"
#include "Utility.h"

#include <fmod/fmod_errors.h>
#include <fmod/fmod_common.h>

#include <string>
#include <assert.h>

SoundSystem::SoundSystem()
{
}


SoundSystem::~SoundSystem()
{
    for (auto& pair : sounds)
        pair.second->release();

    fmodSystem->close();
    fmodSystem->release();
}

void SoundSystem::configure()
{
    if (FMOD::System_Create(&fmodSystem) != FMOD_OK)
    {
        criticalError("FMOD: Failed to create system");
        return;
    }

    int driverCount = 0;
    fmodSystem->getNumDrivers(&driverCount);

    if (driverCount == 0)
    {
        criticalError("FMOD: driver count = 0");
        return;
    }

    // Initialize our Instance with 36 Channels
    fmodSystem->init(36, FMOD_INIT_NORMAL, 0);

    // TODO: automate this
    createSound("Bleep.wav");
    createSound("Blop.wav");
    createSound("Bonk.wav");
    createSound("victory.wav");
    createSound("defeat.wav");
}

void SoundSystem::update(float delta)
{
    FMOD_RESULT result = fmodSystem->update();
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
    FMOD_RESULT result = fmodSystem->createSound(path.c_str(), FMOD_DEFAULT, 0, &sound);
    if (result != FMOD_OK)
        criticalError("Error creating sound " + filename +"\nReason: " + FMOD_ErrorString(result));

    std::string soundName = split(toLower(filename), '.')[0];
    sounds.insert(std::make_pair(soundName, sound));
}

void SoundSystem::playSound(const std::string& name, bool loop, bool playEvenIfAlreadyPlaying)
{
    std::string nameLower = toLower(name);
    assert(sounds.find(nameLower) != sounds.end());
    FMOD::Sound* sound = sounds[nameLower];
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
    channel->getCurrentSound(&currentSound);

    // Check whether to play
    if (playEvenIfAlreadyPlaying || currentSound != sound)
        fmodSystem->playSound(sound, 0, false, &channel);
}
