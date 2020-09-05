#pragma  once

#include <vector>
#include "Engine/Renderer/Texture/Texture.hpp"
#include "Engine/Math/VectorMath/2D/IntVector2.hpp"



class FrameBuffer
{
private:
	FrameBuffer(size_t numberOfColorTargets);
	~FrameBuffer();

public:
	static FrameBuffer* CreateFrameBuffer(const IntVector2& frameDimensions, size_t numberOfColorTargets = 1U);
	static void DestroyFrameBuffer(FrameBuffer* frameBuffer);

	uint32_t GetFrameBufferID() const;
	IntVector2 GetFrameDimensions() const;

	Texture** GetColorTargets() const;
	Texture* GetDepthStencil() const;

	size_t GetNumberOfColorTargets() const;

private:
	uint32_t m_FrameBufferObjectID;
	IntVector2 m_FrameDimensions;

	Texture** m_ColorTargets;
	Texture* m_DepthStencil;

	size_t m_NumberOfColorTargets;
};