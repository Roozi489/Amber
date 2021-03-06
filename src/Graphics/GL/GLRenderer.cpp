#include "Graphics/GL/GLRenderer.h"
#include "Graphics/GL/OpenGL.h"
#include "Graphics/GL/ShaderProgram.h"
#include "Components/RenderComponent.h"
#include "Math/Frustum.h"
#include "Core/Globals.h"
#include "Core/Utility.h"

namespace Amber
{

GLRenderer::GLRenderer()
	: Renderer(GraphicsApi::OpenGL)
{
}

GLRenderer::~GLRenderer()
{
}

void GLRenderer::init()
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

	// Shadow maps
	// TODO: settings for shadow map size
	m_dirLightShadowRT.create(1024, 1024, RenderTexture::Shadow);
	m_spotLightShadowRT.create(1024, 1024, RenderTexture::Shadow);
	m_pointLightShadowRT.create(1024, 1024);

	// Shaders
	ShaderProgram* shaderProgram = g_shaderManager.createProgram("geometry", "geometry.vert", "geometry.frag");
	shaderProgram->use();
	shaderProgram->setUniform("material.diffuseMap", GBuffer::TextureType::Albedo);
	//shaderProgram->setUniform("material.specularMap", GBuffer::TextureType::Specular);
	shaderProgram = g_shaderManager.createProgram("geometryInstanced", "geometryInstanced.vert", "geometryInstanced.frag");
	shaderProgram->use();
	shaderProgram->setUniform("material.diffuseMap", GBuffer::TextureType::Albedo);
	//shaderProgram->setUniform("material.specularMap", GBuffer::TextureType::Specular);
	shaderProgram = g_shaderManager.createProgram("ambientLight", "fullscreenQuad.vert", "ambientLight.frag");
	shaderProgram = g_shaderManager.createProgram("directionalLight", "fullscreenQuad.vert", "directionalLight.frag");
	shaderProgram->use();
	shaderProgram->setUniform("normal", GBuffer::TextureType::Normal);
	shaderProgram = g_shaderManager.createProgram("directionalLightShadow", "fullscreenQuad.vert", "directionalLightShadow.frag");
	shaderProgram->use();
	shaderProgram->setUniform("normal", GBuffer::TextureType::Normal);
	shaderProgram->setUniform("depth", GBuffer::TextureType::Depth);
	shaderProgram->setUniform("shadow", 4);
	shaderProgram = g_shaderManager.createProgram("pointLight", "fullscreenQuad.vert", "pointLight.frag");
	shaderProgram->use();
	//shaderProgram->setUniform("specular", GBuffer::TextureType::Specular);
	shaderProgram->setUniform("normal", GBuffer::TextureType::Normal);
	shaderProgram->setUniform("depth", GBuffer::TextureType::Depth);
	shaderProgram = g_shaderManager.createProgram("pointLightShadow", "fullscreenQuad.vert", "pointLightShadow.frag");
	shaderProgram->use();
	shaderProgram->setUniform("normal", GBuffer::TextureType::Normal);
	shaderProgram->setUniform("depth", GBuffer::TextureType::Depth);
	shaderProgram->setUniform("shadow", 4);
	shaderProgram = g_shaderManager.createProgram("spotLight", "fullscreenQuad.vert", "spotLight.frag");
	shaderProgram->use();
	//shaderProgram->setUniform("specular", GBuffer::TextureType::Specular);
	shaderProgram->setUniform("normal", GBuffer::TextureType::Normal);
	shaderProgram->setUniform("depth", GBuffer::TextureType::Depth);
	shaderProgram = g_shaderManager.createProgram("spotLightShadow", "fullscreenQuad.vert", "spotLightShadow.frag");
	shaderProgram->use();
	shaderProgram->setUniform("normal", GBuffer::TextureType::Normal);
	shaderProgram->setUniform("depth", GBuffer::TextureType::Depth);
	shaderProgram->setUniform("shadow", 4);
	shaderProgram = g_shaderManager.createProgram("shadowMap", "shadowMap.vert");
	shaderProgram = g_shaderManager.createProgram("shadowMapCube", "shadowMapCube.vert", "shadowMapCube.frag");
	shaderProgram = g_shaderManager.createProgram("shadowMapArray", "shadowMapArray.vert", "shadowMapArray.geom", "shadowMapArray.frag");
	shaderProgram = g_shaderManager.createProgram("out", "fullscreenQuad.vert", "out.frag");
	shaderProgram = g_shaderManager.createProgram("texPass", "fullscreenQuad.vert", "texPass.frag");
	shaderProgram = g_shaderManager.createProgram("font", "font.vert", "font.frag");

