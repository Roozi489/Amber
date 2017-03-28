#include "Window/Window.h"
#include "Window/ContextSettings.h"
#include "Window/Input.h"
#include "Core/Log.h"
#include "Core/String.h"
#include "Core/Globals.h"
#include "Graphics/Graphics.h"
#include "Graphics/GL/OpenGL.h"
#include "Graphics/Image.h"
#include "Systems/RenderSystem.h"
#include "Core/Utility.h"
#include "Window/File.h"

#include <unordered_map>

#include <SDL/SDL_syswm.h>
#include <vulkan/vulkan.hpp>

namespace Amber
{

Window::~Window()
{
}

void Window::create(GraphicsApi graphicsApi, std::string title)
{
	switch (graphicsApi)
	{
	case GraphicsApi::OpenGL:
		m_title = title + " - OpenGL";
		createGLWindow();
		break;
	case GraphicsApi::Vulkan:
		m_title = title + " - Vulkan";
		createVulkanWindow();
		break;
	}
}	

void Window::createGLWindow()
{
	Log::info("Using OpenGL");

	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);

	ContextSettings settings;
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, settings.majorVersion);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, settings.minorVersion);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, settings.depthBits);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, settings.stencilBits);
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, settings.acceleratedVisual);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, toUnderlyingType(settings.profileMask));
	// TODO: add double buffering setting (its on by default)

	Log::info("Creating window.");
	// TODO: enable window size change
	m_sdlWindow = SDL_CreateWindow(m_title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, m_width, m_height, SDL_WINDOW_OPENGL);
	if (!m_sdlWindow)
		criticalError(SDL_GetError());

	setIcon("Textures/amber_icon.png");

	Log::info("Creating context.");
	m_context = SDL_GL_CreateContext(m_sdlWindow);
	if (!m_context)
		criticalError(SDL_GetError());

	SDL_GL_SetSwapInterval(settings.vsyncEnabled);
	SDL_GL_MakeCurrent(m_sdlWindow, m_context);

	SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &m_actualGlVersionMajor);
	SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &m_actualGlVersionMinor);

	Log::info(stringFormat("GL Context version: %d.%d", m_actualGlVersionMajor, m_actualGlVersionMinor));

	Log::info("Initializing GLEW.");
	glewExperimental = true;
	if (glewInit() != GLEW_OK)
		criticalError("Failed to initialize GLEW");

	ignoreGlError();
}

void Window::createVulkanWindow()
{
	Log::info("Using Vulkan");

	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);

	Log::info("Creating window.");
	m_sdlWindow = SDL_CreateWindow(m_title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, m_width, m_height, SDL_WINDOW_RESIZABLE);
	if (!m_sdlWindow)
		criticalError(SDL_GetError());

	SDL_SysWMinfo info;
	SDL_VERSION(&info.version);

	if (!SDL_GetWindowWMInfo(m_sdlWindow, &info))
		criticalError(stringFormat("Couldn't get window information: %s.", SDL_GetError()));

	const char* subsystem = "an unknown system";
	switch (info.subsystem)
	{
	case SDL_SYSWM_WINDOWS:
		subsystem = "Microsoft Windows";
		break;
	case SDL_SYSWM_UNKNOWN:
		break;
	case SDL_SYSWM_X11:
		subsystem = "X Window System";
		break;
	case SDL_SYSWM_WINRT:
		subsystem = "WinRT";
		break;
	case SDL_SYSWM_DIRECTFB:
		subsystem = "DirectFB";
		break;
	case SDL_SYSWM_COCOA:
		subsystem = "Apple OS X";
		break;
	case SDL_SYSWM_UIKIT:
		subsystem = "UIKit";
		break;
	case SDL_SYSWM_WAYLAND:
		subsystem = "Wayland";
		break;
	case SDL_SYSWM_MIR:
		subsystem = "Mir";
		break;
	case SDL_SYSWM_ANDROID:
		subsystem = "Android";
		break;
	case SDL_SYSWM_VIVANTE:
		subsystem = "Vivante";
		break;
	}

	Log::info(stringFormat("System: %s", subsystem));
	Log::info(stringFormat("SDL Version: %d.%d.%d", info.version.major, info.version.minor, info.version.patch));

	// TODO: support other systems
	if (info.subsystem != SDL_SYSWM_WINDOWS)
		criticalError("Unsupported system. Exititng...");

	setIcon("Textures/amber_icon.png");

	m_hWnd = info.info.win.window;
	m_hInstance = GetModuleHandle(nullptr);
}

void Window::destroy()
{
}

void Window::update()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			quit();
			break;
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym)
			{
			case SDLK_ESCAPE:
				quit();
				break;
			default:
				break;
			}
			break;
		case SDL_KEYUP:
			break;
		case SDL_MOUSEMOTION:
			break;
		case SDL_WINDOWEVENT:
			switch (event.window.event)
			{
			case SDL_WINDOWEVENT_RESIZED:
				Log::info(stringFormat("Window %d resized to %dx%d", event.window.windowID, event.window.data1, event.window.data2));
				handleWindowSizeChange(event.window.data1, event.window.data2);
				break;
			default:
				break;
			}
			break;
		default:
			break;
		}
	}
}

void Window::display()
{
	static RenderSystem* renderSystem = g_world.getSystem<RenderSystem>();

	renderSystem->display();
	
	if (g_graphicsApi == GraphicsApi::OpenGL)
		SDL_GL_SwapWindow(m_sdlWindow);
}

void Window::setIcon(const std::string& path)
{
	SDL_Surface* iconSurface = loadSDL_SurfaceFromFile(path.c_str());
	SDL_SetWindowIcon(m_sdlWindow, iconSurface);
}

SDL_Window* Window::getSdlWindow() const
{
	return m_sdlWindow;
}

HWND Window::getHWnd() const
{
}

HINSTANCE Window::getHInstance() const
{
}

SDL_GLContext Window::getContext() const
{
	return m_context;
}

void Window::showCursor(bool value)
{
	SDL_ShowCursor(value);
}

void Window::setCursorPosition(int x, int y)
{
	SDL_WarpMouseInWindow(g_window.getSdlWindow(), x, y);
	Input::m_mouseX = Input::m_lastMouseX = x;
	Input::m_mouseY = Input::m_lastMouseY = y;
}

void Window::handleWindowSizeChange(int newWidth, int newHeight)
{
	m_width = newWidth;
	m_height = newHeight;

	g_world.getSystem<RenderSystem>()->handleWindowSizeChange(newWidth, newHeight);
}

void messageBox(const char* message)
{
	messageBox("Amber", message);
}

void messageBox(const std::string& message)
{
	messageBox("Amber", message.c_str());
}

void messageBox(const std::string& title, const std::string& message)
{
	messageBox(title.c_str(), message.c_str());
}

void messageBox(const char* title, const char* message)
{
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title, message, g_window.getSdlWindow());
}

void quit()
{
	if (g_graphicsApi == GraphicsApi::OpenGL)
	{
		SDL_GL_DeleteContext(g_window.getContext());
	}

	SDL_DestroyWindow(g_window.getSdlWindow());
	SDL_Quit();
	exit(0);
}

__declspec(noreturn) void _criticalError(const char* message, const char* file, int line, const char* function)
{
	std::stringstream sstream;
	sstream << message << std::endl << file << ":" << line << "  -  " << function;
	Log::error(sstream.str());
	messageBox("Critical error", sstream.str());
	std::exit(1);
}

__declspec(noreturn) void _criticalError(const std::string& message, const char* file, int line, const char* function)
{
	_criticalError(message.c_str(), file, line, function);
}

}
