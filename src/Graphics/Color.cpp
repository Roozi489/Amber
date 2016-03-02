#include "Graphics/Color.h"

namespace Amber
{

const Color Color::White = { 255, 255, 255, 255 };
const Color Color::Gray = { 128, 128, 128, 255 };
const Color Color::Black = { 0, 0, 0, 255 };
const Color Color::Brown = { 139, 69, 19, 255 };
const Color Color::Red = { 255, 0, 0, 255 };
const Color Color::Orange = { 255, 153, 0, 255 };
const Color Color::Yellow = { 255, 255, 0, 255 };
const Color Color::Green = { 0, 255, 0, 255 };
const Color Color::Cyan = { 0, 255, 255, 255 };
const Color Color::Blue = { 0, 0, 255, 255 };
const Color Color::Magenta = { 255, 0, 255, 255 };
const Color Color::Transparent = { 0, 0, 0, 0 };

Color::Color()
{
}

Color::Color(uint8 r, uint8 g, uint8 b, uint8 a)
	: r(r)
	, g(g)
	, b(b)
	, a(a)
{

}

Vector3f Color::toNormalizedRGB() const
{
	return Vector3f(r / 255.f, g / 255.f, b / 255.f);
}

Vector4f Color::toNormalizedRGBA() const
{
	return Vector4f(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
}

Color Color::fromByteRGB(uint8 r, uint8 g, uint8 b)
{
	Color color;
	color.r = r;
	color.g = g;
	color.b = b;
	return color;
}

Color Color::fromByteRGBA(uint8 r, uint8 g, uint8 b, uint8 a)
{
	Color color;
	color.r = r;
	color.g = g;
	color.b = b;
	color.a = a;
	return color;
}

Color Color::fromNormalizedRGB(float r, float g, float b)
{
	Color color;
	color.r = static_cast<uint8_t>(r * 255);
	color.g = static_cast<uint8_t>(g * 255);
	color.b = static_cast<uint8_t>(b * 255);
	return color;
}

Color Color::fromNormalizedRGBA(float r, float g, float b, char a)
{
	Color color;
	color.r = static_cast<uint8_t>(r * 255);
	color.g = static_cast<uint8_t>(g * 255);
	color.b = static_cast<uint8_t>(b * 255);
	color.a = static_cast<uint8_t>(a * 255);
	return color;
}

}