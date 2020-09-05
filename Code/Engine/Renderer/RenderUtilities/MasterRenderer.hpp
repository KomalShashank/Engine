#pragma once

#include <vector>
#include <string>

#include "Engine/Math/VectorMath/2D/Vector2.hpp"
#include "Engine/Math/VectorMath/2D/AABB2.hpp"
#include "Engine/Math/VectorMath/3D/Vector3.hpp"
#include "Engine/Math/VectorMath/3D/AABB3.hpp"
#include "Engine/Renderer/BitmapFonts/BitmapFont.hpp"
#include "Engine/Renderer/Color/RGBA.hpp"
#include "Engine/Renderer/RenderUtilities/RenderConstants.hpp"
#include "Engine/Renderer/Texture/Texture.hpp"
#include "Engine/Renderer/Vertex/Vertex.hpp"



class MasterRenderer
{
public:
	MasterRenderer();
	virtual ~MasterRenderer();

	virtual void ClearScreen(const RGBA& clearColor = RGBA::WHITE);
	virtual void SetProjectionViewport(const IntVector2& bottomLeft, const IntVector2& viewSize);
	virtual void BlendPixels(unsigned int sourceBlendingMode, unsigned int destinationBlendingMode);

	virtual void SetScissorViewport(const IntVector2& bottomLeft, const IntVector2& viewSize);
	virtual void EnableScissorTesting(bool enabled);

	virtual void EnableBackFaceCulling(bool enabled);
	virtual void EnableDepthTesting(bool enabled);
	virtual void EnableDepthWriting(bool enabled);
	virtual void EnableClipping(bool enabled, size_t clipIndex);

	virtual void SetPointSize(float pointSize);
	virtual void SetLineWidth(float lineWidth);
};