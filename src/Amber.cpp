#include "Amber.h"
#include "Core/Globals.h"
#include "Core/Log.h"
#include "Systems/RenderSystem.h"
#include "Systems/GameplaySystem.h"
#include "Systems/SoundSystem.h"
#include "Window/Input.h"
#include <Window/File.h>

namespace Amber
{

Amber::Amber()
{
}

Amber::~Amber()
{
}

void Amber::run()
{
	clearFile("Logs/shader_compile_error.txt");
	Log::init();
	Log::clearLogs();
	init();

	g_startTime = Time::now();
	Time previous = g_startTime;

	while (true)
	{
		Time current = Time::now();
		Time elapsed = current - previous;
		g_frameTime = elapsed;

		update(elapsed);

		previous = current;
	}
}

void Amber::init()
{
	g_window.create();
	Input::init();

	//
	// Gameplay stuff
	g_world.init();

	g_world.addSystem<RenderSystem>();
	g_world.addSystem<GameplaySystem>();
	g_world.addSystem<SoundSystem>();
	g_world.initSystems();

	g_world.setupLevel();

	g_camera.init();
	g_camera.setPosition(Vector3f(0.f, 38.f, 40.f));
	g_camera.offsetOrientation(0.f, -0.9f);
}

void Amber::update(Time delta)
{
	Input::update();
	g_window.update();
	g_camera.update(delta);
	g_world.update(delta);
	g_window.display();

}

}
