#include "Engine/Renderer/SpriteRendering/SpriteSheet.hpp"



SpriteSheet::SpriteSheet(const char* imageFilePath, int tilesWide, int tilesHigh, const SamplerData& samplerData) :
m_SpriteSheetTexture(Texture::CreateOrGetTexture(imageFilePath, samplerData)),
m_SpriteLayout(tilesWide, tilesHigh),
m_TexCoordsPerTile(1.0f / static_cast<float>(tilesWide), 1.0f / static_cast<float>(tilesHigh))
{

}



AABB2 SpriteSheet::GetTextureCoordsForSpriteCoords(const IntVector2& spriteCoordinates) const
{
	AABB2 textureCoordinates;
	textureCoordinates.minimums.X = m_TexCoordsPerTile.X * static_cast<float>(spriteCoordinates.X);
	textureCoordinates.minimums.Y = m_TexCoordsPerTile.Y * static_cast<float>(spriteCoordinates.Y);
	textureCoordinates.maximums = textureCoordinates.minimums + m_TexCoordsPerTile;

	return textureCoordinates;
}



AABB2 SpriteSheet::GetTextureCoordsForSpriteIndex(int spriteIndex) const
{
	int spriteX = spriteIndex % m_SpriteLayout.X;
	int spriteY = spriteIndex / m_SpriteLayout.X;

	return GetTextureCoordsForSpriteCoords(IntVector2(spriteX, spriteY));
}



int SpriteSheet::GetNumberOfSprites() const
{
	return m_SpriteLayout.X * m_SpriteLayout.Y;
}



Texture* SpriteSheet::GetSpriteSheet() const
{
	return m_SpriteSheetTexture;
}