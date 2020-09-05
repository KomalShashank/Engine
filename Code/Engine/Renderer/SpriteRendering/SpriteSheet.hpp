#pragma once

#include "Engine/Renderer/Texture/Texture.hpp"
#include "Engine/Math/VectorMath/2D/AABB2.hpp"



class SpriteSheet
{
public:
	SpriteSheet(const char* imageFilePath, int tilesWide, int tilesHigh, const SamplerData& samplerData);

	AABB2 GetTextureCoordsForSpriteCoords(const IntVector2& spriteCoordinates) const;
	AABB2 GetTextureCoordsForSpriteIndex(int spriteIndex) const;
	int GetNumberOfSprites() const;
	Texture* GetSpriteSheet() const;

private:
	Texture* m_SpriteSheetTexture;
	IntVector2 m_SpriteLayout;
	Vector2 m_TexCoordsPerTile;
};