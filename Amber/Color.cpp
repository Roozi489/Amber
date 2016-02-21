#include "Color.h"

const Color Color::Black(0.f, 0.f, 0.f);
const Color Color::White(1.f, 1.f, 1.f);
const Color Color::Red(1.f, 0.f, 0.f);
const Color Color::Green(0, 1.f, 0.f);
const Color Color::Blue(0.f, 0.f, 1.f);
const Color Color::Yellow(1.f, 1.f, 0.f);
const Color Color::Magenta(1.f, 0.f, 1.f);
const Color Color::Cyan(0.f, 1.f, 1.f);
const Color Color::Transparent(0.f, 0.f, 0.f, 0.f);

Color::Color()
	: Color(1.f, 1.f, 1.f, 1.f)
{

}

Color::Color(float r, float g, float b)
	: Color(r, g, b, 1.f)
{

}

Color::Color(float r, float g, float b, float a)
	: r(r)
	, g(g)
	, b(b)
	, a(a)
{

}