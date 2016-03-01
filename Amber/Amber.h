#pragma once
#include "Time.h"

class Amber
{
public:
	Amber();
	~Amber();

	void run();
private:
	void init();
	void update(Time delta);
};

