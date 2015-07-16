#pragma once
#include "Camera.h"

#include <SDL/SDL.h>

class Amber
{
public:
	Amber();
	~Amber();

	void run();
private:

	float mTotalSecondsElapsed;

	void init();
	void update(float delta);
};

