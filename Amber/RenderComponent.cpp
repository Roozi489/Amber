#include "RenderComponent.h"
#include "Globals.h"

#include <cassert>

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
		assert(false && "Mesh from code not implemented");
    }
}

RenderComponent::~RenderComponent()
{
}
