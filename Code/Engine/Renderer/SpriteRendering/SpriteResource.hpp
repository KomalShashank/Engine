#pragma once

#include "Engine/Math/VectorMath/2D/Vector2.hpp"
#include "Engine/Math/VectorMath/2D/AABB2.hpp"
#include "Engine/Renderer/MeshAndMaterial/Material.hpp"



class SpriteResource
{
public:
	SpriteResource();
	SpriteResource(const SpriteResource&) = delete;

public:
	const char* m_ID;

	Material* m_Material;
	Vector2 m_Dimensions;
	Vector2 m_Pivot;

	Texture* m_Texture;
	AABB2 m_TextureCoordinates;
};