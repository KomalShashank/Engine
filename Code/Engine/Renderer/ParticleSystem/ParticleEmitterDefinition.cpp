#include "Engine/Renderer/ParticleSystem/ParticleEmitterDefinition.hpp"
#include "Engine/Renderer/SpriteRendering/SpriteDatabase.hpp"
#include "Engine/Math/MathUtilities/MathUtilities.hpp"



ParticleEmitterDefinition::ParticleEmitterDefinition(const char* spriteResourceID) :
m_SpriteResource(SpriteDatabase::SingletonInstance()->GetEditableSpriteResource(spriteResourceID)),
m_InitialNumberOfSpawnParticles(0),
m_SpawnRatePerSecond(0.0f),
m_LifeTime(0.0f),
m_ScaleRange(Vector2::ONE),
m_VelocityRange(AABB2(Vector2::ZERO, Vector2::ZERO)),
m_AccelerationRange(AABB2(Vector2::ZERO, Vector2::ZERO)),
m_Tint(RGBA::WHITE)
{

}



ParticleEmitterDefinition::~ParticleEmitterDefinition()
{

}



void ParticleEmitterDefinition::FadeTintForParticle(Particle* currentParticle)
{
	float alphaBlendFactor = currentParticle->GetRatioOfAgeOverLifeTime();
	float maximumAlphaValue = static_cast<float>(m_Tint.m_Alpha);

	float particleAlphaValue = LinearlyInterpolateIn1D(maximumAlphaValue, 0.0f, alphaBlendFactor);
	currentParticle->m_Tint.m_Alpha = static_cast<unsigned char>(particleAlphaValue);
}



ParticleEmitterDefinition* ParticleEmitterDefinition::CreateParticleEmitterDefinition(const char* spriteResourceID)
{
	return new ParticleEmitterDefinition(spriteResourceID);
}



void ParticleEmitterDefinition::DestroyParticleEmitterDefinition(ParticleEmitterDefinition* particleEmitterDefinition)
{
	delete particleEmitterDefinition;
	particleEmitterDefinition = nullptr;
}



Particle* ParticleEmitterDefinition::SpawnNewParticle()
{
	Particle* emittedParticle = new Particle();
	
	float scaleFactor = GetRandomFloatWithinRange(m_ScaleRange.X, m_ScaleRange.Y);
	emittedParticle->m_Scale = Vector2(scaleFactor, scaleFactor);

	emittedParticle->m_Velocity = GetRandomVector2WithinRange(m_VelocityRange.minimums, m_VelocityRange.maximums);
	emittedParticle->m_Acceleration = GetRandomVector2WithinRange(m_AccelerationRange.minimums, m_AccelerationRange.maximums);

	emittedParticle->m_LifeTime = m_LifeTime;
	emittedParticle->m_Tint = m_Tint;

	return emittedParticle;
}



void ParticleEmitterDefinition::UpdateParticles(std::set<Particle*>& allParticles, float deltaTimeInSeconds)
{
	for (Particle* currentParticle : allParticles)
	{
		currentParticle->m_Age += deltaTimeInSeconds;
		currentParticle->m_Velocity += currentParticle->m_Acceleration * deltaTimeInSeconds;
		currentParticle->m_Position += currentParticle->m_Velocity * deltaTimeInSeconds;

		FadeTintForParticle(currentParticle);
	}
}



float ParticleEmitterDefinition::GetTimePerParticleInSeconds() const
{
	return (1.0f / m_SpawnRatePerSecond);
}



bool ParticleEmitterDefinition::IsLooping() const
{
	return (m_SpawnRatePerSecond > 0.0f);
}



Vector2 ParticleEmitterDefinition::GetParticleMinimums() const
{
	return m_SpriteResource->m_Pivot.GetNegatedVector2();
}



Vector2 ParticleEmitterDefinition::GetParticleMaximums() const
{
	return (GetParticleMinimums() + m_SpriteResource->m_Dimensions);
}



AABB2 ParticleEmitterDefinition::GetParticleTextureCoordinates() const
{
	return m_SpriteResource->m_TextureCoordinates;
}