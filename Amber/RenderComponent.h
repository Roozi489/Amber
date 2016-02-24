#pragma once
#include "BaseComponent.h"
#include "Mesh.h"
#include "Material.h"

#include <string>

class RenderComponent : public BaseComponent
{
public:
    Mesh* mesh;
    std::string shaderName;

	Material material;

    RenderComponent() = default;
    RenderComponent(const std::string& shader, const std::string& meshName);
    ~RenderComponent();
};

