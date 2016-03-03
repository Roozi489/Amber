#include "Systems/RenderSystem.h"
#include "Core/Globals.h"
#include "Core/Utility.h"
#include "Core/String.h"
#include "Components/TransformComponent.h"
#include "Components/RenderComponent.h"

namespace Amber
{

RenderSystem::RenderSystem()
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

	// Deferred shading structures
	m_gBuffer.create(g_window.getWidth(), g_window.getHeight());
	m_lightingRT.create(g_window.getWidth(), g_window.getHeight(), RenderTexture::Lighting);
	m_outRT.create(g_window.getWidth(), g_window.getHeight(), RenderTexture::Color);

	// Shaders
	g_shaderManager.createProgram("geometry", "geometryPass.vert", "geometryPass.frag");
	g_shaderManager.createProgram("ambientLight", "fullscreenQuad.vert", "ambientLight.frag");
	g_shaderManager.createProgram("directionalLight", "fullscreenQuad.vert", "directionalLight.frag");
	g_shaderManager.createProgram("pointLight", "fullscreenQuad.vert", "pointLight.frag");
	g_shaderManager.createProgram("spotLight", "fullscreenQuad.vert", "spotLight.frag");
	g_shaderManager.createProgram("shadowMap", "shadowMap.vert");
	g_shaderManager.createProgram("out", "fullscreenQuad.vert", "out.frag");
	g_shaderManager.createProgram("texPass", "fullscreenQuad.vert", "texPass.frag");
	g_shaderManager.createProgram("font", "font.vert", "font.frag");

	g_fontManager.init();

	//m_skybox.init();

	// Lights
	m_ambientLight.color = Color::fromByteRGB(50, 50, 50);
	m_ambientLight.intensity = 0.02f;

	DirectionalLight dirLight;
	dirLight.color = Color::Gray;
	dirLight.intensity = 0.15f;
	dirLight.direction = normalize(Vector3f(1.f, -1.f, -0.5f));
	m_directionalLights.push_back(dirLight);


	dirLight.direction = normalize(Vector3f(0.4f, -0.8f, 0.85f));
	m_directionalLights.push_back(dirLight);

	/*PointLight pointLight;
	pointLight.intensity = 300.f;
	pointLight.color = Color::White;
	pointLight.position = Vector3f(0.f, 20.f, 0.f);
	pointLight.attenuation.range = 22.f;
	m_pointLights.push_back(pointLight);
	pointLight.intensity = 50.f;
	pointLight.color = Color::Red;
	pointLight.position = Vector3f(10.f, 10.f, 10.f);
	m_pointLights.push_back(pointLight);
	pointLight.color = Color::Yellow;
	pointLight.position = Vector3f(-10.f, 10.f, 10.f);
	m_pointLights.push_back(pointLight);
	pointLight.color = Color::Blue;
	pointLight.position = Vector3f(-10.f, 10.f, -10.f);
	m_pointLights.push_back(pointLight);
	pointLight.color = Color::Green;
	pointLight.position = Vector3f(10.f, 10.f, -10.f);
	m_pointLights.push_back(pointLight);

	SpotLight spotLight;
	spotLight.intensity = 300.f;
	spotLight.position = Vector3f(0.f, 20.f, 0.f);
	spotLight.coneAngle = toRadians(17.f);
	m_spotLights.push_back(spotLight);*/

	// Shadow maps
	m_dirLightDepthRT.create(1024, 1024, RenderTexture::Depth);


	// Meshes
	std::unique_ptr<Mesh> fullscreenQuadMesh = std::make_unique<Mesh>();
	fullscreenQuadMesh->meshComponents = MeshComponents::VertexIndex;
	fullscreenQuadMesh->vertices = { { -1.0f, -1.0f, 0.0f }, { 1.0f, -1.0f, 0.0f }, { 1.0f, 1.0f, 0.0f }, { -1.0f, 1.0f, 0.0f } };
	fullscreenQuadMesh->indices = { 0, 1, 2, 2, 3, 0 };
	fullscreenQuadMesh->setVaoAndVbo();
	g_meshManager.meshes.insert(std::make_pair("fullscreenQuad", std::move(fullscreenQuadMesh)));

