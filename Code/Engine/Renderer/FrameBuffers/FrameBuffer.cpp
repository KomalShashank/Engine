#include "Engine/Renderer/FrameBuffers/FrameBuffer.hpp"
#include "Engine/Renderer/OpenGL/OpenGLExtensions.hpp"
#include "Engine/Renderer/RenderUtilities/RenderConstants.hpp"
#include "Engine/Math/MatrixMath/Matrix4.hpp"



FrameBuffer::FrameBuffer(size_t numberOfColorTargets) :
m_FrameBufferObjectID(0U),
m_FrameDimensions(IntVector2::ZERO),
m_ColorTargets(nullptr),
m_DepthStencil(nullptr),
m_NumberOfColorTargets(0U)
{
	m_NumberOfColorTargets = (numberOfColorTargets > 0U) ? numberOfColorTargets : 1U;
	m_ColorTargets = new Texture*[m_NumberOfColorTargets];
}



FrameBuffer::~FrameBuffer()
{
	delete[] m_ColorTargets;
	glDeleteFramebuffers(1, &m_FrameBufferObjectID);
}



FrameBuffer* FrameBuffer::CreateFrameBuffer(const IntVector2& frameDimensions, size_t numberOfColorTargets /*= 1U*/)
{
	uint32_t frameBufferObjectID;
	glGenFramebuffers(1, &frameBufferObjectID);

	FrameBuffer* frameBuffer = new FrameBuffer(numberOfColorTargets);
	frameBuffer->m_FrameBufferObjectID = frameBufferObjectID;
	frameBuffer->m_FrameDimensions = frameDimensions;

	SamplerData samplerData = SamplerData(CLAMP_WRAP, CLAMP_WRAP, LINEAR_FILTER, NEAREST_FILTER);

	for (size_t colorTargetIndex = 0; colorTargetIndex < frameBuffer->m_NumberOfColorTargets; ++colorTargetIndex)
	{
		frameBuffer->m_ColorTargets[colorTargetIndex] = Texture::CreateDefaultTexture(frameDimensions, RGBA_8, samplerData);
	}
	frameBuffer->m_DepthStencil = Texture::CreateDefaultTexture(frameDimensions, D_24S_8, samplerData);

	glBindFramebuffer(GL_FRAMEBUFFER, frameBufferObjectID);

	for (size_t colorTargetIndex = 0; colorTargetIndex < frameBuffer->m_NumberOfColorTargets; ++colorTargetIndex)
	{
		uint32_t colorTargetTextureID = frameBuffer->m_ColorTargets[colorTargetIndex]->m_TextureID;
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + colorTargetIndex, colorTargetTextureID, 0);
	}

	uint32_t depthStencilTextureID = frameBuffer->m_DepthStencil->m_TextureID;
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, depthStencilTextureID, 0);

	uint32_t status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		delete frameBuffer;
		return nullptr;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, NULL);

	return frameBuffer;
}



void FrameBuffer::DestroyFrameBuffer(FrameBuffer* frameBuffer)
{
	delete frameBuffer;
}



uint32_t FrameBuffer::GetFrameBufferID() const
{
	return m_FrameBufferObjectID;
}



IntVector2 FrameBuffer::GetFrameDimensions() const
{
	return m_FrameDimensions;
}



Texture** FrameBuffer::GetColorTargets() const
{
	return m_ColorTargets;
}



Texture* FrameBuffer::GetDepthStencil() const
{
	return m_DepthStencil;
}



size_t FrameBuffer::GetNumberOfColorTargets() const
{
	return m_NumberOfColorTargets;
}