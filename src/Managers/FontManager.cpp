#include "Managers/FontManager.h"
#include "Core/Utility.h"
#include "Core/Globals.h"

#include <freetype/ftglyph.h>

namespace Amber
{

static FT_Error faceRequester(FTC_FaceID faceId, FT_Library lib, FT_Pointer tag, FT_Face* newFace);

enum class BitmapFormat
{
	GRAY8,
	RGB8,
	RGBA8
};

struct Bitmap
{
	int width;
	int height;
	BitmapFormat format;
	unsigned char* buffer;
};

FontManager::FontManager()
{
}


FontManager::~FontManager()
{
}

void FontManager::init()
{
	log("Initializing FreeType library...");

	FT_Error error = FT_Init_FreeType(&ftLib);
	if (error)
		criticalError("Failed to initialize FreeType library");

	error = FTC_Manager_New(ftLib, 0, 0, 0, faceRequester, nullptr, &ftCacheManager);
	if (error)
		criticalError("FreeType - Could not initialize FreeType cache manager");

	error = FTC_SBitCache_New(ftCacheManager, &ftSmallBitmapsCache);
	if (error)
		criticalError("FreeType - Could not initialize small bitmaps cache");

	error = FTC_ImageCache_New(ftCacheManager, &ftImageCache);
	if (error)
		criticalError("FreeType - Could not initialize glyph image cache");

	error = FTC_CMapCache_New(ftCacheManager, &ftCharmapCache);
	if (error)
		criticalError("FreeType - Could not initialize charmap cache");

	fonts = new FtFontType*[maxFonts];

	error = ftAddFont("Fonts/FreeSans.ttf", false);
	if (error)
		criticalError("FreeType - Could not load font:  Fonts/FreeSans.ttf");
}

// TODO: add left top or center options
void FontManager::renderText(const std::string& text, float x, float y)
{
	float sx = 2.0f / g_window.getWidth();
	float sy = 2.0f / g_window.getHeight();

	// TODO: fix y value not being top-left
	// Change to <-1, 1> and invert y coord
	auto xPos = [](float value) { return value * 2 - 1; };
	auto yPos = [](float value) { return -(value * 2 - 1); };
	x = xPos(x);
	y = yPos(y);

	ShaderProgram& fontShaderProgram = g_shaderManager.getShaderProgram("font");
	// TODO: add color to the parameters
	Color c = Color::White;

	fontShaderProgram.setUniform("fontColor", c);
	//glBindBuffer(GL_ARRAY_BUFFER, fontVbo);

	// TODO: fix this
	//		 only works with one font
	FTC_FaceID faceID = (FTC_FaceID)fonts[0];
	// #############################################

	FT_Face face;
	FT_Error error = FTC_Manager_LookupFace(ftCacheManager, faceID, &face);
	if (error)
		criticalError("FreeType - FTC_Manager_LookupFace");

	FT_UInt prevGlyphIndex = 0;
	for (std::size_t n = 0; n < text.size(); ++n)
	{
		FT_UInt glyphIndex = ftGetIndex(faceID, text[n]);

		// Handle kerning
		if (n > 0)
		{
			FT_Vector  delta;
			error = FT_Get_Kerning(face, prevGlyphIndex, glyphIndex, FT_KERNING_DEFAULT, &delta);
			if (error)
				criticalError("FreeType - FT_Get_Kerning");

			x += (delta.x >> 6) * sx;
		}

		Bitmap bitmap;
		FT_Glyph glyph;
		int left, top;
		int xAdvance, yAdvance;
		error = ftIndexToBitmap(glyphIndex, faceID, &left, &top, &xAdvance, &yAdvance, &bitmap, &glyph);
		if (error)
			criticalError("ftIndexToBitmap returned error");
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, bitmap.width, bitmap.height, 0, GL_RED, GL_UNSIGNED_BYTE, bitmap.buffer);


		// TODO: center the text position
		float x2 = x + left * sx;
		float y2 = -y - top * sy;
		float w = bitmap.width * sx;
		float h = bitmap.height * sy;

		GLfloat box[4][4] =
		{
			{ x2 + w, -y2 - h, 1.f, 1.f },
			{ x2 + w, -y2, 1.f, 0.f },
			{ x2, -y2 - h, 0.f, 1.f },
			{ x2, -y2, 0.f, 0.f },
		};

		glBufferData(GL_ARRAY_BUFFER, sizeof(box), box, GL_DYNAMIC_DRAW);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		x += xAdvance * sx;
		// only needed for multi-line
		//y -= (face->glyph->metrics.vertAdvance >> 6) * sy;

		prevGlyphIndex = glyphIndex;
	}
}

