#pragma once

#include <SDL/SDL.h>
#include <string>

class Window
{
public:
	Window() = default;
	~Window();

	void create();
	void destroy();

	void update();
	void display();
	
	int getWidth() const;
	int getHeight() const;

	void setIcon(const std::string& path);
	SDL_Window* getSdlWindow() const;
	SDL_GLContext getContext() const;

	std::string getTitle() const;
	void setTitle(const std::string& title);

	void showCursor(bool value);
	void setCursorPosition(int x, int y);

private:
	int m_width = 1024;
	int m_height = 768;

	SDL_Window *m_sdlWindow;
	SDL_GLContext m_context;

	int m_actualGlVersionMajor;
	int m_actualGlVersionMinor;

	std::string m_title = "Amber";
};

void messageBox(const char* message);
void messageBox(const std::string& message);
void messageBox(const char* title, const char* message);
void messageBox(const std::string& title, const std::string& message);
void quit();

void _criticalError(const char* message, const char* file, int line, const char* function);
void _criticalError(const std::string& message, const char* file, int line, const char* function);
#define criticalError(msg) _criticalError(msg, __FILE__,__LINE__,__FUNCTION__)

static_assert(SDL_FALSE == false, "SDL_FALSE is not the same as false.");
static_assert(SDL_TRUE == true, "SDL_TRUE is not the same as true.");