#include "Components/RenderComponent.h"
#include "Core/Globals.h"

namespace Amber
{

RenderComponent::RenderComponent(const std::string& meshName)
{
	mesh = g_meshManager.getMesh(meshName);
}

RenderComponent::~RenderComponent()
{
}

}