	g_fontManager.init();

	//m_skybox.init();

	// Lights
	m_ambientLight.color = Color::White;
	m_ambientLight.intensity = 0.02f;

	DirectionalLight dirLight;
	//dirLight.castsShadow = false;
	dirLight.color = Color::White;
	dirLight.intensity = 0.2f;
	dirLight.direction = normalize(Vector3f(0.8f, -1.f, 0.2f));
	//m_directionalLights.push_back(dirLight);

	PointLight pointLight;
	//pointLight.castsShadow = false;
	pointLight.intensity = 300.f;
	pointLight.color = Color::fromByteRGB(255, 255, 100);
	pointLight.position = Vector3f(0.f, 12.f, 10.f);
	pointLight.attenuation.range = 30.f;
	m_pointLights.push_back(pointLight);

	SpotLight spotLight;
	//spotLight.castsShadow = false;
	spotLight.intensity = 500.f;
	spotLight.color = Color::fromByteRGB(120, 255, 120);
	spotLight.position = Vector3f(-20.f, 20.f, 1.f);
	spotLight.direction = normalize(Vector3f(1.8f, -1.5f, -0.2f));
	spotLight.coneAngle = toRadians(34.f);
	//m_spotLights.push_back(spotLight);

	// Meshes
	auto fullscreenQuadMesh = std::make_unique<Mesh>();
	fullscreenQuadMesh->meshComponents = MeshComponents::VertexIndex;
	fullscreenQuadMesh->name = "fullscreenQuad";
	fullscreenQuadMesh->vertices = { { -1.0f, -1.0f, 0.0f },{ 1.0f, -1.0f, 0.0f },{ 1.0f, 1.0f, 0.0f },{ -1.0f, 1.0f, 0.0f } };
	fullscreenQuadMesh->indices = { 0, 1, 2, 2, 3, 0 };
	fullscreenQuadMesh->setVaoAndVbo();
	g_meshManager.meshes.insert(std::make_pair(fullscreenQuadMesh->name, std::move(fullscreenQuadMesh)));

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

void GLRenderer::handleWindowSizeChange(int newWidth, int newHeight)
{
	Renderer::handleWindowSizeChange(newWidth, newHeight);
}

void GLRenderer::renderAll(Time delta)
{

	displayText(stringFormat("%5.3f ms", g_frameTime.asMilliseconds()), { 0.01f, 0.02f });
	displayText(stringFormat("%d fps", m_fps), { 0.01f, 0.04f });
	displayText(stringFormat("Culled objects: %d", m_culledObjectsCount), { 0.01f, 0.06f });

	geometryPass();
	lightPass();
	outPass();

	m_fpsCounter++;
	m_timeSinceFpsUpdate += delta;
	if (m_timeSinceFpsUpdate >= m_fpsUpdateInterval)
	{
		m_fps = m_fpsCounter * 2;
		m_fpsCounter = 0;
		m_timeSinceFpsUpdate -= m_fpsUpdateInterval;
	}
}

void GLRenderer::finalRenderToMainFramebuffer()
{
	glViewport(0, 0, outWidth, outHeight);
	glClear(GL_COLOR_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);

	ShaderProgram* program = g_shaderManager.getShaderProgram("texPass");
	program->use();
	m_outRT.colorTexture.activeAndBind(0);
	program->setUniform("tex", 0);
	drawFullscreenQuad();
	program->stopUsing();
}

void GLRenderer::displayText(const std::string& text, Vector2f position, FontID fontId)
{
	m_textsToRender.emplace_back(text, position, fontId);
}

void GLRenderer::drawFullscreenQuad()
{
	static Mesh* quad = g_meshManager.getMesh("fullscreenQuad");
	glBindVertexArray(quad->vaoHandle);
	glDrawElements(GL_TRIANGLES, static_cast<int>(quad->indices.size()), GL_UNSIGNED_SHORT, nullptr);
}

void GLRenderer::geometryPass()
{
	/* TODO: sorting
	2) Run through the list of game objects, frustum/visibility cull each one, and if visible add its meshes (one mesh per material) to a render list. Generally you want two render lists, one for opaque and one for transparent objects.
	3) Sort the render list(s) into order. The exact ordering is up to you, but you generally want to at least sort by material/texture/shader for the opaque list, or by depth for the transparent list.
	4) Run through the sorted render list and do the actual OpenGL calls for each mesh. Only change materials as needed.
	*/

	// TODO: better skybox
	//http://gamedev.stackexchange.com/questions/60313/implementing-a-m_skybox-with-glsl-version-330
	//m_skybox.update();
	//m_skybox.render();

	m_gBuffer.bind();

	const Matrix4x4f& vp = g_camera.getProjectionMatrix() * g_camera.getViewMatrix();
	Frustum cameraFrustum(g_camera.getPosition(), g_camera.forwardVector(), g_camera.getFieldOfViewX(), g_window.windowRatio(), g_camera.getNearPlane(), g_camera.getFarPlane());
	m_culledObjectsCount = 0;

	CommandQueue commandQueue;
	commandQueue.settings.viewportWidth = m_gBuffer.width;
	commandQueue.settings.viewportHeight = m_gBuffer.height;

	applyCommandQueueSettings(commandQueue.settings);

	TransformComponent transformComp;
	RenderComponent renderComp;

	for (const Entity& entity : g_world.entityManager.entities_with_components(transformComp, renderComp))
	{
		// TODO: scene graph that stores and updates aabb (or maybe spheres)
		if (m_frustumCullingEnabled && !cameraFrustum.intersectsSphere({ transformComp.position, renderComp.mesh->getBoundingSphereRadiusFast() }))
		{
			m_culledObjectsCount++;
			continue;
		}
		if (commandQueue.batches.find(renderComp.mesh->id) == commandQueue.batches.end())
		{
			Batch batch;
			batch.mesh = renderComp.mesh;
			batch.AddUniform({ "material.color", renderComp.material.color.rgba });
			commandQueue.batches.insert({ batch.mesh->id, batch });
		}
		Batch& batch = commandQueue.batches[renderComp.mesh->id];
		batch.count++;

		// TODO: add scale
		Matrix4x4f modelMatrix = Math::translate(transformComp.position) * transformComp.orientation.toMatrix();
		Matrix4x4f mvp = vp * modelMatrix;

		batch.mvps.push_back(mvp);
		batch.orientations.push_back(transformComp.orientation);
		batch.colors.push_back(renderComp.material.color);
	}

	for (auto& batch : commandQueue.batches)
	{
		if (batch.second.count > UseInstancingCountThreshold)
			batch.second.useInstancing = true;
	}

	// PERF: sort batches by _useInstaning_ and dont swap shaders
	for (const auto& batch : commandQueue.batches)
	{
		batch.second.mesh->texture->activeAndBind(0);
		if (batch.second.useInstancing)
		{
			ShaderProgram* activeShaderProgram = g_shaderManager.getShaderProgram("geometryInstanced");
			activeShaderProgram->use();

			setDynamicVBOs(batch.second);

			setUniforms(activeShaderProgram, batch.second.uniforms);



			glBindVertexArray(batch.second.mesh->vaoHandle);
			glDrawElementsInstanced(GL_TRIANGLES, static_cast<GLsizei>(batch.second.mesh->indices.size()), GL_UNSIGNED_SHORT, nullptr, batch.second.count);
		}
		else
		{
			ShaderProgram* activeShaderProgram = g_shaderManager.getShaderProgram("geometry");
			activeShaderProgram->use();
			for (int instance = 0; instance < batch.second.count; ++instance)
			{
				setUniforms(activeShaderProgram, batch.second.uniforms);
				activeShaderProgram->setUniform("mvp", batch.second.mvps[instance]);
				activeShaderProgram->setUniform("orientation", batch.second.orientations[instance]);
				activeShaderProgram->setUniform("material.color", batch.second.colors[instance]);

				glBindVertexArray(batch.second.mesh->vaoHandle);
				glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(batch.second.mesh->indices.size()), GL_UNSIGNED_SHORT, nullptr);
			}
		}
	}
	
