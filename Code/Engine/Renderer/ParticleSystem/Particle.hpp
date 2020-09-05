#pragma once

#include "Engine/Math/VectorMath/2D/Vector2.hpp"
#include "Engine/Renderer/Color/RGBA.hpp"



class Particle
{
public:
	Particle();
	~Particle();

	float GetRatioOfAgeOverLifeTime() const;
	bool IsDead() const;

public:
	Vector2 m_Position;
	Vector2 m_Scale;
	Vector2 m_Velocity;
	Vector2 m_Acceleration;
	
	float m_Age;
	float m_LifeTime;

	RGBA m_Tint;
};