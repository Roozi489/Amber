#include "Amber.h"
#include "Globals.h"
#include "Vector.h"
#include "RenderSystem.h"
#include "GameplaySystem.h"
#include "SoundSystem.h"
#include "ContextSettings.h"
#include "OpenGL.h"

#include <SDL/SDL.h>
#include <SDL/SDL_keyboard.h>

Amber::Amber()
{
}

Amber::~Amber()
{
}

void Amber::run()
{
	clearLog();
    init();

	Time previous = Time::now();

	while (true)
	{
		Time current = Time::now();
		Time elapsed = current - previous;
		g_FrameTime = elapsed;

		update(elapsed);

		previous = current;
	}
}

void Amber::init()
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);

    SDL_GL_SetSwapInterval(0);

	ContextSettings settings;
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, settings.majorVersion);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, settings.minorVersion);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, settings.depthBits);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, settings.stencilBits);
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, settings.acceleratedVisual);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, static_cast<int>(settings.profileMask));
	// TODO: add double buffering setting (its on by default)

    log("Creating window...");
    g_MainWindow = SDL_CreateWindow("Amber", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, g_WindowWidth, g_WindowHeight, SDL_WINDOW_OPENGL);
    if (!g_MainWindow)
        criticalError(SDL_GetError());

	SDL_Surface* iconSurface = loadSDL_SurfaceFromFile("Textures/amber_icon.png");
	SDL_SetWindowIcon(g_MainWindow, iconSurface);

	log("Creating context...");
    g_Context = SDL_GL_CreateContext(g_MainWindow);
    if (!g_Context)
        criticalError(SDL_GetError());

    SDL_GL_MakeCurrent(g_MainWindow, g_Context);

    int glVersionMajor;
    int glVersionMinor;
    int result = SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &glVersionMajor);
    result = SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &glVersionMinor);

    checkGlError();

    log("GL Context version: " + std::to_string(glVersionMajor) + "." + std::to_string(glVersionMinor));

    log("Initializing GLEW...");
    glewExperimental = true;
    if (glewInit() != GLEW_OK)
        criticalError("Failed to initialize GLEW");
	
    ignoreGLError();

	g_Keystate = SDL_GetKeyboardState(NULL);

	int mouseX, mouseY;
	SDL_GetMouseState(&mouseX, &mouseY);
	m_lastMouseX = mouseX;
	m_lastMouseY = mouseY;

    //
    // Gameplay stuff
    g_World.init();

    g_World.addSystem<RenderSystem>();
    g_World.addSystem<GameplaySystem>();
    g_World.addSystem<SoundSystem>();
    g_World.initSystems();

    g_World.setupLevel();

    g_Camera.init();
    g_Camera.setPosition(Vector3f(0.f, 100.f, 120.f));
	g_Camera.offsetOrientation(0.f, -0.7f);
}

void Amber::update(Time delta)
{
    m_totalTimeElapsed += delta;

    SDL_PumpEvents();
	int mouseX, mouseY;
	Uint32 mouseState = SDL_GetMouseState(&mouseX, &mouseY);

	if (g_Keystate[SDL_SCANCODE_W])
	{
		g_Camera.moveForward();
	}
	if (g_Keystate[SDL_SCANCODE_S])
	{
		g_Camera.moveBackward();
	}
	if (g_Keystate[SDL_SCANCODE_A])
	{
		g_Camera.moveLeft();
	}
	if (g_Keystate[SDL_SCANCODE_D])
	{
		g_Camera.moveRight();
	}
	if (g_Keystate[SDL_SCANCODE_F])
	{
		g_Camera.rotateUp();
	}
	if (g_Keystate[SDL_SCANCODE_V])
	{
		g_Camera.rotateDown();
	}
	if (g_Keystate[SDL_SCANCODE_X])
	{
		g_Camera.rotateLeft();
	}
	if (g_Keystate[SDL_SCANCODE_C])
	{
		g_Camera.rotateRight();
	}
    if (g_Keystate[SDL_SCANCODE_I])
    {
        g_Camera.moveUp();
    }
    if (g_Keystate[SDL_SCANCODE_K])
    {
        g_Camera.moveDown();
    }

	if (mouseState & SDL_BUTTON_RMASK)
	{	
		SDL_ShowCursor(SDL_FALSE);

		float xOffset = static_cast<float>(m_lastMouseX - mouseX);
		float yOffset = static_cast<float>(m_lastMouseY - mouseY);
		g_Camera.offsetOrientation(xOffset / 500.f, yOffset / 500.f);

		SDL_WarpMouseInWindow(g_MainWindow, g_WindowWidth / 2, g_WindowHeight / 2);
		mouseX = g_WindowWidth / 2;
		mouseY = g_WindowHeight / 2;
	}
	else
	{
		SDL_ShowCursor(SDL_TRUE);
	}

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

	g_Camera.update(delta);
	g_World.update(delta);

	m_lastMouseX = mouseX;
	m_lastMouseY = mouseY;
}
