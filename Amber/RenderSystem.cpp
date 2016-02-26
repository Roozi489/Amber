#include "RenderSystem.h"
#include "Globals.h"
#include "TransformComponent.h"
#include "RenderComponent.h"

#include FT_GLYPH_H

static FT_Error faceRequester(FTC_FaceID faceId, FT_Library lib, FT_Pointer tag, FT_Face* newFace);

RenderSystem::RenderSystem()
{

}

RenderSystem::~RenderSystem()
{
}

void RenderSystem::init()
{
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glDrawBuffer(GL_BACK_LEFT);

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

	fonts = (FontHandle*)calloc(maxFonts, sizeof(FontHandle));

	error = ftAddFont("Fonts/FreeSans.ttf", false);
	if (error)
		criticalError("FreeType - Could not load font:  Fonts/FreeSans.ttf");

	//skybox.init();
	

	// Shaders
	gShaderManager.createProgram("geometry", "geometryPass.vert", "geometryPass.frag");
	gShaderManager.createProgram("ambientLight", "fullscreenQuad.vert", "ambientLight.frag");
	gShaderManager.createProgram("directionalLight", "fullscreenQuad.vert", "directionalLight.frag");
	gShaderManager.createProgram("pointLight", "fullscreenQuad.vert", "pointLight.frag");
	gShaderManager.createProgram("spotLight", "fullscreenQuad.vert", "spotLight.frag");
	gShaderManager.createProgram("out", "fullscreenQuad.vert", "out.frag");
	gShaderManager.createProgram("font", "font.vert", "font.frag");

	// Lights
	ambientLight.color = Color::fromByteRGB(50, 50, 50);
	ambientLight.intensity = 0.02f;

	DirectionalLight dirLight;
	dirLight.color = Color::Gray;
	dirLight.intensity = 0.1f;
	dirLight.direction = normalize(Vector3f(1.f, -1.f, -0.5f));
	directionalLights.push_back(dirLight);

	PointLight pointLight;
	pointLight.intensity = 300.f;
	pointLight.color = Color::White;
	pointLight.position = Vector3f(0.f, 20.f, 0.f);
	pointLight.attenuation.range = 22.f;
	pointLights.push_back(pointLight);
	pointLight.intensity = 50.f;
	pointLight.color = Color::Red;
	pointLight.position = Vector3f(10.f, 10.f, 10.f);
	pointLights.push_back(pointLight);
	pointLight.color = Color::Yellow;
	pointLight.position = Vector3f(-10.f, 10.f, 10.f);
	pointLights.push_back(pointLight);
	pointLight.color = Color::Blue;
	pointLight.position = Vector3f(-10.f, 10.f, -10.f);
	pointLights.push_back(pointLight);
	pointLight.color = Color::Green;
	pointLight.position = Vector3f(10.f, 10.f, -10.f);
	pointLights.push_back(pointLight);

	/*
	SpotLight spotLight;
	spotLight.intensity = 300.f;
	spotLight.position = Vector3f(0.f, 20.f, 0.f);
	spotLight.coneAngle = toRadians(30.f);
	spotLights.push_back(spotLight);*/

	// Meshes
	std::unique_ptr<Mesh> fullscreenQuadMesh = std::make_unique<Mesh>();
	fullscreenQuadMesh->meshComponents = MeshComponents::VertexIndex;
	fullscreenQuadMesh->vertices = { { -1.0f, -1.0f, 0.0f }, { 1.0f, -1.0f, 0.0f }, { 1.0f, 1.0f, 0.0f }, { -1.0f, 1.0f, 0.0f } };
	fullscreenQuadMesh->indices = { 0, 1, 2, 2, 3, 0 };
	fullscreenQuadMesh->setVaoAndVbo();
	gMeshManager.meshes.insert(std::make_pair("fullscreenQuad", std::move(fullscreenQuadMesh)));
	// -----------------

	// Font rendering
	GLuint fontShaderProgram = gShaderManager.getShaderProgram("font")->handle;
	glUseProgram(fontShaderProgram);
	fontTexture.genAndBind();
	fontTexture.setFilterAndWrap(TextureFilter::Linear, TextureWrapMode::ClampToEdge);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glGenVertexArrays(1, &fontVao);
	glBindVertexArray(fontVao);

	glGenBuffers(1, &fontVbo);
	glBindBuffer(GL_ARRAY_BUFFER, fontVbo);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	// -----------------

	// RenderTexture
	gBuffer.create(gWindowWidth, gWindowHeight);
	lightingRenderTexture.create(gWindowWidth, gWindowHeight, RenderTexture::Lighting);
	outRenderTexture.create(gWindowWidth, gWindowHeight, RenderTexture::Color);
}

void RenderSystem::update(Time delta)
{
	BaseSystem::update(delta);

	geometryPass();
	lightPass();
	outPass();

	SDL_GL_SwapWindow(gMainWindow);
}