	m_gBuffer.unbind();
}

void GLRenderer::lightPass()
{
	const float zNear = 0.01f;
	const auto& cameraVpInv = inverse(g_camera.getProjectionMatrix() * g_camera.getViewMatrix());

	m_gBuffer.textures[GBuffer::TextureType::Specular].activeAndBind(1);
	m_gBuffer.textures[GBuffer::TextureType::Normal].activeAndBind(2);
	m_gBuffer.textures[GBuffer::TextureType::Depth].activeAndBind(3);

	auto renderGeometry = [](const Matrix4x4f& view, const Matrix4x4f& projection, int viewportWidth, int viewportHeight, ShaderProgram* program)
	{
		glDepthMask(true);
		glClear(GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
		glCullFace(GL_FRONT);
		glViewport(0, 0, viewportWidth, viewportHeight);
		program->use();

		TransformComponent transformComp;
		RenderComponent renderComp;

		for (const Entity& entity : g_world.entityManager.entities_with_components(transformComp, renderComp))
		{
			Matrix4x4f model = Math::translate(transformComp.position) * transformComp.orientation.toMatrix();
			Matrix4x4f mvp = projection * view * model;
			program->setUniform("mvp", mvp);

			glBindVertexArray(renderComp.mesh->vaoHandle);
			glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(renderComp.mesh->indices.size()), GL_UNSIGNED_SHORT, nullptr);
		}
	};

	auto setupForLightingWithShadows = [&](const Matrix4x4f& view, const Matrix4x4f& projection, RenderTexture& shadowRT, ShaderProgram* program)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);
		glDisable(GL_DEPTH_TEST);
		glViewport(0, 0, m_lightingRT.width, m_lightingRT.height);
		glDepthMask(false);
		glCullFace(GL_BACK);

		m_lightingRT.bind();

		program->use();
		shadowRT.depthTexture.activeAndBind(4);

		program->setUniform("shadowVp", projection * view);
	};

	m_lightingRT.bind();

	glDisable(GL_DEPTH_TEST);
	glViewport(0, 0, m_lightingRT.width, m_lightingRT.height);
	glClear(GL_COLOR_BUFFER_BIT);
	glDepthMask(false);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);

	// Ambient light
	if (m_ambientLight.isOn)
	{
		static ShaderProgram* shaderProgram = g_shaderManager.getShaderProgram("ambientLight");

		shaderProgram->use();
		shaderProgram->setUniform("light.coloredIntensity", m_ambientLight.color.rgb * m_ambientLight.intensity);

		drawFullscreenQuad();
	}

	// Directional lights
	static ShaderProgram* directionalShadowMapProgram = g_shaderManager.getShaderProgram("shadowMap");
	ShaderProgram* directionalLightProgram;
	for (const auto& light : m_directionalLights)
	{
		if (light.isOn)
		{
			if (light.castsShadow)
			{
				Matrix4x4f view = Math::lookAt(-light.direction, Vector3f::Zero, Vector3f::Up);
				AABB worldAABB = g_world.computeAABB();
				// TODO: find out why it doesnt work with aabb values
				float maxDist = max(abs(worldAABB.left()), worldAABB.right(), abs(worldAABB.bottom()), worldAABB.top(), abs(worldAABB.back()), worldAABB.front());
				Matrix4x4f projection = Math::ortho(-maxDist, maxDist, -maxDist, maxDist, -maxDist, maxDist);

				m_dirLightShadowRT.bind();
				renderGeometry(view, projection, m_dirLightShadowRT.width, m_dirLightShadowRT.height, directionalShadowMapProgram);
				directionalLightProgram = g_shaderManager.getShaderProgram("directionalLightShadow");
				setupForLightingWithShadows(view, projection, m_dirLightShadowRT, directionalLightProgram);
				directionalLightProgram->setUniform("cameraVpInv", cameraVpInv);
			}
			else
			{
				directionalLightProgram = g_shaderManager.getShaderProgram("directionalLight");
				directionalLightProgram->use();
			}

			directionalLightProgram->setUniform("light.base.coloredIntensity", light.color.rgb * light.intensity);
			directionalLightProgram->setUniform("light.direction", light.direction);

			drawFullscreenQuad();
		}
	}

	// Point lights
	static ShaderProgram* pointShadowMapProgram = g_shaderManager.getShaderProgram("shadowMapCube");
	ShaderProgram* pointLightProgram;
	for (const auto& light : m_pointLights)
	{
		if (light.isOn)
		{
			if (light.castsShadow)
			{
				Matrix4x4f projection = Math::perspectiveFov(toRadians(90.f), m_pointLightShadowRT.ratio(), zNear, light.attenuation.range);

				m_pointLightShadowRT.bind();
				for (int i = 0; i < 6; ++i)
				{
					Matrix4x4f view = Math::lookAt(light.position, light.position + Camera::CameraDirections[i].dir, Camera::CameraDirections[i].up);
					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_pointLightShadowRT.shadowTexture.textureHandle, 0);

					glDepthMask(true);
					glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
					glEnable(GL_DEPTH_TEST);
					glDisable(GL_BLEND);
					glCullFace(GL_BACK);
					glViewport(0, 0, m_pointLightShadowRT.width, m_pointLightShadowRT.height);
					pointShadowMapProgram->use();
					pointShadowMapProgram->setUniform("lightPosition", light.position);

					TransformComponent transformComp;
					RenderComponent renderComp;

					for (const Entity& entity : g_world.entityManager.entities_with_components(transformComp, renderComp))
					{
						Matrix4x4f model = Math::translate(transformComp.position) * transformComp.orientation.toMatrix();
						Matrix4x4f mvp = projection * view * model;
						pointShadowMapProgram->setUniform("mvp", mvp);
						pointShadowMapProgram->setUniform("model", model);

						glBindVertexArray(renderComp.mesh->vaoHandle);
						glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(renderComp.mesh->indices.size()), GL_UNSIGNED_SHORT, nullptr);
					}
				}
				m_pointLightShadowRT.unbind();

				m_lightingRT.bind();
				glEnable(GL_BLEND);
				glBlendFunc(GL_ONE, GL_ONE);
				glDisable(GL_DEPTH_TEST);
				glViewport(0, 0, m_lightingRT.width, m_lightingRT.height);
				glDepthMask(false);
				glCullFace(GL_BACK);

				pointLightProgram = g_shaderManager.getShaderProgram("pointLightShadow");
				pointLightProgram->use();
				m_pointLightShadowRT.shadowTexture.activeAndBind(4);
			}
			else
			{
				pointLightProgram = g_shaderManager.getShaderProgram("pointLight");
				pointLightProgram->use();
			}

			pointLightProgram->setUniform("cameraVpInv", cameraVpInv);
			pointLightProgram->setUniform("cameraPosition", g_camera.getPosition());
			pointLightProgram->setUniform("light.base.coloredIntensity", light.color.rgb * light.intensity);
			pointLightProgram->setUniform("light.position", light.position);
			pointLightProgram->setUniform("light.attenuation.constant", light.attenuation.constant);
			pointLightProgram->setUniform("light.attenuation.linear", light.attenuation.linear);
			pointLightProgram->setUniform("light.attenuation.quadratic", light.attenuation.quadratic);
			pointLightProgram->setUniform("light.attenuation.range", light.attenuation.range);

			drawFullscreenQuad();
		}
	}

	// Spot lights
	static ShaderProgram* spotShadowMapProgram = g_shaderManager.getShaderProgram("shadowMap");
	ShaderProgram* spotLightProgram;
	for (const auto& light : m_spotLights)
	{
		if (light.isOn)
		{
			if (light.castsShadow)
			{
				Matrix4x4f view = Math::lookAt(light.position, light.position + light.direction, Vector3f::Up);
				Matrix4x4f projection = Math::perspectiveFov(2 * light.coneAngle, m_spotLightShadowRT.ratio(), zNear, light.attenuation.range);

				m_spotLightShadowRT.bind();
				renderGeometry(view, projection, m_spotLightShadowRT.width, m_spotLightShadowRT.height, spotShadowMapProgram);
				spotLightProgram = g_shaderManager.getShaderProgram("spotLightShadow");
				setupForLightingWithShadows(view, projection, m_spotLightShadowRT, spotLightProgram);
			}
			else
			{
				spotLightProgram = g_shaderManager.getShaderProgram("spotLight");
				spotLightProgram->use();
			}

			spotLightProgram->setUniform("cameraVpInv", cameraVpInv);
			spotLightProgram->setUniform("light.base.base.coloredIntensity", light.color.rgb * light.intensity);
			spotLightProgram->setUniform("light.base.position", light.position);
			spotLightProgram->setUniform("light.base.attenuation.constant", light.attenuation.constant);
			spotLightProgram->setUniform("light.base.attenuation.linear", light.attenuation.linear);
			spotLightProgram->setUniform("light.base.attenuation.quadratic", light.attenuation.quadratic);
			spotLightProgram->setUniform("light.base.attenuation.range", light.attenuation.range);
			spotLightProgram->setUniform("light.direction", light.direction);
			spotLightProgram->setUniform("light.coneAngle", light.coneAngle);

			drawFullscreenQuad();
		}
	}

	m_lightingRT.unbind();
}

