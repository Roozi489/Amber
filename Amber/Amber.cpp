﻿#include "Amber.h"
#include "Globals.h"
#include "Vector.h"
#include "Matrix.h"

#include "RenderSystem.h"
#include "GameplaySystem.h"
#include "SoundSystem.h"

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
    float deltaMillis = 16.f;
    float deltaSeconds = deltaMillis / 1000.f;

	clearLog();
    init();
    
    /*auto lastTime = std::chrono::system_clock::now();;
    while (true)
    {
    auto current = std::chrono::system_clock::now();;
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(current - lastTime);
    update((static_cast<float>(elapsed.count()) / 1000.f));
    log(std::to_string(elapsed.count()));

    lastTime = current;
    }*/

    while (true)
    {
        auto start = std::chrono::system_clock::now();

        update(deltaSeconds);

        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start);
        
        auto sleepFor = max(0, (int)deltaMillis - duration.count());

        gFrameTime = static_cast<float>(duration.count());

        std::this_thread::sleep_for(std::chrono::milliseconds(sleepFor));
    }
}

void Amber::init()
{
    SDL_Init(SDL_INIT_VIDEO);

    SDL_GL_SetSwapInterval(0);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);	

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
    {
        criticalError("Failed to initialize GLEW");
    }
	
	// trap the mouse in the window and hide the cursor
	//SDL_SetRelativeMouseMode(SDL_TRUE);

    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.f, 0.f, 0.f, 1.f);

    checkGlError();

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
		float xOffset = static_cast<float>(lastMouseX - mouseX);
		float yOffset = static_cast<float>(lastMouseY - mouseY);
		gCamera.offsetOrientation(xOffset / 500.f, yOffset / 500.f);

		SDL_WarpMouseInWindow(gMainWindow, gWindowWidth / 2, gWindowHeight / 2);
		mouseX = gWindowWidth / 2;
		mouseY = gWindowHeight / 2;
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