void RenderSystem::geometryPass()
{
	/* TODO:
	1) Run through your game objects and do the update tick on each one. No rendering happens yet.
	2) Run through the list of game objects, frustum/visibility cull each one, and if visible add its meshes (one mesh per material) to a render list. Generally you want two render lists, one for opaque and one for transparent objects.
	3) Sort the render list(s) into order. The exact ordering is up to you, but you generally want to at least sort by material/texture/shader for the opaque list, or by depth for the transparent list.
	4) Run through the sorted render list and do the actual OpenGL calls for each mesh. Only change materials as needed.
	*/

	// TODO: better skybox
	//http://gamedev.stackexchange.com/questions/60313/implementing-a-skybox-with-glsl-version-330
	//skybox.update();
	//skybox.render();

	gBuffer.bind();

	glEnable(GL_DEPTH_TEST);
	glViewport(0, 0, gBuffer.width, gBuffer.height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	const Matrix4x4f& viewMatrix = gCamera.getViewMatrix();
	const Matrix4x4f& projectionMatrix = gCamera.getProjectionMatrix();

	TransformComponent transformComp;
	RenderComponent renderComp;

	ShaderProgram* activeShaderProgram = gShaderManager.getShaderProgram("geometry");
	activeShaderProgram->use();

	for (Entity& entity : gWorld.entityManager.entities_with_components(transformComp, renderComp))
	{
		// TODO: add scale
		Matrix4x4f modelMatrix = Matrix4x4f::translate(transformComp.position) * quaternionToMatrix4x4f(transformComp.orientation);
		Matrix4x4f mvp = projectionMatrix * viewMatrix * modelMatrix;

		// TODO: different shaders for different objects ???
		/*if (activeShaderProgram == nullptr || activeShaderProgram->name != renderComp.shaderName)
		{
			activeShaderProgram = gShaderManager.getShaderProgram(renderComp.shaderName);
			glUseProgram(activeShaderProgram->handle);
		}*/

		activeShaderProgram->setUniform("mvp", mvp);
		activeShaderProgram->setUniform("transform", transformComp);

		activeShaderProgram->setUniform("material.hasTexture", renderComp.mesh->hasTexture);
		if (renderComp.mesh->hasTexture)
		{
			renderComp.mesh->texture->activeAndBind(0);
			activeShaderProgram->setUniform("material.diffuseMap", 0);
			activeShaderProgram->setUniform("material.specularMap", 1);
		}
		else
		{
			activeShaderProgram->setUniform("material.color", renderComp.material.color);
		}

		glBindVertexArray(renderComp.mesh->vao);
		glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(renderComp.mesh->indices.size()), GL_UNSIGNED_SHORT, nullptr);
	}
	checkGlError();

	gBuffer.unbind();

	// Disable depth write for text rendering
	glDepthMask(GL_FALSE);
	// ############################################################
	// TODO: text
	//textRendering();
	// ############################################################
	glDepthMask(GL_TRUE);

}

