#include "Engine/Renderer/ParticleSystem/Particle.hpp"
#include "Engine/Math/MathUtilities/MathUtilities.hpp"



Particle::Particle() :
m_Position(Vector2::ZERO),
m_Scale(Vector2::ONE),
m_Velocity(Vector2::ZERO),
m_Acceleration(Vector2::ZERO),
m_Age(0.0f),
m_LifeTime(0.0f),
m_Tint(RGBA::WHITE)
{

}



Particle::~Particle()
{

}



float Particle::GetRatioOfAgeOverLifeTime() const
{
	float clampedAge = ClampFloat(m_Age, 0.0f, m_LifeTime);
	
	return clampedAge / m_LifeTime;
}



bool Particle::IsDead() const
{
	return (m_Age >= m_LifeTime);
}