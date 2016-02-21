#pragma once
struct Color
{
	float r;
	float g;
	float b;
	float a;

	static const Color Black;
	static const Color White;
	static const Color Red;
	static const Color Green;
	static const Color Blue;
	static const Color Yellow;
	static const Color Magenta;
	static const Color Cyan;
	static const Color Transparent;

	Color();
	Color(float r, float g, float b);
	Color(float r, float g, float b, float a);
};