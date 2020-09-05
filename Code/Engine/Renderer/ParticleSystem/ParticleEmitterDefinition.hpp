#pragma once

#include <set>
#include "Engine/Math/VectorMath/2D/Vector2.hpp"
#include "Engine/Renderer/Color/RGBA.hpp"
#include "Engine/Renderer/SpriteRendering/SpriteResource.hpp"
#include "Engine/Renderer/ParticleSystem/Particle.hpp"



class ParticleEmitterDefinition
{
private:
	ParticleEmitterDefinition(const char* spriteResourceID);
	~ParticleEmitterDefinition();

	void FadeTintForParticle(Particle* currentParticle);

public:
	static ParticleEmitterDefinition* CreateParticleEmitterDefinition(const char* spriteResourceID);
	static void DestroyParticleEmitterDefinition(ParticleEmitterDefinition* particleEmitterDefinition);

	Particle* SpawnNewParticle();
	void UpdateParticles(std::set<Particle*>& allParticles, float deltaTimeInSeconds);
	
	float GetTimePerParticleInSeconds() const;
	bool IsLooping() const;

	Vector2 GetParticleMinimums() const;
	Vector2 GetParticleMaximums() const;
	AABB2 GetParticleTextureCoordinates() const;

public:
	SpriteResource* m_SpriteResource;
	
	uint32_t m_InitialNumberOfSpawnParticles;

	float m_SpawnRatePerSecond;
	float m_LifeTime;

	Vector2 m_ScaleRange;
	AABB2 m_VelocityRange;
	AABB2 m_AccelerationRange;

	RGBA m_Tint;
};