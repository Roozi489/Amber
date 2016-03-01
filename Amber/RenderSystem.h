#pragma once
#include "BaseSystem.h"
#include "RenderTexture.h"
#include "Skybox.h"
#include "GBuffer.h"
#include "Lighting.h"
#include "RenderText.h"

class RenderSystem : public BaseSystem
{
public:
    RenderSystem();
    ~RenderSystem();

    virtual void init() override;
	virtual void destroy() override;
	virtual void update(Time delta) override;

	void displayText(const std::string& text, Vector2f position, FontID fontId = FontID::Default);
	void drawFullscreenQuad();

	void geometryPass();
	void lightPass();
	void outPass();

	RenderTexture& getOutRenderTexture();

private:
	AmbientLight m_ambientLight;
	std::vector<DirectionalLight> m_directionalLights;
	std::vector<PointLight> m_pointLights;
	std::vector<SpotLight> m_spotLights;
	
	Skybox m_skybox;

	std::vector<RenderText> m_textsToRender;

	GBuffer m_gBuffer;
	RenderTexture m_lightingRenderTexture;
	RenderTexture m_outRenderTexture;

	GLuint m_fontVao;
	GLuint m_fontVbo;
	Texture m_fontTexture;

	int m_fps = 0;
	int m_fpsCounter = 0;
	Time m_timeSinceFpsUpdate = Time::Zero;
};

