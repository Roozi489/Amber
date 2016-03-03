#include "Graphics/RenderTexture.h"
#include "Window/Window.h"

namespace Amber
{

RenderTexture::RenderTexture()
{
}

void RenderTexture::create(int w, int h, RenderTextureType type, TextureFilter minMag, TextureWrapMode wrap)
{
	width = w;
	height = h;

	glGenFramebuffers(1, &framebufferHandle);
	glBindFramebuffer(GL_FRAMEBUFFER, framebufferHandle);

	if (type & RenderTextureType::Color)
	{
		colorTexture.genAndBind(width, height);

		if (type == RenderTextureType::Lighting)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB10_A2, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
		else
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

		colorTexture.setFilterAndWrap(minMag, wrap);

		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, colorTexture.textureHandle, 0);

		drawbufferAttachments.push_back(GL_COLOR_ATTACHMENT0);
	}

	if (type & RenderTextureType::Depth)
	{
		depthTexture.genAndBind(width, height);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
		depthTexture.setFilterAndWrap(minMag, wrap);

		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture.textureHandle, 0);
	}

	glDrawBuffers(static_cast<GLsizei>(drawbufferAttachments.size()), drawbufferAttachments.data());

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		criticalError("FrameBuffer is not complete");

	// unbind
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void RenderTexture::destroy()
{
	colorTexture.destroy();
	depthTexture.destroy();

	if (framebufferHandle != -1)
		glDeleteFramebuffers(1, &framebufferHandle);
}

void RenderTexture::bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, framebufferHandle);
}

void RenderTexture::unbind()
{
	glFlush();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

}