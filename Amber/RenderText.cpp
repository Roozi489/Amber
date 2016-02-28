#include "RenderText.h"

RenderText::RenderText(const std::string& text, const Vector2f& position, FontID fontId)
	: text(text)
	, position(position)
	, fontId(fontId)
{
}
