#pragma once
#include "Vector.h"
#include "FontManager.h"

#include <string>

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

