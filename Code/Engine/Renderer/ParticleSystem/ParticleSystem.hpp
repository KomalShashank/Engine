#pragma once

#include <set>
#include "Engine/Renderer/ParticleSystem/ParticleEmitter.hpp"
#include "Engine/Renderer/ParticleSystem/ParticleSystemDefinition.hpp"



class ParticleSystem
{
private:
	ParticleSystem(const char* definitionID, const Vector2& spawnPosition);
	~ParticleSystem();

public:
	static ParticleSystem* CreateParticleSystemFromDefinition(const char* definitionID, const Vector2& spawnPosition);
	static void DestroyParticleSystem(ParticleSystem* particleSystem);

	void SpawnInitialParticles();

	void Update(float deltaTimeInSeconds);
	void Render() const;

	bool HasTerminated() const;
	void TerminateSystem();

	bool HasFinishedPlaying() const;
	bool IsLooping() const;

public:
	ParticleSystemDefinition* m_ParticleSystemDefinition;
	std::set<ParticleEmitter*> m_ParticleEmitters;

private:
	bool m_HasTerminated;
};