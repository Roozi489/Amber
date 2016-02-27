#pragma once
#include "OpenGL.h"
#include "Texture.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_CACHE_H

namespace
{
	typedef struct FontType_
	{
		const char*  filepathname;
		int          face_index;
		int          cmap_index;
		int          num_indices;
		void*        file_address;  // for preloaded files - what is this???
		size_t       file_size;

	} FontType, *FontHandle;
}

struct Bitmap;

class FontManager
{
public:
	size_t maxFonts = 16;
	int numFonts = 0;
	FontHandle* fonts;

	// Font rendering
	FT_Library ftLib;
	FTC_Manager ftCacheManager;
	FTC_ImageCache ftImageCache;
	FTC_SBitCache ftSmallBitmapsCache;
	FTC_CMapCache ftCharmapCache;

	GLuint fontVao;
	GLuint fontVbo;
	Texture fontTexture;

	FontManager();
	~FontManager();

	void init();
	void textRendering();
	void renderText(const std::string& text, float x, float y, float sx, float sy);

	FT_Error ftAddFont(const char* filepath, FT_Bool outline_only);
	FT_UInt ftGetIndex(FTC_FaceID faceID, FT_UInt32 charcode);
	FT_Error ftIndexToBitmap(FT_ULong index, FTC_FaceID faceID, int* left, int* top, int* xAdvance, int* yAdvance, Bitmap* outBitmap, FT_Glyph* outGlyph);
	FT_Error ftGlyphToBitmap(FT_Glyph glyph, int* left, int* top, int* xAdvance, int* yAdvance, Bitmap* outBitmap, FT_Glyph* outGlyph);
};

