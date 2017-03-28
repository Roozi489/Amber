#pragma once
#include "Graphics/Renderer.h"
#include "Graphics/Lighting.h"
#include "Graphics/GL/RenderTexture.h"
#include "Graphics/GL/Skybox.h"
#include "Graphics/GL/GBuffer.h"
#include "Graphics/GL/RenderTextureCubeShadow.h"
#include "Graphics/GL/CommandQueue.h"
#include "Graphics/RenderText.h"

namespace Amber
{

class GLRenderer : public Renderer
{
public:
	GLRenderer();
	~GLRenderer();

	virtual void init() override;
	virtual void handleWindowSizeChange(int newWidth, int newHeight) override;
	virtual void renderAll(Time delta) override;
	virtual void finalRenderToMainFramebuffer() override;

	void displayText(const std::string& text, Vector2f position, FontID fontId = FontID::Default);
	void drawFullscreenQuad();

	void geometryPass();
	void lightPass();
	void outPass();

	Texture& getOutColorTexture();

private:
	AmbientLight m_ambientLight;
	std::vector<DirectionalLight> m_directionalLights;
	std::vector<PointLight> m_pointLights;
	std::vector<SpotLight> m_spotLights;

	RenderTexture m_dirLightShadowRT;
	RenderTextureCubeShadow m_pointLightShadowRT;
	RenderTexture m_spotLightShadowRT;

	Skybox m_skybox;

	std::vector<RenderText> m_textsToRender;

	GBuffer m_gBuffer;
	RenderTexture m_outRT;
	RenderTexture m_lightingRT;

	GLuint m_fontVao;
	GLuint m_fontVbo;
	Texture m_fontTexture;

	// Debug info
	const Time m_fpsUpdateInterval = milliseconds(500);
	int m_fps = 0;
	int m_fpsCounter = 0;
	Time m_timeSinceFpsUpdate = Time::Zero;
	int m_culledObjectsCount = 0;

	// Settings
	static const int UseInstancingCountThreshold = 1000;
	const bool m_frustumCullingEnabled = true;

	void applyCommandQueueSettings(const CommandQueueSettings& settings);
	void setDynamicVBOs(const Batch& batch);
	void setUniforms(ShaderProgram* program, const std::vector<UniformValue>& values);
};

}
