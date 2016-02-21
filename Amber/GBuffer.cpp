#include "GBuffer.h"
#include "OpenGL.h"

#include <vector>

GBuffer::GBuffer()
{
}

GBuffer::~GBuffer()
{
}

bool GBuffer::create(int w, int h)
{
	width = w;
	height = h;

	glGenBuffers(1, &frambufferHandle);
	glBindFramebuffer(GL_FRAMEBUFFER, frambufferHandle);

	glEnable(GL_TEXTURE_2D);

	std::vector<GLenum> drawBuffers;
	
	auto addRenderTarget = [w, h, &drawBuffers](Texture& texture, GLenum attachment, GLint internalFormat, GLenum format, GLenum type)
	{
		texture.genAndBind(w, h);
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, w, h, 0, format, type, nullptr);
		texture.setFilterAndWrap();

		glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, texture.textureID, 0);

		if (attachment != GL_DEPTH_ATTACHMENT)
			drawBuffers.push_back(attachment);
	};

	addRenderTarget(textures[TextureType::Albedo], GL_COLOR_ATTACHMENT0, GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE);
	addRenderTarget(textures[TextureType::Specular], GL_COLOR_ATTACHMENT1, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE);
	addRenderTarget(textures[TextureType::Normal], GL_COLOR_ATTACHMENT2, GL_RGB10_A2, GL_RGBA, GL_FLOAT);
	addRenderTarget(textures[TextureType::Depth], GL_DEPTH_ATTACHMENT, GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_FLOAT);

	glDrawBuffers(static_cast<GLsizei>(drawBuffers.size()), &drawBuffers[0]);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		return false;

	return true;
}

void GBuffer::destroy()
{
	for (int i = 0; i < TextureType::Count; i++)
		textures[i].destroy();

	glDeleteFramebuffers(1, &frambufferHandle);
}

void GBuffer::bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, frambufferHandle);
}

void GBuffer::unbind()
{
	glFlush();
	glBindFramebuffer(GL_FRAMEBUFFER, frambufferHandle);
}
