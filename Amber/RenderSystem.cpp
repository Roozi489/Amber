#include "RenderSystem.h"
#include "Globals.h"
#include "TransformComponent.h"
#include "RenderComponent.h"
#include "GameplaySystem.h"

#include FT_GLYPH_H

static FT_Error faceRequester(FTC_FaceID faceId, FT_Library lib, FT_Pointer tag, FT_Face* newFace);

RenderSystem::RenderSystem()
    : maxFonts(0)
    , numFonts(0)
{
    log("Initializing FreeType library...");

    FT_Error error = FT_Init_FreeType(&ftLib);
    if (error)
        criticalError("Failed to initialize FreeType library");

    error = FTC_Manager_New(ftLib, 0, 0, 0, faceRequester, 0, &ftCacheManager);
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

    error = ftAddFont("Fonts/FreeSans.ttf", false);

    // TODO: font manager
    /*if (FT_New_Face(gFtLib, "Fonts/FreeSans.ttf", 0, &gFreeSansFace32))
    {
    criticalError("Failed to load font FreeSans");
    }
    FT_Set_Pixel_Sizes(gFreeSansFace32, 0, 32);

    if (FT_New_Face(gFtLib, "Fonts/FreeSans.ttf", 0, &gFreeSansFace128))
    {
    criticalError("Failed to load font FreeSans");
    }
    FT_Set_Pixel_Sizes(gFreeSansFace128, 0, 128);*/
}

RenderSystem::~RenderSystem()
{
}

void RenderSystem::configure()
{
    gameplaySystem = gWorld.getSystem<GameplaySystem>();
    //skybox.init();

    // TODO: automate this
    gShaderManager.createProgram("default", "default.vert", "default.frag");
    GLuint fontShaderProgram = gShaderManager.createProgram("font", "font.vert", "font.frag");

    // Font rendering
    glUseProgram(fontShaderProgram);
    glGenTextures(1, &fontTexID);
    glBindTexture(GL_TEXTURE_2D, fontTexID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glGenVertexArrays(1, &fontVao);
    glBindVertexArray(fontVao);

    glGenBuffers(1, &fontVbo);
    glBindBuffer(GL_ARRAY_BUFFER, fontVbo);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
}

void RenderSystem::update(float delta)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //skybox.update();
    //mSkybox.render();

    //view = Math::scale(Vector3(1) / transform.scale) *    quaternionToMatrix4(conjugate(transform.orientation)) *        Math::translate(-transform.position);


    Vector3f up(0.f, -1.f, 0.f);

    Matrix4x4f viewMatrix = Matrix4x4f::lookAt(gCamera.getPosition(), gCamera.getTarget(), up);
    Matrix4x4f projectionMatrix = gCamera.getProjectionMatrix();

    TransformComponent transformComp;
    RenderComponent renderComp;

    for (Entity& entity : gWorld.entityManager.entities_with_components(transformComp, renderComp))
    {
        Matrix4x4f modelMatrix = Matrix4x4f::translate(transformComp.position) * Matrix4x4f::rotate(transformComp.angle, transformComp.axis);
        Matrix4x4f MVP = projectionMatrix * viewMatrix * modelMatrix;

        GLuint programID = gShaderManager.getShaderProgram(renderComp.shaderName);
        glUseProgram(programID);

        GLint matrixMVPLoc = glGetUniformLocation(programID, "MVP");
        GLint viewMatrixLoc = glGetUniformLocation(programID, "V");
        GLint modelMatrixLoc = glGetUniformLocation(programID, "M");
        GLint useTextureLoc = glGetUniformLocation(programID, "useTexture");
        GLint textureSamplerLoc = glGetUniformLocation(programID, "texSampler");
        GLint colorLoc = glGetUniformLocation(programID, "color");

        glUniformMatrix4fv(matrixMVPLoc, 1, GL_FALSE, &MVP[0][0]);
        glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &modelMatrix[0][0]);
        glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, &viewMatrix[0][0]);

        glUniform1i(useTextureLoc, (int)renderComp.mesh->hasTexture);
        glUniform4f(colorLoc, renderComp.color.r, renderComp.color.g, renderComp.color.b, renderComp.color.a);

        if (renderComp.mesh->hasTexture)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, renderComp.mesh->texture.textureID);
            glUniform1i(textureSamplerLoc, 0);
        }

        GLint cameraPosLoc = glGetUniformLocation(programID, "cameraPosition");
        glUniform3f(cameraPosLoc, gCamera.getPosition().x, gCamera.getPosition().y, gCamera.getPosition().z);
        GLint cameraDirLoc = glGetUniformLocation(programID, "cameraDirection");
        Vector3f cameraDirection = normalize(gCamera.getTarget() - gCamera.getPosition());
        glUniform3f(cameraDirLoc, cameraDirection.x, cameraDirection.y, cameraDirection.z);

        glBindVertexArray(renderComp.mesh->vao);
        glDrawElements(GL_TRIANGLES, (GLsizei)renderComp.mesh->indices.size(), GL_UNSIGNED_SHORT, (void*)0);

        checkGlError();
    }

    textRendering();

    SDL_GL_SwapWindow(gMainWindow);
}

