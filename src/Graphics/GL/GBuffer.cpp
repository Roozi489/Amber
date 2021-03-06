#include "Graphics/GL/GBuffer.h"
#include "Graphics/GL/OpenGL.h"
#include "Window/Window.h"

#include <vector>

namespace Amber
{

GBuffer::GBuffer()
{
}

GBuffer::~GBuffer()
{
}

void GBuffer::create(int w, int h)
{
	width = w;
	height = h;

	glGenFramebuffers(1, &framebufferHandle);
	glBindFramebuffer(GL_FRAMEBUFFER, framebufferHandle);

	std::vector<GLenum> drawBuffers;

	auto addRenderTarget = [w, h, &drawBuffers](Texture& texture, GLenum attachment, GLint internalFormat, GLenum format, GLenum type)
	{
		texture.genAndBind(w, h);
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, w, h, 0, format, type, nullptr);
		texture.setFilterAndWrap();

		glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, texture.textureHandle, 0);

		if (attachment != GL_DEPTH_ATTACHMENT)
			drawBuffers.push_back(attachment);
	};

	addRenderTarget(textures[TextureType::Albedo], GL_COLOR_ATTACHMENT0, GL_RGBA8, GL_RGB, GL_UNSIGNED_BYTE);
	addRenderTarget(textures[TextureType::Specular], GL_COLOR_ATTACHMENT1, GL_RGBA8, GL_RGB, GL_UNSIGNED_BYTE);
	addRenderTarget(textures[TextureType::Normal], GL_COLOR_ATTACHMENT2, GL_RGB10_A2, GL_RGBA, GL_FLOAT);
	addRenderTarget(textures[TextureType::Depth], GL_DEPTH_ATTACHMENT, GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_FLOAT);

	glDrawBuffers(static_cast<GLsizei>(drawBuffers.size()), drawBuffers.data());
	checkGlError();

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		criticalError("Framebuffer is not complete.");
}

void GBuffer::destroy()
{
	for (int i = 0; i < TextureType::Count; i++)
		textures[i].destroy();

	glDeleteFramebuffers(1, &framebufferHandle);
}

void GBuffer::bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, framebufferHandle);
}

void GBuffer::unbind()
{
	glFlush();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

}