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
	: r(static_cast<float>(r) / 255)
	, g(static_cast<float>(g) / 255)
	, b(static_cast<float>(b) / 255)
	, a(static_cast<float>(a) / 255)
{

}

Color Color::fromByteRGB(uint8 r, uint8 g, uint8 b)
{
	Color color;
	color.r = static_cast<float>(r) / 255;
	color.g = static_cast<float>(g) / 255;
	color.b = static_cast<float>(b) / 255;
	color.a = 1.f;
	return color;
}

Color Color::fromByteRGBA(uint8 r, uint8 g, uint8 b, uint8 a)
{
	Color color;
	color.r = static_cast<float>(r) / 255;
	color.g = static_cast<float>(g) / 255;
	color.b = static_cast<float>(b) / 255;
	color.a = static_cast<float>(a) / 255;
	return color;
}

Color Color::fromNormalizedRGB(float r, float g, float b)
{
	Color color;
	color.r = r;
	color.g = g;
	color.b = b;
	return color;
}

Color Color::fromNormalizedRGBA(float r, float g, float b, char a)
{
	Color color;
	color.r = r;
	color.g = g;
	color.b = b;
	color.a = a;
	return color;
}

}