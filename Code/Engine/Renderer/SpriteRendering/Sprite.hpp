#pragma once

#include "Engine/Math/VectorMath/2D/Vector2.hpp"
#include "Engine/Renderer/MeshAndMaterial/Mesh.hpp"
#include "Engine/Renderer/MeshAndMaterial/Material.hpp"
#include "Engine/Renderer/SpriteRendering/SpriteResource.hpp"



class Sprite
{
protected:
	Sprite();

public:
	Sprite(const char* spriteResourceID);
	~Sprite();

	void SetSpriteScale(const Vector2& spriteScale);
	Vector2 GetSpriteScale() const;

	void SetSpriteRotation(float spriteRotation);
	float GetSpriteRotation() const;

	void SetSpritePosition(const Vector2& spritePosition);
	Vector2 GetSpritePosition() const;

	AABB2 GetSpriteWorldBounds() const;
	Vector2 GetSpriteDimensions() const;

	void SetLayerID(uint8_t layerID);
	uint8_t GetLayerID() const;

	virtual void EnableSprite();
	virtual void DisableSprite();
	bool IsSpriteEnabled() const;

	void SetMaterial(Material* spriteMaterial);

	void Render() const;

private:
	void UpdateSpriteMesh() const;

protected:
	Vector2 m_Scale;
	float m_Rotation;
	Vector2 m_Position;

	Mesh* m_Mesh;
	Material* m_Material;
	uint8_t m_LayerID;
	bool m_IsEnabled;

	const SpriteResource* m_SpriteResource;
};