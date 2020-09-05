#pragma once



class RenderBuffer
{
public:
	RenderBuffer();
	RenderBuffer(unsigned int bufferType, const void* bufferData, size_t elementCount, size_t elementSize, unsigned int usageMode);
	~RenderBuffer();

	unsigned int CreateRenderBuffer(unsigned int bufferType, const void* bufferData, size_t elementCount, size_t elementSize, unsigned int usageMode);
	void DestroyRenderBuffer(unsigned int bufferID);

	void WriteToRenderBuffer(unsigned int bufferType, const void* bufferData, size_t elementCount, size_t elementSize, unsigned int usageMode);

	unsigned int GetBufferType() const;
	unsigned int GetRenderBufferID() const;
	unsigned int GetElementCount() const;
	unsigned int GetElementSize() const;
	unsigned int GetRenderBufferSize() const;
	unsigned int GetUsageMode() const;

private:
	void UpdateRenderBufferAttributes(unsigned int bufferType, size_t elementCount, size_t elementSize, unsigned int usageMode);

private:
	unsigned int m_BufferType;
	unsigned int m_RenderBufferID;
	unsigned int m_ElementCount;
	unsigned int m_ElementSize;
	unsigned int m_UsageMode;
};