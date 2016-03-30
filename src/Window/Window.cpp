#include "Window/Window.h"
#include "Window/ContextSettings.h"
#include "Window/Input.h"
#include "Core/Log.h"
#include "Core/String.h"
#include "Core/Globals.h"
#include "Graphics/OpenGL.h"
#include "Graphics/Image.h"
#include "Systems/RenderSystem.h"

namespace Amber
{

Window::~Window()
{
}

void Window::create()
{
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);

	ContextSettings settings;
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, settings.majorVersion);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, settings.minorVersion);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, settings.depthBits);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, settings.stencilBits);
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, settings.acceleratedVisual);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, static_cast<int>(settings.profileMask));
	// TODO: add double buffering setting (its on by default)

	Log::message("Creating window.");
	m_sdlWindow = SDL_CreateWindow(m_title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, m_width, m_height, SDL_WINDOW_OPENGL);
	if (!m_sdlWindow)
		criticalError(SDL_GetError());

	setIcon("Textures/amber_icon.png");

	Log::message("Creating context.");
	m_context = SDL_GL_CreateContext(m_sdlWindow);
	if (!m_context)
		criticalError(SDL_GetError());

	SDL_GL_SetSwapInterval(settings.vsyncEnabled);
	SDL_GL_MakeCurrent(m_sdlWindow, m_context);

	SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &m_actualGlVersionMajor);
	SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &m_actualGlVersionMinor);

	Log::message(stringFormat("GL Context version: %d.%d", m_actualGlVersionMajor, m_actualGlVersionMinor));

	Log::message("Initializing GLEW.");
	glewExperimental = true;
	if (glewInit() != GLEW_OK)
		criticalError("Failed to initialize GLEW");

	ignoreGlError();
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
		default:
			break;
		}
	}
}

void Window::display()
{
	static RenderSystem* renderSystem = g_world.getSystem<RenderSystem>();
	
	glViewport(0, 0, m_width, m_height);
	glClear(GL_COLOR_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);

	ShaderProgram* program = g_shaderManager.getShaderProgram("texPass");
	program->use();
	renderSystem->getOutColorTexture().activeAndBind(0);
	program->setUniform("tex", 0);
	renderSystem->drawFullscreenQuad();
	program->stopUsing();

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

SDL_GLContext Window::getContext() const
{
	return m_context;
}

std::string Window::getTitle() const
{
	return m_title;
}

void Window::setTitle(const std::string& title)
{
	m_title = title;
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
	SDL_GL_DeleteContext(g_window.getContext());
	SDL_DestroyWindow(g_window.getSdlWindow());
	SDL_Quit();
	exit(0);
}

void _criticalError(const char* message, const char* file, int line, const char* function)
{
	std::stringstream sstream;
	sstream << message << std::endl << file << ":" << line << "  -  " << function;
	messageBox("Critical error", sstream.str());
	std::exit(1);
}

void _criticalError(const std::string& message, const char* file, int line, const char* function)
{
	_criticalError(message.c_str(), file, line, function);
}

}