#include "Engine/Renderer/RenderUtilities/RenderBuffer.hpp"
#include "Engine/Renderer/OpenGL/OpenGLExtensions.hpp"



RenderBuffer::RenderBuffer() :
m_BufferType(NULL),
m_RenderBufferID(NULL),
m_ElementCount(NULL),
m_ElementSize(NULL),
m_UsageMode(NULL)
{

}



RenderBuffer::RenderBuffer(unsigned int bufferType, const void* bufferData, size_t elementCount, size_t elementSize, unsigned int usageMode) :
m_BufferType(bufferType),
m_ElementCount(elementCount),
m_ElementSize(elementSize),
m_UsageMode(usageMode)
{
	m_RenderBufferID = CreateRenderBuffer(bufferType, bufferData, elementCount, elementSize, usageMode);
}



RenderBuffer::~RenderBuffer()
{
	DestroyRenderBuffer(m_RenderBufferID);
}



unsigned int RenderBuffer::CreateRenderBuffer(unsigned int bufferType, const void* bufferData, size_t elementCount, size_t elementSize, unsigned int usageMode)
{
	unsigned int bufferID = NULL;

	glGenBuffers(1, &bufferID);
	glBindBuffer(bufferType, bufferID);
	glBufferData(bufferType, elementCount * elementSize, bufferData, usageMode);
	glBindBuffer(bufferType, NULL);

	return bufferID;
}



void RenderBuffer::DestroyRenderBuffer(unsigned int bufferID)
{
	glDeleteBuffers(1, &bufferID);
}



void RenderBuffer::WriteToRenderBuffer(unsigned int bufferType, const void* bufferData, size_t elementCount, size_t elementSize, unsigned int usageMode)
{
	UpdateRenderBufferAttributes(bufferType, elementCount, elementSize, usageMode);

	if (m_RenderBufferID == NULL)
	{
		m_RenderBufferID = CreateRenderBuffer(bufferType, bufferData, elementCount, elementSize, usageMode);
		return;
	}

	glBindBuffer(bufferType, m_RenderBufferID);
	glBufferData(bufferType, elementCount * elementSize, bufferData, usageMode);
	glBindBuffer(bufferType, NULL);
}



unsigned int RenderBuffer::GetBufferType() const
{
	return m_BufferType;
}



unsigned int RenderBuffer::GetRenderBufferID() const
{
	return m_RenderBufferID;
}



unsigned int RenderBuffer::GetElementCount() const
{
	return m_ElementCount;
}



unsigned int RenderBuffer::GetElementSize() const
{
	return m_ElementSize;
}



unsigned int RenderBuffer::GetRenderBufferSize() const
{
	return (m_ElementSize * m_ElementCount);
}



unsigned int RenderBuffer::GetUsageMode() const
{
	return m_UsageMode;
}



void RenderBuffer::UpdateRenderBufferAttributes(unsigned int bufferType, size_t elementCount, size_t elementSize, unsigned int usageMode)
{
	m_BufferType = bufferType;
	m_ElementCount = elementCount;
	m_ElementSize = elementSize;
	m_UsageMode = usageMode;
}