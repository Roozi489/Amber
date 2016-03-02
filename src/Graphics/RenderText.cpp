#include "Graphics/RenderText.h"

namespace Amber
{

RenderText::RenderText(const std::string& text, const Vector2f& position, FontID fontId)
	: text(text)
	, position(position)
	, fontId(fontId)
{
}

}