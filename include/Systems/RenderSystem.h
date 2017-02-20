#pragma once
#include "BaseSystem.h"
#include "Graphics/GL/RenderTexture.h"
#include "Graphics/GL/Skybox.h"
#include "Graphics/GL/GBuffer.h"
#include "Graphics/Lighting.h"
#include "Graphics/RenderText.h"
#include "Graphics/GL/RenderTextureCubeShadow.h"
#include <Graphics/GL/CommandQueue.h>
#include "Graphics/Renderer.h"

namespace Amber
{

struct UniformValue;
struct CommandQueueSettings;
class ShaderProgram;

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
	std::unique_ptr<Renderer> m_renderer;

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