void GLRenderer::outPass()
{
	m_outRT.bind();

	glDisable(GL_DEPTH_TEST);
	glViewport(0, 0, m_outRT.width, m_outRT.height);
	glClear(GL_COLOR_BUFFER_BIT);

	static ShaderProgram* program = g_shaderManager.getShaderProgram("out");
	program->use();

	m_gBuffer.textures[GBuffer::TextureType::Albedo].activeAndBind(0);
	m_lightingRT.colorTexture.activeAndBind(1);
	program->setUniform("diffuse", 0);
	program->setUniform("lighting", 1);

	drawFullscreenQuad();

	// Text rendering
	static ShaderProgram* fontShaderProgram = g_shaderManager.getShaderProgram("font");
	fontShaderProgram->use();

	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBindVertexArray(m_fontVao);
	glBindBuffer(GL_ARRAY_BUFFER, m_fontVbo);
	m_fontTexture.activeAndBind(0);
	fontShaderProgram->setUniform("tex", 0);

	for (auto& text : m_textsToRender)
	{
		g_fontManager.renderText(text.text, text.position.x, text.position.y);
	}
	m_textsToRender.clear();

	glDepthMask(GL_TRUE);

	m_outRT.unbind();
}

Texture& GLRenderer::getOutColorTexture()
{
	return m_outRT.colorTexture;
}

