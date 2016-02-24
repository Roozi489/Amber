#include "RenderComponent.h"
#include "Globals.h"

RenderComponent::RenderComponent(const std::string& meshName)
{
	mesh = gMeshManager.getMesh(meshName);
}

RenderComponent::~RenderComponent()
{
}
