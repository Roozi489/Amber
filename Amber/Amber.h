#pragma once
#include "Time.h"

class Amber
{
public:
	Amber();
	~Amber();

	void run();
private:
	// TODO: move this to input or window
	int m_lastMouseX;
	int m_lastMouseY;
	// TODO: decide whether to move this to globals
	Time m_totalTimeElapsed;

	void init();
	void update(Time delta);
};