void RenderSystem::textRendering()
{
    GLuint fontShaderProgram = gShaderManager.getShaderProgram("font");
    glUseProgram(fontShaderProgram);

    glBindVertexArray(fontVao);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, fontTexID);
    GLint texSamplerLoc = glGetUniformLocation(fontShaderProgram, "tex");
    glUniform1i(texSamplerLoc, 0);

    float sx = 2.0f / gWindowWidth;
    float sy = 2.0f / gWindowHeight;

    switch (gameplaySystem->gameState)
    {
    case GameState::Start:
        renderText("Use left and right arrows keys to move pads", -0.6f, -0.30f, sx, sy);
        renderText("Press space to launch ball", -0.38f, -0.4f, sx, sy);
        renderText("Press F1, F2 or F3 to change camera", -0.49f, -0.5f, sx, sy);
    case GameState::Playing:
        renderText("Score: " + std::to_string(gameplaySystem->score), -0.98f, 0.91f, sx, sy);
        renderText("Lives remaining: " + std::to_string(gameplaySystem->lives), 0.47f, 0.91f, sx, sy);
        break;
    case GameState::Victory:
        renderText("Victory", -0.38f, 0.05f, sx, sy);
        renderText("Your Score: " + std::to_string(gameplaySystem->score), -0.26f, -0.15f, sx, sy);
        break;
    case GameState::Defeat:
        renderText("Game Over", -0.65f, 0.05f, sx, sy);
        renderText("Your Score: " + std::to_string(gameplaySystem->score), -0.26f, -0.15f, sx, sy);
        break;
    default:
        break;
    }
}

void RenderSystem::renderText(const std::string& text, float x, float y, float sx, float sy)
{
    GLint fontColorLoc = glGetUniformLocation(gShaderManager.getShaderProgram("font"), "fontColor");
    Color c = Color::White;

    glUniform4f(fontColorLoc, c.r, c.g, c.b, c.a);
    glBindBuffer(GL_ARRAY_BUFFER, fontVbo);

    //
    FTC_FaceID faceID = (FTC_FaceID)fonts[0];
    //

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
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, bitmap.width, bitmap.height, 0, GL_RED, GL_UNSIGNED_BYTE, bitmap.buffer);
        //glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, bitmapGlyph->bitmap.width, bitmapGlyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, bitmapGlyph->bitmap.buffer);

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
        //x += (face->glyph->metrics.horiAdvance >> 6) * sx;
        // only needed for multi-line
        //y -= (face->glyph->metrics.vertAdvance >> 6) * sy;

        prevGlyphIndex = glyphIndex;
    }
}

FT_Error RenderSystem::ftAddFont(const char* filepath, FT_Bool outline_only)
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
        FontHandle font;

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

        font = (FontHandle)malloc(sizeof(*font));

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

        if (maxFonts == 0)
        {
            maxFonts = 16;
            fonts = (FontHandle*)calloc((size_t)maxFonts, sizeof(FontHandle));
        }
        else if (numFonts >= maxFonts)
        {
            maxFonts *= 2;
            fonts = (FontHandle*)realloc(fonts, (size_t)maxFonts * sizeof(FontHandle));

            memset(&fonts[numFonts], 0, (size_t)(maxFonts - numFonts) * sizeof(FontHandle));
        }

        fonts[numFonts++] = font;
    }

    return FT_Err_Ok;
}

FT_UInt RenderSystem::ftGetIndex(FTC_FaceID faceID, FT_UInt32 charcode)
{
    //FontHandle font = handle->current_font;
    // TODO: replace -1 with font->cmap_index
    return FTC_CMapCache_Lookup(ftCharmapCache, faceID, -1, charcode);
}

FT_Error RenderSystem::ftIndexToBitmap(FT_ULong index, FTC_FaceID faceID, int* left, int* top, int* xAdvance, int* yAdvance, Bitmap* outBitmap, FT_Glyph* outGlyph)
{
    bool useSbitsCache = true;

    FTC_ScalerRec scaler;
    scaler.pixel = 1; // use pixel sizes
    scaler.width = (FT_UInt)0;// TODO: is width needed???
    scaler.height = (FT_UInt)32;
    scaler.x_res = 0;
    scaler.y_res = 0;
    scaler.face_id = faceID;

    FT_Error error;
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

        error = FTC_SBitCache_LookupScaler(ftSmallBitmapsCache, &scaler, FT_LOAD_DEFAULT, index, &sbit, NULL);

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
        FT_Glyph  glyph;
        error = FTC_ImageCache_LookupScaler(ftImageCache, &scaler, FT_LOAD_DEFAULT, index, &glyph, NULL);

        if (!error)
            error = ftGlyphToBitmap(glyph, left, top, xAdvance, yAdvance, outBitmap, outGlyph);
    }

    /* don't accept a `missing' character with zero or negative width */
    if (index == 0 && *xAdvance <= 0)
        *xAdvance = 1;

    return error;
}

FT_Error RenderSystem::ftGlyphToBitmap(FT_Glyph glyph, int* left, int* top, int* xAdvance, int* yAdvance, Bitmap* outBitmap, FT_Glyph* outGlyph)
{
    FT_BitmapGlyph bitmap;
    FT_Bitmap* source;

    *outGlyph = NULL;

    FT_Error error = FT_Err_Ok;

    if (glyph->format == FT_GLYPH_FORMAT_OUTLINE)
    {
        FT_Render_Mode render_mode = FT_RENDER_MODE_NORMAL;

        /* render the glyph to a bitmap, don't destroy original */
        error = FT_Glyph_To_Bitmap(&glyph, render_mode, NULL, 0);
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
    FontHandle font = (FontHandle)faceId;

    FT_Error error = FT_New_Face(lib, font->filepathname, font->face_index, outFace);
    if (!error)
    {
        if ((*outFace)->charmaps)
            (*outFace)->charmap = (*outFace)->charmaps[font->cmap_index];
    }

    return error;
}