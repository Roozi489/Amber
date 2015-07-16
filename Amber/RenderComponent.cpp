#include "RenderComponent.h"
#include "Globals.h"

RenderComponent::RenderComponent(const std::string& shader, const std::string& meshName, MeshType meshType)
    : shaderName(shader)
    , color(Color::White)
{
    if (meshType == FromOBJ)
    {
        mesh = gMeshManager.getMesh(meshName);
    }
    else if (meshType == FromCode)
    {
        // Not implemented
    }
}

RenderComponent::~RenderComponent()
{
}
