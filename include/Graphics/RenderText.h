#pragma once
#include "Math/Vector.h"
#include "Managers/FontManager.h"

#include <string>

namespace Amber
{

class RenderText
{
public:
	RenderText(const std::string& text, const Vector2f& position, FontID fontId);
	RenderText() = default;
	~RenderText() = default;

	std::string text;
	Vector2f position = Vector2f::Zero;
	FontID fontId = Default;
};

}