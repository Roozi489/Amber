#pragma once
#include "Core/Time.h"

namespace Amber
{

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

}