#pragma once
#include "BaseSystem.h"
#include "RenderTexture.h"
#include "Skybox.h"
#include "Utility.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_CACHE_H
#include "GBuffer.h"

typedef struct FontType_
{
    const char*  filepathname;
    int          face_index;
    int          cmap_index;
    int          num_indices;
    void*        file_address;  // for preloaded files - what is this???
    size_t       file_size;

} FontType, *FontHandle;

class RenderSystem : public BaseSystem
{
public:
    RenderSystem();
    ~RenderSystem();

    virtual void configure() override;
	virtual void update(Time delta) override;
	virtual void destroy() override;
	
	void geometryPass();
	void lightPass();
	void outPass();

	void textRendering();

    void renderText(const std::string& text, float x, float y, float sx, float sy);
private:
	// Font rendering
    FT_Library ftLib;
    FTC_Manager ftCacheManager;
    FTC_ImageCache ftImageCache;
    FTC_SBitCache ftSmallBitmapsCache;
    FTC_CMapCache ftCharmapCache;

	GLuint fontVao;
	GLuint fontVbo;
	Texture fontTexture;

    size_t maxFonts = 16;
    int numFonts = 0;
    FontHandle* fonts;

	GBuffer gBuffer;
	RenderTexture renderTexture;

    Skybox skybox;

    FT_Error ftAddFont(const char* filepath, FT_Bool outline_only);
    FT_UInt ftGetIndex(FTC_FaceID faceID, FT_UInt32 charcode);
    FT_Error ftIndexToBitmap(FT_ULong index, FTC_FaceID faceID, int* left, int* top, int* xAdvance, int* yAdvance, Bitmap* outBitmap, FT_Glyph* outGlyph);
    FT_Error ftGlyphToBitmap(FT_Glyph glyph, int* left, int* top, int* xAdvance, int* yAdvance, Bitmap* outBitmap, FT_Glyph* outGlyph);
};

