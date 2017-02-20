#pragma once
#include "Core/Types.h"
#include "Math/Vector.h"

namespace Amber
{

class Color
{
public:
	union
	{
		struct
		{
			float r;
			float g;
			float b;
			float a;
		};
		Vector3f rgb;
		Vector4f rgba;
		float data[4];
	};

	static const Color White;
	static const Color Gray;
	static const Color Black;
	static const Color Brown;
	static const Color Red;
	static const Color Orange;
	static const Color Yellow;
	static const Color Green;
	static const Color Cyan;
	static const Color Blue;
	static const Color Magenta;
	static const Color Transparent;

	Color();

	static Color fromByteRGB(uint8 r, uint8 g, uint8 b);
	static Color fromByteRGBA(uint8 r, uint8 g, uint8 b, uint8 a);
	static Color fromNormalizedRGB(float r, float g, float b);
	static Color fromNormalizedRGBA(float r, float g, float b, char a);

private:
	Color(uint8 r, uint8 g, uint8 b, uint8 a);
};

}