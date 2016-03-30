#pragma once
#include "BaseSystem.h"
#include "Graphics/RenderTexture.h"
#include "Graphics/Skybox.h"
#include "Graphics/GBuffer.h"
#include "Graphics/Lighting.h"
#include "Graphics/RenderText.h"
#include "Graphics/RenderTextureCubeShadow.h"

namespace Amber
{

class RenderSystem : public BaseSystem
{
public:
	RenderSystem();
	~RenderSystem() = default;

	virtual void init() override;
	virtual void destroy() override;
	virtual void update(Time delta) override;

	void displayText(const std::string& text, Vector2f position, FontID fontId = FontID::Default);
	void drawFullscreenQuad();

	void geometryPass();
	void lightPass();
	void outPass();

	Texture& RenderSystem::getOutColorTexture();

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
	bool m_frustumCullingEnabled = true;
};

}