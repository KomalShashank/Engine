#pragma once

#include <set>
#include "Engine/Renderer/SpriteRendering/Sprite.hpp"
#include "Engine/Renderer/ParticleSystem/ParticleSystem.hpp"



#define PARTICLE_LAYER		0xFF



class SpriteLayer
{
public:
	SpriteLayer(uint8_t layerID);

	void AddSprite(Sprite* currentSprite);
	void RemoveSprite(Sprite* currentSprite);

	void RegisterParticleSystem(ParticleSystem* particleSystem);

	void SetLayerEnabled(bool enabled);
	bool IsLayerEnabled() const;
	uint8_t GetLayerID() const;

	void RenderAllSprites() const;

	void UpdateAllParticleSystems(float deltaTimeInSeconds);
	void DestroyTerminatedParticleSystems();
	void RenderAllParticleSystems() const;

private:
	uint8_t m_ID;
	bool m_IsEnabled;

	std::set<Sprite*> m_Sprites;
	std::set<ParticleSystem*> m_ParticleSystems;
};