	m_fontTexture.genAndBind();
	m_fontTexture.setFilterAndWrap(TextureFilter::Linear, TextureWrapMode::ClampToEdge);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glGenVertexArrays(1, &m_fontVao);
	glBindVertexArray(m_fontVao);

	glGenBuffers(1, &m_fontVbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_fontVbo);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
}

void RenderSystem::destroy()
{

}

void RenderSystem::update(Time delta)
{
	BaseSystem::update(delta);

	// TODO: move to window class
	displayText(stringFormat("%5.3f ms", g_frameTime.asMilliseconds()), { 0.01f, 0.02f });
	displayText(stringFormat("%d fps", m_fps), { 0.01f, 0.04f });

	geometryPass();
	lightPass();
	outPass();

	m_fpsCounter++;
	m_timeSinceFpsUpdate += delta;
	if (m_timeSinceFpsUpdate >= milliseconds(500))
	{
		m_fps = m_fpsCounter * 2;
		m_fpsCounter = 0;
		m_timeSinceFpsUpdate -= milliseconds(500);
	}
}

void RenderSystem::displayText(const std::string& text, Vector2f position, FontID fontId)
{
	m_textsToRender.emplace_back(text, position, fontId);
}

void RenderSystem::drawFullscreenQuad()
{
	static Mesh* quad = g_meshManager.getMesh("fullscreenQuad");
	glBindVertexArray(quad->vao);
	glDrawElements(GL_TRIANGLES, static_cast<int>(quad->indices.size()), GL_UNSIGNED_SHORT, nullptr);
}

void RenderSystem::geometryPass()
{
	/* TODO:
	1) Run through your game objects and do the update tick on each one. No rendering happens yet.
	2) Run through the list of game objects, frustum/visibility cull each one, and if visible add its meshes (one mesh per material) to a render list. Generally you want two render lists, one for opaque and one for transparent objects.
	3) Sort the render list(s) into order. The exact ordering is up to you, but you generally want to at least sort by material/texture/shader for the opaque list, or by depth for the transparent list.
	4) Run through the sorted render list and do the actual OpenGL calls for each mesh. Only change materials as needed.
	*/

	// TODO: better m_skybox
	//http://gamedev.stackexchange.com/questions/60313/implementing-a-m_skybox-with-glsl-version-330
	//m_skybox.update();
	//m_skybox.render();

	m_gBuffer.bind();

	glDepthMask(true);
	glEnable(GL_DEPTH_TEST);
	glViewport(0, 0, m_gBuffer.width, m_gBuffer.height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	const Matrix4x4f& vp = g_camera.getProjectionMatrix() * g_camera.getViewMatrix();

	ShaderProgram& activeShaderProgram = g_shaderManager.getShaderProgram("geometry");
	activeShaderProgram.use();
	
	TransformComponent transformComp;
	RenderComponent renderComp;

	for (Entity& entity : g_world.entityManager.entities_with_components(transformComp, renderComp))
	{
		// TODO: add scale
		Matrix4x4f modelMatrix = Matrix4x4f::translate(transformComp.position) * quaternionToMatrix4x4f(transformComp.orientation);
		Matrix4x4f mvp = vp * modelMatrix;

		// TODO: different shaders for different objects ???
		/*if (activeShaderProgram == nullptr || activeShaderProgram->name != renderComp.shaderName)
		{
			activeShaderProgram = g_shaderManager.getShaderProgram(renderComp.shaderName);
			glUseProgram(activeShaderProgram->handle);
		}*/

		activeShaderProgram.setUniform("mvp", mvp);
		activeShaderProgram.setUniform("transform", transformComp);

		activeShaderProgram.setUniform("material.hasTexture", renderComp.mesh->hasTexture);
		if (renderComp.mesh->hasTexture)
		{
			renderComp.mesh->texture->activeAndBind(0);
			activeShaderProgram.setUniform("material.diffuseMap", 0);
			activeShaderProgram.setUniform("material.specularMap", 1);
		}
		else
		{
			activeShaderProgram.setUniform("material.color", renderComp.material.color);
		}

		glBindVertexArray(renderComp.mesh->vao);
		glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(renderComp.mesh->indices.size()), GL_UNSIGNED_SHORT, nullptr);
	}
	checkGlError();

	m_gBuffer.unbind();
}

