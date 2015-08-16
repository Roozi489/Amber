#include "Amber.h"
#include "Globals.h"
#include "Vector.h"
#include "RenderSystem.h"
#include "GameplaySystem.h"
#include "SoundSystem.h"
#include "ContextSettings.h"
#include "Time.h"

#include <SDL/SDL.h>
#include <SDL/SDL_keyboard.h>
#include <GL/glew.h>

#include <chrono>
#include <thread>


Amber::Amber()
{
}


Amber::~Amber()
{
    
}

void Amber::run()
{
	Time delta = milliseconds(16.f);

	clearLog();
    init();

    while (true)
    {
		Time before = Time::now();

        update(delta.asSeconds());

		Time duration = Time::now() - before;

        gFrameTime = duration.asMilliseconds();

		float sleepFor = max(0.f, delta.asMilliseconds() - duration.asMilliseconds());
		Time::sleep(milliseconds(sleepFor));
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

    log("Creating window...");
    gMainWindow = SDL_CreateWindow("Amber", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, gWindowWidth, gWindowHeight, SDL_WINDOW_OPENGL);

    if (!gMainWindow)
        criticalError(SDL_GetError());

	log("Creating context...");
    gContext = SDL_GL_CreateContext(gMainWindow);
    if (!gContext)
        criticalError(SDL_GetError());

    SDL_GL_MakeCurrent(gMainWindow, gContext);

    int glVersionMajor;
    int glVersioMinor;
    int result = SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &glVersionMajor);
    result = SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &glVersioMinor);

    checkGlError();

    log("GL Context version: " + std::to_string(glVersionMajor) + "." + std::to_string(glVersioMinor));

    log("Initializing GLEW...");
    glewExperimental = true;
    if (glewInit() != GLEW_OK)
        criticalError("Failed to initialize GLEW");

    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.f, 0.f, 0.f, 1.f);

	ignoreGLError();

	gKeystate = SDL_GetKeyboardState(NULL);

	int mouseX, mouseY;
	SDL_GetMouseState(&mouseX, &mouseY);
	lastMouseX = mouseX;
	lastMouseY = mouseY;

    //
    // Gameplay stuff
    gWorld.init();

    gWorld.addSystem<RenderSystem>();
    gWorld.addSystem<GameplaySystem>();
    gWorld.addSystem<SoundSystem>();
    gWorld.configureSystems();

    gWorld.setupLevel();

    gCamera.init();
    gCamera.setPosition(Vector3f(0.f, 100.f, 120.f));
	gCamera.offsetOrientation(0.f, -0.7f);
}

void Amber::update(float delta)
{
    mTotalSecondsElapsed += static_cast<float>(delta);

    gCamera.update(delta);

    SDL_PumpEvents();
	int mouseX, mouseY;
	Uint32 mouseState = SDL_GetMouseState(&mouseX, &mouseY);

	if (gKeystate[SDL_SCANCODE_W])
	{
		gCamera.moveForward();
	}
	if (gKeystate[SDL_SCANCODE_S])
	{
		gCamera.moveBackward();
	}
	if (gKeystate[SDL_SCANCODE_A])
	{
		gCamera.moveLeft();
	}
	if (gKeystate[SDL_SCANCODE_D])
	{
		gCamera.moveRight();
	}
	if (gKeystate[SDL_SCANCODE_F])
	{
		gCamera.rotateUp();
	}
	if (gKeystate[SDL_SCANCODE_V])
	{
		gCamera.rotateDown();
	}
	if (gKeystate[SDL_SCANCODE_X])
	{
		gCamera.rotateLeft();
	}
	if (gKeystate[SDL_SCANCODE_C])
	{
		gCamera.rotateRight();
	}
    if (gKeystate[SDL_SCANCODE_I])
    {
        gCamera.moveUp();
    }
    if (gKeystate[SDL_SCANCODE_K])
    {
        gCamera.moveDown();
    }

	if (mouseState & SDL_BUTTON_RMASK)
	{	
		SDL_ShowCursor(SDL_FALSE);

		float xOffset = static_cast<float>(lastMouseX - mouseX);
		float yOffset = static_cast<float>(lastMouseY - mouseY);
		gCamera.offsetOrientation(xOffset / 500.f, yOffset / 500.f);

		SDL_WarpMouseInWindow(gMainWindow, gWindowWidth / 2, gWindowHeight / 2);
		mouseX = gWindowWidth / 2;
		mouseY = gWindowHeight / 2;
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

    gWorld.update(delta);

	lastMouseX = mouseX;
	lastMouseY = mouseY;
}