void GLRenderer::applyCommandQueueSettings(const CommandQueueSettings& settings)
{
	glDepthMask(settings.writeDepth);

	if (settings.depthTest)
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);

	if (settings.blending)
		glEnable(GL_BLEND);
	else
		glDisable(GL_BLEND);

	glBlendFunc(toUnderlyingType(settings.sourceBlendFactor), toUnderlyingType(settings.destinationBlendFactor));
	glCullFace(toUnderlyingType(settings.cullFace));
	glClear(toUnderlyingType(settings.clearBufferFlags));
	glViewport(0, 0, settings.viewportWidth, settings.viewportHeight);
}

void GLRenderer::setDynamicVBOs(const Batch& batch)
{
	// Orientation
	glBindBuffer(GL_ARRAY_BUFFER, batch.mesh->vboHandles[VBOIndex::Orientation]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Quaternion) * batch.count, batch.orientations.data(), GL_DYNAMIC_DRAW);
	// MVP
	glBindBuffer(GL_ARRAY_BUFFER, batch.mesh->vboHandles[VBOIndex::MVP]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Matrix4x4f) * batch.count, batch.mvps.data(), GL_DYNAMIC_DRAW);
	// Color
	glBindBuffer(GL_ARRAY_BUFFER, batch.mesh->vboHandles[VBOIndex::Color]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Color) * batch.count, batch.colors.data(), GL_DYNAMIC_DRAW);
};

void GLRenderer::setUniforms(ShaderProgram* program, const std::vector<UniformValue>& values)
{
	for (const auto& value : values)
	{
		switch (value.type)
		{
		case UniformType::Bool:
			program->setUniform(value.name, value.boolean);
			break;
		case UniformType::Int32:
			program->setUniform(value.name, value.integer);
			break;
		case UniformType::Float:
			program->setUniform(value.name, value.single);
			break;
		case UniformType::Vec2:
			program->setUniform(value.name, value.vec2);
			break;
		case UniformType::Vec3:
			program->setUniform(value.name, value.vec3);
			break;
		case UniformType::Vec4:
			program->setUniform(value.name, value.vec4);
			break;
		case UniformType::Mat3:
			program->setUniform(value.name, value.mat3);
			break;
		case UniformType::Mat4:
			program->setUniform(value.name, value.mat4);
			break;
		case UniformType::Sampler:
			program->setUniform(value.name, value.integer);
			break;
		default:
			criticalError("Unsupported uniform type.");
			break;
		}
	}
}

}
