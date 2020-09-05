#pragma once

#include <vector>
#include <set>

#include "Engine/Math/VectorMath/2D/IntVector2.hpp"
#include "Engine/Math/VectorMath/2D/AABB2.hpp"
#include "Engine/Renderer/MeshAndMaterial/Material.hpp"
#include "Engine/Renderer/SpriteRendering/Sprite.hpp"
#include "Engine/Renderer/SpriteRendering/AnimatedSprite.hpp"
#include "Engine/Renderer/SpriteRendering/SpriteLayer.hpp"
#include "Engine/Renderer/Color/RGBA.hpp"
#include "Engine/DeveloperConsole/Command.hpp"
#include "Engine/Time/Clock.hpp"



struct CompareLayerIDs
{
	bool operator()(SpriteLayer* firstLayer, SpriteLayer* secondLayer) const;
};



class SpriteRenderer
{
private:
	SpriteRenderer();
	SpriteRenderer(const SpriteRenderer&) = delete;
	~SpriteRenderer();

public:
	static void InitializeSpriteRenderer();
	static void UninitializeSpriteRenderer();

	static SpriteRenderer* SingletonInstance();

	void Update();
	void Render() const;

	void SetClearColor(const RGBA& clearColor);

	void SetVirtualSize(float virtualSize);
	float GetVirtualSize() const;

	void SetImportSize(float importSize);
	float GetImportSize() const;

	void SetPixelResolution(const IntVector2& pixelResolution);
	IntVector2 GetPixelResolution() const;

	void SetWorldBounds(const Vector2& worldMinimums, const Vector2& worldMaximums);
	AABB2 GetWorldBounds() const;

	void SetCameraPosition(const Vector2& cameraPosition);
	Vector2 GetCameraPosition() const;

	Vector2 GetScreenSize() const;
	AABB2 GetScreenBounds() const;
	
	float GetAspectRatio() const;
	float GetVirtualToImportRatio() const;

	Material* GetDefaultMaterial() const;

	void RegisterSprite(Sprite* currentSprite);
	void UnregisterSprite(Sprite* currentSprite);

	void RegisterAnimatedSprite(AnimatedSprite* currentAnimatedSprite);
	void UnregisterAnimatedSprite(AnimatedSprite* currentAnimatedSprite);

	void EnableSpriteLayer(uint8_t layerID);
	void DisableSpriteLayer(uint8_t layerID);

	ParticleSystem* CreateParticleSystem(const char* definitionID, const Vector2& spawnPosition);
	void DestroyParticleSystem(ParticleSystem* particleSystem);

	void PlayParticleSystem(const char* definitionID, const Vector2& spawnPosition);

private:
	SpriteLayer* CreateOrGetSpriteLayer(uint8_t layerID);
	
	void UpdateAllLayers(float deltaTimeInSeconds);
	void RenderAllLayers() const;

private:
	RGBA m_ClearColor;
	Clock* m_RendererClock;

	float m_VirtualSize;
	float m_ImportSize;

	IntVector2 m_PixelResolution;
	AABB2 m_WorldBounds;
	Vector2 m_CameraPosition;

	Material* m_DefaultMaterial;

	std::set<SpriteLayer*, CompareLayerIDs> m_SpriteLayers;
	std::set<AnimatedSprite*> m_AnimatedSprites;
};

void EnableLayerCommand(Command& currentCommand);
void DisableLayerCommand(Command& currentCommand);