FT_Error FontManager::ftAddFont(const char* filepath, FT_Bool outline_only)
{
	FT_Encoding encoding = FT_ENCODING_UNICODE;
	static char   filename[1024 + 5];
	long          i, num_faces;
	size_t  len;
	FT_Face       face;


	len = strlen(filepath);
	if (len > 1024)
		len = 1024;

	strncpy_s(filename, filepath, len);
	filename[len] = 0;

	FT_Error error = FT_New_Face(ftLib, filename, 0, &face);
	if (error)
		return error;

	if (outline_only && !FT_IS_SCALABLE(face))
		return FT_Err_Invalid_Argument;

	/* allocate new font object */
	num_faces = face->num_faces;
	for (i = 0; i < num_faces; i++)
	{
		if (i > 0)
		{
			error = FT_New_Face(ftLib, filename, i, &face);
			if (error)
				continue;
		}

		error = FT_Select_Charmap(face, encoding);
		if (error)
		{
			FT_Done_Face(face);
			criticalError("FreeType - FT_Select_Charmap");
			return error;
		}

		FtFontType* font = new FtFontType;

		/* We allocate four more bytes since we want to attach an AFM */
		/* or PFM file for Type 1 fonts (if available).  Such fonts   */
		/* always have the extension `.afm' or `.pfm'.                */
		font->filepathname = (char*)malloc(strlen(filename) + 4 + 1);
		strcpy_s((char*)font->filepathname, strlen(filename) + 4 + 1, filename);

		font->face_index = i;
		font->cmap_index = face->charmap ? FT_Get_Charmap_Index(face->charmap) : 0;

		// TODO: decide whether to preload
		if (false) // preload = false
		{
			FILE* file;
			fopen_s(&file, filename, "rb");
			int file_size;

			if (file == NULL)  /* shouldn't happen */
			{
				free(font);
				return FT_Err_Invalid_Argument;
			}

			fseek(file, 0, SEEK_END);
			file_size = ftell(file);
			fseek(file, 0, SEEK_SET);

			if (file_size <= 0)
				return FT_Err_Invalid_Stream_Operation;

			font->file_address = malloc((size_t)file_size);
			fread(font->file_address, 1, (size_t)file_size, file);

			font->file_size = (size_t)file_size;

			fclose(file);
		}
		else
		{
			font->file_address = NULL;
			font->file_size = 0;
		}

		// Only works for Unicode
		font->num_indices = 0x110000L;

		FT_Done_Face(face);
		face = NULL;

		if (numFonts >= maxFonts)
		{
			maxFonts *= 2;
			fonts = new FtFontType*[maxFonts];
		}

		fonts[numFonts++] = font;
	}

	return FT_Err_Ok;
}

FT_UInt FontManager::ftGetIndex(FTC_FaceID faceID, FT_UInt32 charcode)
{
	// TODO: currentFont
	return FTC_CMapCache_Lookup(ftCharmapCache, faceID, fonts[0]->cmap_index, charcode);
}

