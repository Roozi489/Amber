#pragma once
#include "BaseComponent.h"
#include "Mesh.h"

#include <string>
#include "Color.h"

class RenderComponent : public BaseComponent
{
public:
    Mesh* mesh;
    std::string shaderName;

	// TODO: Move to material
    Color color;

    RenderComponent() = default;
    RenderComponent(const std::string& shader, const std::string& meshName);
    ~RenderComponent();
};

