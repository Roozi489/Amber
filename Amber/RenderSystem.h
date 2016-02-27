#pragma once
#include "BaseSystem.h"
#include "RenderTexture.h"
#include "Skybox.h"
#include "GBuffer.h"
#include "Lighting.h"

class RenderSystem : public BaseSystem
{
public:
	AmbientLight ambientLight;
	std::vector<DirectionalLight> directionalLights;
	std::vector<PointLight> pointLights;
	std::vector<SpotLight> spotLights;

    RenderSystem();
    ~RenderSystem();

    virtual void init() override;
	virtual void destroy() override;
	virtual void update(Time delta) override;
	
	void geometryPass();
	void lightPass();
	void outPass();

private:
	GBuffer m_gBuffer;
	RenderTexture m_lightingRenderTexture;
	RenderTexture m_outRenderTexture;

	Skybox m_skybox;

	void renderFullscreenQuad();
};

