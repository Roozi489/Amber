#include "RenderComponent.h"
#include "Globals.h"

RenderComponent::RenderComponent(const std::string& meshName)
{
	mesh = g_MeshManager.getMesh(meshName);
}

RenderComponent::~RenderComponent()
{
}
