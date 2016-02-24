#pragma once
#include "Vector.h"

class Color
{
public:
	union
	{
		struct
		{
			uint8_t r;
			uint8_t g;
			uint8_t b;
			uint8_t a;
		};
		Vector3f rgb;
		Vector4f rgba;
	};

	static const Color White;
	static const Color Gray;
	static const Color Black;
	static const Color Brown;
	static const Color Red;
	static const Color Orange;
	static const Color Yellow ;
	static const Color Green;
	static const Color Cyan;
	static const Color Blue;
	static const Color Magenta;
	static const Color Transparent;

	Color();

	Vector3f toNormalizedRGB() const;
	Vector4f toNormalizedRGBA() const;

	static Color fromByteRGB(uint8_t r, uint8_t g, uint8_t b);
	static Color fromByteRGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
	static Color fromNormalizedRGB(float r, float g, float b);
	static Color fromNormalizedRGBA(float r, float g, float b, char a);

private:
	Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
};
