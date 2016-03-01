#include "Input.h"

#include <SDL/SDL.h>
#include "Globals.h"

const uint8* Input::m_keyboardState;
uint32 Input::m_mouseState;

int Input::m_mouseX;
int Input::m_mouseY;

int Input::m_lastMouseX;
int Input::m_lastMouseY;

int Input::m_mouseRelativeChangeX;
int Input::m_mouseRelativeChangeY;

Input::Input()
{
}


Input::~Input()
{
}

void Input::init()
{
	SDL_PumpEvents();
	m_keyboardState = SDL_GetKeyboardState(nullptr);

	SDL_GetMouseState(&m_lastMouseX, &m_lastMouseY);
}

void Input::update()
{
	SDL_PumpEvents();
	m_keyboardState = SDL_GetKeyboardState(nullptr);

	m_mouseState = SDL_GetMouseState(&m_mouseX, &m_mouseY);

	m_mouseRelativeChangeX = m_lastMouseX - m_mouseX;
	m_mouseRelativeChangeY = m_lastMouseY - m_mouseY;

	m_lastMouseX = m_mouseX;
	m_lastMouseY = m_mouseY;
}

int Input::mouseX()
{
	return m_mouseX;
}

int Input::mouseY()
{
	return m_mouseY;
}

int Input::mouseRelativeChangeX()
{
	return m_mouseRelativeChangeX;
}

int Input::mouseRelativeChangeY()
{
	return m_mouseRelativeChangeY;
}

bool Input::isKeyDown(SDL_Scancode code)
{
	return m_keyboardState[code] != 0;
}

bool Input::MouseLeftDown()
{
	return (m_mouseState & SDL_BUTTON_LMASK) != 0;
}

bool Input::MouseRightDown()
{
	return (m_mouseState & SDL_BUTTON_RMASK) != 0;
}

bool Input::MouseMiddleDown()
{
	return (m_mouseState & SDL_BUTTON_MMASK) != 0;
}
