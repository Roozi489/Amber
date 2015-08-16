#pragma once

class Amber
{
public:
	Amber();
	~Amber();

	void run();
private:
	int lastMouseX;
	int lastMouseY;


	float mTotalSecondsElapsed;

	void init();
	void update(float delta);
};

