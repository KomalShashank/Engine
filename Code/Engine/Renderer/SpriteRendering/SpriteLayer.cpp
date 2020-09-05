#include "Engine/Renderer/SpriteRendering/SpriteLayer.hpp"



SpriteLayer::SpriteLayer(uint8_t layerID) :
m_ID(layerID),
m_IsEnabled(true)
{

}



void SpriteLayer::AddSprite(Sprite* currentSprite)
{
	m_Sprites.insert(currentSprite);
}



void SpriteLayer::RemoveSprite(Sprite* currentSprite)
{
	m_Sprites.erase(currentSprite);
}



void SpriteLayer::RegisterParticleSystem(ParticleSystem* particleSystem)
{
	m_ParticleSystems.insert(particleSystem);
}



void SpriteLayer::SetLayerEnabled(bool enabled)
{
	m_IsEnabled = enabled;
}



bool SpriteLayer::IsLayerEnabled() const
{
	return m_IsEnabled;
}



uint8_t SpriteLayer::GetLayerID() const
{
	return m_ID;
}



void SpriteLayer::RenderAllSprites() const
{
	for (const Sprite* currentSprite : m_Sprites)
	{
		if (currentSprite->IsSpriteEnabled())
		{
			currentSprite->Render();
		}
	}
}



void SpriteLayer::UpdateAllParticleSystems(float deltaTimeInSeconds)
{
	for (ParticleSystem* currentParticleSystem : m_ParticleSystems)
	{
		currentParticleSystem->Update(deltaTimeInSeconds);
	}
}



void SpriteLayer::DestroyTerminatedParticleSystems()
{
	for (auto particleSystemIterator = m_ParticleSystems.begin(); particleSystemIterator != m_ParticleSystems.end();)
	{
		ParticleSystem* currentParticleSystem = *particleSystemIterator;
		if (currentParticleSystem->HasTerminated() && currentParticleSystem->HasFinishedPlaying())
		{
			particleSystemIterator = m_ParticleSystems.erase(particleSystemIterator);
			ParticleSystem::DestroyParticleSystem(currentParticleSystem);
			continue;
		}

		++particleSystemIterator;
	}
}



void SpriteLayer::RenderAllParticleSystems() const
{
	for (const ParticleSystem* currentParticleSystem : m_ParticleSystems)
	{
		currentParticleSystem->Render();
	}
}