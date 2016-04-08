#include "Graphics/RenderTextureArrayShadow.h"
#include "Window/Window.h"


namespace Amber
{

void RenderTextureArrayShadow::create(int w, int h, TextureWrapMode wrap)
{
	width = w;
	height = h;

	glGenFramebuffers(1, &framebufferHandle);
	glBindFramebuffer(GL_FRAMEBUFFER, framebufferHandle);

	glGenTextures(1, &shadowTextureHandle);
	glBindTexture(GL_TEXTURE_2D_ARRAY, shadowTextureHandle);
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, static_cast<GLint>(TextureInternalFormat::R32F), width, height, 6, 0, static_cast<GLint>(TextureFormat::Red), GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, shadowTextureHandle, 0);

	glGenTextures(1, &depthTextureHandle);
	glBindTexture(GL_TEXTURE_2D_ARRAY, depthTextureHandle);
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, static_cast<GLint>(TextureInternalFormat::Depth32F), width, height, 6, 0, static_cast<GLint>(TextureFormat::DepthComponent), GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTextureHandle, 0);

	GLenum attachments[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, attachments);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		criticalError("FrameBuffer is not complete");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void RenderTextureArrayShadow::destroy()
{
	if (depthTextureHandle != -1)
		glDeleteTextures(1, &depthTextureHandle);
	if (shadowTextureHandle != -1)
		glDeleteTextures(1, &shadowTextureHandle);

	if (framebufferHandle != -1)
		glDeleteFramebuffers(1, &framebufferHandle);
}

void RenderTextureArrayShadow::bind()
{
	glBindBuffer(GL_FRAMEBUFFER, framebufferHandle);
}

void RenderTextureArrayShadow::unbind()
{
	glFlush();
	glBindBuffer(GL_FRAMEBUFFER, 0);
}

float RenderTextureArrayShadow::ratio() const
{
	return static_cast<float>(height) / width;
}

}
