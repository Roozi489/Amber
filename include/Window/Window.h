#pragma once
#include "Amber.h"
#include "Core/Types.h"
#include "Core/Log.h"
#include "Core/String.h"
#include "Math/Vector.h"
#include "Math/Matrix.h"
#include "Graphics/Graphics.h"

#include <SDL/SDL.h>
#include <SDL/SDL_syswm.h> 

#include <string>

namespace Amber
{

class Window
{
public:
	Window() = default;
	~Window();

	void create(GraphicsApi graphicsApi, std::string title = "Amber");
	void destroy();

	void update();
	void display();

	int getWidth() const { return m_width; };
	int getHeight() const { return m_height; };
	float windowRatio() const { return static_cast<float>(m_height) / m_width; };

	void setIcon(const std::string& path);
	SDL_Window* getSdlWindow() const;
	SDL_GLContext getContext() const;
	HWND getHWnd() const;
	HINSTANCE getHInstance() const;

	void showCursor(bool value);
	void setCursorPosition(int x, int y);

private:
	int m_width = 900;
	int m_height = 800;

	std::string m_title;

	SDL_Window *m_sdlWindow;
	SDL_GLContext m_context;
	HWND m_hWnd;
	HINSTANCE m_hInstance;

	int m_actualGlVersionMajor;
	int m_actualGlVersionMinor;

	void createGLWindow();
	void createVulkanWindow();

	void handleWindowSizeChange(int newWidth, int newHeight);
};

void messageBox(const char* message);
void messageBox(const std::string& message);
void messageBox(const char* title, const char* message);
void messageBox(const std::string& title, const std::string& message);
void quit();

__declspec(noreturn) void _criticalError(const char* message, const char* file, int line, const char* function);
__declspec(noreturn) void _criticalError(const std::string& message, const char* file, int line, const char* function);
#define criticalError(msg) _criticalError(msg, __FILE__,__LINE__,__FUNCTION__)

static_assert(SDL_FALSE == false, "SDL_FALSE is not the same as false.");
static_assert(SDL_TRUE == true, "SDL_TRUE is not the same as true.");

}


