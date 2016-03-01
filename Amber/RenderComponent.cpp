#include "RenderComponent.h"
#include "Globals.h"

RenderComponent::RenderComponent(const std::string& meshName)
{
	mesh = g_meshManager.getMesh(meshName);
}

RenderComponent::~RenderComponent()
{
}
