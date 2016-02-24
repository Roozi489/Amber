#include "RenderComponent.h"
#include "Globals.h"

RenderComponent::RenderComponent(const std::string& shader, const std::string& meshName)
	: shaderName(shader)
{
	mesh = gMeshManager.getMesh(meshName);
}

RenderComponent::~RenderComponent()
{
}