FT_Error FontManager::ftIndexToBitmap(FT_ULong index, FTC_FaceID faceID, int* left, int* top, int* xAdvance, int* yAdvance, Bitmap* outBitmap, FT_Glyph* outGlyph)
{
	bool useSbitsCache = true;

	FTC_ScalerRec scaler;
	scaler.pixel = 1; // use pixel sizes
	scaler.width = (FT_UInt)0;// TODO: is width needed???
	scaler.height = (FT_UInt)12;
	scaler.x_res = 0;
	scaler.y_res = 0;
	scaler.face_id = faceID;

	FT_Error error = 0;
	unsigned int  width, height;

	*outGlyph = NULL;
	*xAdvance = 0;

	// use the SBits cache to store small glyph bitmaps
	width = scaler.width;
	height = scaler.height;
	if (useSbitsCache && !scaler.pixel) // scaler.pixel should always be true
	{
		width = ((width * scaler.x_res + 36) / 72) >> 6;
		height = ((height * scaler.y_res + 36) / 72) >> 6;
	}

	bool SBitCache = useSbitsCache && width < 48 && height < 48;
	if (SBitCache)
	{
		FTC_SBit sbit;

		error = FTC_SBitCache_LookupScaler(ftSmallBitmapsCache, &scaler, FT_LOAD_DEFAULT, index, &sbit, nullptr);

		if (error)
			criticalError("FreeType - FTC_ImageCache_LookupScaler");

		if (sbit->buffer)
		{
			outBitmap->height = sbit->height;
			outBitmap->width = sbit->width;
			outBitmap->buffer = sbit->buffer;
			outBitmap->format = BitmapFormat::GRAY8; // Expecting the format to be GRAY8

			*left = sbit->left;
			*top = sbit->top;
			*xAdvance = sbit->xadvance;
			*yAdvance = sbit->yadvance;
		}
		else
		{
			SBitCache = false;
		}
	}

	// Store glyph outlines and render them on demand
	// Also do this for all whitespace characters (sbit->buffer == 0)
	if (!SBitCache)
	{
		FT_Glyph glyph;
		error = FTC_ImageCache_LookupScaler(ftImageCache, &scaler, FT_LOAD_DEFAULT, index, &glyph, nullptr);

		if (error)
			criticalError("FreeType - FTC_ImageCache_LookupScaler");
		error = ftGlyphToBitmap(glyph, left, top, xAdvance, yAdvance, outBitmap, outGlyph);
	}

	/* don't accept a `missing' character with zero or negative width */
	if (index == 0 && *xAdvance <= 0)
		*xAdvance = 1;

	return error;
}

FT_Error FontManager::ftGlyphToBitmap(FT_Glyph glyph, int* left, int* top, int* xAdvance, int* yAdvance, Bitmap* outBitmap, FT_Glyph* outGlyph)
{
	FT_BitmapGlyph bitmap;
	FT_Bitmap* source;

	*outGlyph = NULL;

	FT_Error error = FT_Err_Ok;

	if (glyph->format == FT_GLYPH_FORMAT_OUTLINE)
	{
		FT_Render_Mode render_mode = FT_RENDER_MODE_NORMAL;

		/* render the glyph to a bitmap, don't destroy original */
		error = FT_Glyph_To_Bitmap(&glyph, render_mode, nullptr, 0);
		if (error)
			return error;

		*outGlyph = glyph;
	}

	if (glyph->format != FT_GLYPH_FORMAT_BITMAP)
		criticalError("ftGlyphToBitmap - Invalid glyph format returned!");

	bitmap = (FT_BitmapGlyph)glyph;
	source = &bitmap->bitmap;

	outBitmap->height = (int)source->rows;
	outBitmap->width = (int)source->width;
	outBitmap->buffer = source->buffer;
	outBitmap->format = BitmapFormat::GRAY8;

	*left = bitmap->left;
	*top = bitmap->top;

	*xAdvance = (glyph->advance.x + 0x8000) >> 16;
	*yAdvance = (glyph->advance.y + 0x8000) >> 16;

	return error;
}

static FT_Error faceRequester(FTC_FaceID faceId, FT_Library lib, FT_Pointer tag, FT_Face* outFace)
{
	FtFontType* font = (FtFontType*)faceId;

	FT_Error error = FT_New_Face(lib, font->filepathname, font->face_index, outFace);
	if (!error && (*outFace)->charmaps)
		(*outFace)->charmap = (*outFace)->charmaps[font->cmap_index];

	return error;
}

}