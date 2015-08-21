#pragma once
#include "Time.h"

class Amber
{
public:
	Amber();
	~Amber();

	void run();
private:
	int mLastMouseX;
	int mLastMouseY;

	Time mTotalTimeElapsed;

	void init();
	void update(Time delta);
};

