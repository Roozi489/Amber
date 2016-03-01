#pragma once
#include "Types.h"
#include "Window.h"

#include <SDL/SDL.h>

class Input
{
public:
	Input();
	~Input();
	static void init();
	static void update();

	static bool isKeyDown(SDL_Scancode code);
	
	static int mouseX();
	static int mouseY();
	static int mouseRelativeChangeX();
	static int mouseRelativeChangeY();
	static bool MouseLeftDown();
	static bool MouseRightDown();
	static bool MouseMiddleDown();

private:
	static const uint8* m_keyboardState;
	static uint32 m_mouseState;

	static int m_mouseX;
	static int m_mouseY;

	static int m_lastMouseX;
	static int m_lastMouseY;

	static int m_mouseRelativeChangeX;
	static int m_mouseRelativeChangeY;

	friend void Window::setCursorPosition(int x, int y);
};

