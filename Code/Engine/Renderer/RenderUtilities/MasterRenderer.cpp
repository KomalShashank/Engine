#include "Engine/Renderer/OpenGL/OpenGLExtensions.hpp"
#include "Engine/Renderer/RenderUtilities/MasterRenderer.hpp"



MasterRenderer::MasterRenderer()
{
	BlendPixels(SOURCE_ALPHA, SOURCE_ALPHA_INVERSE);
	SetLineWidth(1.5f);
}



MasterRenderer::~MasterRenderer()
{
	Texture::DestroyAllTextures();

	MonospaceFont::DestroyAllMonospaceFonts();
	ProportionalFont::DestroyAllProportionalFonts();
}



void MasterRenderer::ClearScreen(const RGBA& clearColor /*= RGBA::WHITE*/)
{
	glClearColor(clearColor.m_Red, clearColor.m_Green, clearColor.m_Blue, clearColor.m_Alpha);
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}



void MasterRenderer::SetProjectionViewport(const IntVector2& bottomLeft, const IntVector2& viewSize)
{
	glViewport(bottomLeft.X, bottomLeft.Y, viewSize.X, viewSize.Y);
}



void MasterRenderer::BlendPixels(unsigned int sourceBlendingMode, unsigned int destinationBlendingMode)
{
	glEnable(GL_BLEND);
	glBlendFunc(sourceBlendingMode, destinationBlendingMode);
}



void MasterRenderer::SetScissorViewport(const IntVector2& bottomLeft, const IntVector2& viewSize)
{
	glScissor(bottomLeft.X, bottomLeft.Y, viewSize.X, viewSize.Y);
}



void MasterRenderer::EnableScissorTesting(bool enabled)
{
	(enabled) ? glEnable(GL_SCISSOR_TEST) : glDisable(GL_SCISSOR_TEST);
}



void MasterRenderer::EnableBackFaceCulling(bool enabled)
{
	(enabled) ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);
}



void MasterRenderer::EnableDepthTesting(bool enabled)
{
	(enabled) ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
}



void MasterRenderer::EnableDepthWriting(bool enabled)
{
	(enabled) ? glDepthMask(GL_TRUE) : glDepthMask(GL_FALSE);
}



void MasterRenderer::EnableClipping(bool enabled, size_t clipIndex)
{
	(enabled) ? glEnable(GL_CLIP_DISTANCE0 + clipIndex) : glDisable(GL_CLIP_DISTANCE0 + clipIndex);
}



void MasterRenderer::SetPointSize(float pointSize)
{
	glPointSize(pointSize);
}



void MasterRenderer::SetLineWidth(float lineWidth)
{
	glLineWidth(lineWidth);
	glEnable(GL_LINE_SMOOTH);
}