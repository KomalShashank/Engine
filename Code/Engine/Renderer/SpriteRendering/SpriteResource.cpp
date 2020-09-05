#include "Engine/Renderer/SpriteRendering/SpriteResource.hpp"



SpriteResource::SpriteResource() :
m_ID("DefaultNullSpriteResource"),
m_Material(nullptr),
m_Dimensions(Vector2::ONE),
m_Pivot(Vector2::ONE * 0.5f),
m_Texture(nullptr),
m_TextureCoordinates(AABB2::UNIT_AABB2)
{

}