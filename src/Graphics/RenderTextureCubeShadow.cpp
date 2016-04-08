#include "Graphics/RenderTextureCubeShadow.h"
#include "Window/Window.h"

namespace Amber
{

void RenderTextureCubeShadow::create(int w, int h)
{
	width = w;
	height = h;

	depthTexture.genAndBind(width, height);
	depthTexture.setFormat(TextureInternalFormat::Depth32F, TextureFormat::DepthComponent, TextureType::Float);
	depthTexture.setFilterAndWrap();

	shadowTexture.genAndBind(width, height);
	shadowTexture.setFilterAndWrap();

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

	for (int i = 0; i < 6; i++) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_R32F, width, height, 0, GL_RED, GL_FLOAT, nullptr);
	}

	glGenFramebuffers(1, &framebufferHandle);
	glBindFramebuffer(GL_FRAMEBUFFER, framebufferHandle);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture.textureHandle, 0);

	GLenum attachments[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, attachments);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		criticalError("FrameBuffer is not complete");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void RenderTextureCubeShadow::destroy()
{
	depthTexture.destroy();
	shadowTexture.destroy();

	if (framebufferHandle != -1)
		glDeleteFramebuffers(1, &framebufferHandle);
}

void RenderTextureCubeShadow::bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, framebufferHandle);
}

void RenderTextureCubeShadow::unbind()
{
	glFlush();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

float RenderTextureCubeShadow::ratio() const
{
	return static_cast<float>(height) / width;
}

}
