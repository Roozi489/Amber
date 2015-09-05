#include "RenderComponent.h"
#include "Globals.h"

#include <cassert>

RenderComponent::RenderComponent(const std::string& shader, const std::string& meshName)
	: shaderName(shader)
	, color(Color::White)
{
	mesh = gMeshManager.getMesh(meshName);
}

RenderComponent::~RenderComponent()
{
}
