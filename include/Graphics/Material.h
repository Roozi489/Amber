#pragma once
#include "Color.h"

namespace Amber
{

// Used for PBR rendering
struct Material
{
	Color color = Color::White;
	float roughness = 0.f;
	float metalness = 0.f;

	Material() = default;
};

}