void RenderSystem::lightPass()
{
	gBuffer.textures[GBuffer::TextureType::Albedo].activeAndBind(0);
	gBuffer.textures[GBuffer::TextureType::Specular].activeAndBind(1);
	gBuffer.textures[GBuffer::TextureType::Normal].activeAndBind(2);
	gBuffer.textures[GBuffer::TextureType::Depth].activeAndBind(3);

	lightingRenderTexture.bind();

	glDisable(GL_DEPTH_TEST);
	glViewport(0, 0, lightingRenderTexture.width, lightingRenderTexture.height);
	glClear(GL_COLOR_BUFFER_BIT);

	glDepthMask(false);
	defer(glDepthMask(true));

	glEnable(GL_BLEND);
	defer(glDisable(GL_BLEND));
	glBlendFunc(GL_ONE, GL_ONE);

	// Ambient light
	if (ambientLight.isOn)
	{
		ShaderProgram* shaderProgram = gShaderManager.getShaderProgram("ambientLight");

		shaderProgram->use();
		shaderProgram->setUniform("light.coloredIntensity", ambientLight.color.toNormalizedRGB() * ambientLight.intensity);

		Mesh* quad = gMeshManager.getMesh("fullscreenQuad");
		glBindVertexArray(quad->vao);
		glDrawElements(GL_TRIANGLES, static_cast<int>(quad->indices.size()), GL_UNSIGNED_SHORT, nullptr);
	}

	// Directional lights
	ShaderProgram* shaderProgram = gShaderManager.getShaderProgram("directionalLight");
	shaderProgram->use();
	shaderProgram->setUniform("specular", 1);
	shaderProgram->setUniform("normal", 2);
	for (const auto& light : directionalLights)
	{
		if (light.isOn)
		{
			shaderProgram->setUniform("light.base.coloredIntensity", light.color.toNormalizedRGB() * light.intensity);
			shaderProgram->setUniform("light.direction", light.direction);

			Mesh* quad = gMeshManager.getMesh("fullscreenQuad");
			glBindVertexArray(quad->vao);
			glDrawElements(GL_TRIANGLES, static_cast<int>(quad->indices.size()), GL_UNSIGNED_SHORT, nullptr);
		}
	}

	// Point lights
	shaderProgram = gShaderManager.getShaderProgram("pointLight");
	shaderProgram->use();
	shaderProgram->setUniform("specular", 1);
	shaderProgram->setUniform("normal", 2);
	shaderProgram->setUniform("depth", 3);
	const auto& vpInverse = inverse(gCamera.getProjectionMatrix() * gCamera.getViewMatrix());
	for (const auto& light : pointLights)
	{
		if (light.isOn)
		{
			shaderProgram->setUniform("light.base.coloredIntensity", light.color.toNormalizedRGB() * light.intensity);
			shaderProgram->setUniform("light.position", light.position);
			shaderProgram->setUniform("light.attenuation.constant", light.attenuation.constant);
			shaderProgram->setUniform("light.attenuation.linear", light.attenuation.linear);
			shaderProgram->setUniform("light.attenuation.quadratic", light.attenuation.quadratic);
			shaderProgram->setUniform("light.attenuation.range", light.attenuation.range);
			shaderProgram->setUniform("vpInverse", vpInverse);

			Mesh* quad = gMeshManager.getMesh("fullscreenQuad");
			glBindVertexArray(quad->vao);
			glDrawElements(GL_TRIANGLES, static_cast<int>(quad->indices.size()), GL_UNSIGNED_SHORT, nullptr);
		}
	}

	// Spot lights
	shaderProgram = gShaderManager.getShaderProgram("spotLight");
	shaderProgram->use();
	shaderProgram->setUniform("specular", 1);
	shaderProgram->setUniform("normal", 2);
	shaderProgram->setUniform("depth", 3);
	for (const auto& light : spotLights)
	{
		if (light.isOn)
		{
			shaderProgram->setUniform("light.base.base.coloredIntensity", light.color.toNormalizedRGB() * light.intensity);
			shaderProgram->setUniform("light.base.position", light.position);
			shaderProgram->setUniform("light.base.attenuation.constant", light.attenuation.constant);
			shaderProgram->setUniform("light.base.attenuation.linear", light.attenuation.linear);
			shaderProgram->setUniform("light.base.attenuation.quadratic", light.attenuation.quadratic);
			shaderProgram->setUniform("light.base.attenuation.range", light.attenuation.range);
			shaderProgram->setUniform("light.direction", light.direction);
			shaderProgram->setUniform("light.coneAngle", light.coneAngle);
			shaderProgram->setUniform("vpInverse", vpInverse);

			Mesh* quad = gMeshManager.getMesh("fullscreenQuad");
			glBindVertexArray(quad->vao);
			glDrawElements(GL_TRIANGLES, static_cast<int>(quad->indices.size()), GL_UNSIGNED_SHORT, nullptr);
		}
	}
	shaderProgram->stopUsing();

	lightingRenderTexture.unbind();
}

void RenderSystem::outPass()
{
	// TODO: render to outRenderTexture and show it in window class
	//outRenderTexture.bind();

	glDisable(GL_DEPTH_TEST);
	glViewport(0, 0, outRenderTexture.width, outRenderTexture.height);
	glClear(GL_COLOR_BUFFER_BIT);

	ShaderProgram* program = gShaderManager.getShaderProgram("out");

	program->use();

	gBuffer.textures[GBuffer::TextureType::Albedo].activeAndBind(0);
	program->setUniform("diffuse", 0);
	lightingRenderTexture.colorTexture.activeAndBind(1);
	program->setUniform("lighting", 1);

	Mesh* quad = gMeshManager.getMesh("fullscreenQuad");
	glBindVertexArray(quad->vao);
	glDrawElements(GL_TRIANGLES, static_cast<int>(quad->indices.size()), GL_UNSIGNED_SHORT, nullptr);

	//outRenderTexture.unbind();

	program->stopUsing();
}

void RenderSystem::destroy()
{

}

void RenderSystem::textRendering()
{
	GLuint fontShaderProgram = gShaderManager.getShaderProgram("font")->handle;
	glUseProgram(fontShaderProgram);

	glBindVertexArray(fontVao);

	fontTexture.activeAndBind(0);
	GLint texSamplerLoc = glGetUniformLocation(fontShaderProgram, "tex");
	glUniform1i(texSamplerLoc, 0);

	float sx = 2.0f / gWindowWidth;
	float sy = 2.0f / gWindowHeight;

	renderText(std::to_string(gFrameTime.asMilliseconds()) + " ms", -0.99f, 0.95f, sx, sy);
}

void RenderSystem::renderText(const std::string& text, float x, float y, float sx, float sy)
{
	GLint fontColorLoc = glGetUniformLocation(gShaderManager.getShaderProgram("font")->handle, "fontColor");
	Color c = Color::White;

	glUniform4f(fontColorLoc, c.r, c.g, c.b, c.a);
	glBindBuffer(GL_ARRAY_BUFFER, fontVbo);

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

		if (numFonts >= maxFonts)
		{
			maxFonts *= 2;
			fonts = (FontHandle*)realloc(fonts, maxFonts * sizeof(FontHandle));

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
	scaler.height = (FT_UInt)12;
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