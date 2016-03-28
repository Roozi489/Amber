#include "Graphics/RenderTexture.h"
#include "Window/Window.h"

namespace Amber
{

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
			colorTexture.setFormat(TextureInternalFormat::RGB10A2, TextureFormat::RGB, TextureType::UByte);
		else
			colorTexture.setFormat(TextureInternalFormat::RGB8, TextureFormat::RGB, TextureType::UByte);

		colorTexture.setFilterAndWrap(minMag, wrap);

		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, colorTexture.textureHandle, 0);

		drawbufferAttachments.push_back(GL_COLOR_ATTACHMENT0);
	}

	if (type & RenderTextureType::Depth)
	{
		depthTexture.genAndBind(width, height);
		depthTexture.setFormat(TextureInternalFormat::Depth32F, TextureFormat::DepthComponent, TextureType::Float);
		depthTexture.setFilterAndWrap(minMag, wrap);

		if (type == RenderTextureType::Shadow)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}

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

float RenderTexture::ratio() const
{
	return static_cast<float>(height) / width;
}

}