void RenderSystem::lightPass()
{
	const auto& cameraVpInv = inverse(g_camera.getProjectionMatrix() * g_camera.getViewMatrix());

	m_gBuffer.textures[GBuffer::TextureType::Specular].activeAndBind(1);
	m_gBuffer.textures[GBuffer::TextureType::Normal].activeAndBind(2);
	m_gBuffer.textures[GBuffer::TextureType::Depth].activeAndBind(3);

	m_lightingRT.bind();

	glDisable(GL_DEPTH_TEST);
	glViewport(0, 0, m_lightingRT.width, m_lightingRT.height);
	glClear(GL_COLOR_BUFFER_BIT);

	glDepthMask(false);

	glBlendFunc(GL_ONE, GL_ONE);

	// Ambient light
	if (m_ambientLight.isOn)
	{
		ShaderProgram& shaderProgram = g_shaderManager.getShaderProgram("ambientLight");

		shaderProgram.use();
		shaderProgram.setUniform("light.coloredIntensity", m_ambientLight.color.toNormalizedRGB() * m_ambientLight.intensity);

		drawFullscreenQuad();
	}

	// Directional lights
	ShaderProgram& directionalShadowMapProgram = g_shaderManager.getShaderProgram("shadowMap");
	ShaderProgram& directionalLightProgram = g_shaderManager.getShaderProgram("directionalLight");
	for (const auto& light : m_directionalLights)
	{
		if (light.isOn)
		{
			if (light.castsShadow)
			{
				m_lightingRT.unbind();

				// Shadow map
				Matrix4x4f projection = ortho(-50, 50, -50, 50, -50, 50);
				Vector3f lightDirInv = -light.direction;
				Matrix4x4f view = Camera::lookAt(lightDirInv, Vector3f::Zero, Vector3f::Up);

				m_dirLightDepthRT.bind();
				directionalShadowMapProgram.use();

				glDepthMask(true);
				glClear(GL_DEPTH_BUFFER_BIT);
				glEnable(GL_DEPTH_TEST);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				glCullFace(GL_FRONT);
				glViewport(0, 0, m_dirLightDepthRT.width, m_dirLightDepthRT.height);

				TransformComponent transformComp;
				RenderComponent renderComp;

				for (Entity& entity : g_world.entityManager.entities_with_components(transformComp, renderComp))
				{
					Matrix4x4f model = Matrix4x4f::translate(transformComp.position) * quaternionToMatrix4x4f(transformComp.orientation);
					Matrix4x4f mvp = projection * view * model;
					directionalShadowMapProgram.setUniform("mvp", mvp);

					glBindVertexArray(renderComp.mesh->vao);
					glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(renderComp.mesh->indices.size()), GL_UNSIGNED_SHORT, nullptr);
				}

				// Lighting
				glBlendFunc(GL_ONE, GL_ONE);
				glDisable(GL_DEPTH_TEST);
				glViewport(0, 0, m_lightingRT.width, m_lightingRT.height);
				glDepthMask(false);
				glCullFace(GL_BACK);

				m_lightingRT.bind();

				directionalLightProgram.use();
				m_dirLightDepthRT.depthTexture.activeAndBind(4);
				directionalLightProgram.setUniform("shadow", 4);

				directionalLightProgram.setUniform("cameraVpInv", cameraVpInv);
				directionalLightProgram.setUniform("shadowVp", projection * view);
			}
			else
			{
				criticalError("gg");
				// TODO: different shader?
			}

			directionalLightProgram.setUniform("specular", 1);
			directionalLightProgram.setUniform("normal", 2);
			directionalLightProgram.setUniform("depth", 3);

			directionalLightProgram.setUniform("light.base.coloredIntensity", light.color.toNormalizedRGB() * light.intensity);
			directionalLightProgram.setUniform("light.direction", light.direction);

			drawFullscreenQuad();
		}
	}

	// Point lights
	ShaderProgram& pointLightProgram = g_shaderManager.getShaderProgram("pointLight");
	pointLightProgram.use();
	pointLightProgram.setUniform("specular", 1);
	pointLightProgram.setUniform("normal", 2);
	pointLightProgram.setUniform("depth", 3);
	for (const auto& light : m_pointLights)
	{
		if (light.isOn)
		{
			pointLightProgram.setUniform("light.base.coloredIntensity", light.color.toNormalizedRGB() * light.intensity);
			pointLightProgram.setUniform("light.position", light.position);
			pointLightProgram.setUniform("light.attenuation.constant", light.attenuation.constant);
			pointLightProgram.setUniform("light.attenuation.linear", light.attenuation.linear);
			pointLightProgram.setUniform("light.attenuation.quadratic", light.attenuation.quadratic);
			pointLightProgram.setUniform("light.attenuation.range", light.attenuation.range);
			pointLightProgram.setUniform("cameraVpInv", cameraVpInv);

			drawFullscreenQuad();
		}
	}

	// Spot lights
	ShaderProgram& spotLightProgram = g_shaderManager.getShaderProgram("spotLight");
	spotLightProgram.use();
	spotLightProgram.setUniform("specular", 1);
	spotLightProgram.setUniform("normal", 2);
	spotLightProgram.setUniform("depth", 3);
	for (const auto& light : m_spotLights)
	{
		if (light.isOn)
		{
			spotLightProgram.setUniform("light.base.base.coloredIntensity", light.color.toNormalizedRGB() * light.intensity);
			spotLightProgram.setUniform("light.base.position", light.position);
			spotLightProgram.setUniform("light.base.attenuation.constant", light.attenuation.constant);
			spotLightProgram.setUniform("light.base.attenuation.linear", light.attenuation.linear);
			spotLightProgram.setUniform("light.base.attenuation.quadratic", light.attenuation.quadratic);
			spotLightProgram.setUniform("light.base.attenuation.range", light.attenuation.range);
			spotLightProgram.setUniform("light.direction", light.direction);
			spotLightProgram.setUniform("light.coneAngle", light.coneAngle);
			spotLightProgram.setUniform("cameraVpInv", cameraVpInv);

			drawFullscreenQuad();
		}
	}
	spotLightProgram.stopUsing();

	m_lightingRT.unbind();
}

