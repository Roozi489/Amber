#include "RenderTexture.h"
#include "Utility.h"


RenderTexture::RenderTexture()
	: bufferID(-1)
{
}

bool RenderTexture::create(int width, int height, RenderTextureType type, TextureFilter minMag, TextureWrapMode wrap)
{
	glEnable(GL_DEPTH_TEST);
	glGenFramebuffers(1, &bufferID);
	glBindFramebuffer(GL_FRAMEBUFFER, bufferID);

	if (type & RenderTextureType::Color)
	{
		colorTexture.genAndBind(width, height);

		// empty texture
		if (type & RenderTextureType::Lighting)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB10_A2, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
		else
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

		colorTexture.setFilterAndWrap(minMag, wrap);

		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, colorTexture.textureID, 0);
	}
	
	if (type & RenderTextureType::Depth)
	{
		depthTexture.genAndBind(width, height);
		// empty texture
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
		depthTexture.setFilterAndWrap(minMag, wrap);
		// not sure what this does
		//glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);

		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture.textureID, 0);
	}

	if (type & RenderTextureType::Color || type & RenderTextureType::Lighting)
		drawbufferAttachments.push_back(GL_COLOR_ATTACHMENT0);

	glDrawBuffers(static_cast<GLsizei>(drawbufferAttachments.size()), &drawbufferAttachments[0]);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		criticalError("FrameBuffer is not complete");

	// unbind
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	checkGlError();

	return true;
}

void RenderTexture::destroy()
{
	if (bufferID != -1)
		glDeleteFramebuffers(1, &bufferID);
	colorTexture.destroy();
	depthTexture.destroy();
}

void RenderTexture::bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, bufferID);
}

void RenderTexture::unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}