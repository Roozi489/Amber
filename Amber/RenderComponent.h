#pragma once
#include "BaseComponent.h"
#include "Mesh.h"

#include <string>

enum MeshType
{
    FromOBJ,
    FromCode
};

class RenderComponent : public BaseComponent
{
public:
    Mesh* mesh;
    std::string shaderName;

    Color color;

    RenderComponent() = default;
    RenderComponent(const std::string& shader, const std::string& meshName, MeshType meshType = FromOBJ);
    ~RenderComponent();
};

