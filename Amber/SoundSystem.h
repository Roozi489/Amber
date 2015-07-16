#pragma once
#include "BaseSystem.h"

#include <fmod/fmod.hpp>

#include <unordered_map>
#include <string>

/*
FMOD Studio, copyright © Firelight Technologies Pty, Ltd., 2012-2015.
Audio engine : FMOD Studio by Firelight Technologies
FMOD Studio by Firelight Technologies
*/

class SoundSystem : public BaseSystem
{
public:
    FMOD::System* fmodSystem;
    FMOD::Channel* channel = 0;

    std::unordered_map<std::string, FMOD::Sound*> sounds;

    SoundSystem();
    ~SoundSystem();

    virtual void configure();
    virtual void update(float delta);

    void createSound(const std::string& filename);
    void playSound(const std::string& name, bool loop = false, bool playEvenIfAlreadyPlaying = false);
};