void RenderSystem::outPass()
{
	m_outRT.bind();

	glDisable(GL_DEPTH_TEST);
	glViewport(0, 0, m_outRT.width, m_outRT.height);
	glClear(GL_COLOR_BUFFER_BIT);

	ShaderProgram& program = g_shaderManager.getShaderProgram("out");
	program.use();

	m_gBuffer.textures[GBuffer::TextureType::Albedo].activeAndBind(0);
	m_lightingRT.colorTexture.activeAndBind(1);
	program.setUniform("diffuse", 0);
	program.setUniform("lighting", 1);

	drawFullscreenQuad();

	// Text rendering
	ShaderProgram& fontShaderProgram = g_shaderManager.getShaderProgram("font");
	fontShaderProgram.use();

	glDepthMask(GL_FALSE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBindVertexArray(m_fontVao);
	glBindBuffer(GL_ARRAY_BUFFER, m_fontVbo);
	m_fontTexture.activeAndBind(0);
	fontShaderProgram.setUniform("tex", 0);

	for (auto& text : m_textsToRender)
	{
		g_fontManager.renderText(text.text, text.position.x, text.position.y);
	}
	m_textsToRender.clear();

	glDepthMask(GL_TRUE);

	m_outRT.unbind();
}

Texture& RenderSystem::getOutColorTexture()
{
	return m_outRT.colorTexture;
}

}