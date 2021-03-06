#pragma once
#include "BaseComponent.h"
#include "Graphics/GL/Mesh.h"
#include "Graphics/Material.h"

#include <string>

namespace Amber
{

class RenderComponent : public BaseComponent
{
public:
	Mesh* mesh;

	Material material;

	RenderComponent() = default;
	RenderComponent(const std::string& meshName);
	~RenderComponent();
};

}