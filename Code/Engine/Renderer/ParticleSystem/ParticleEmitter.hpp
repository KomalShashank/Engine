#pragma once

#include <set>
#include "Engine/Renderer/ParticleSystem/Particle.hpp"
#include "Engine/Renderer/ParticleSystem/ParticleEmitterDefinition.hpp"
#include "Engine/Renderer/MeshAndMaterial/Mesh.hpp"
#include "Engine/Renderer/MeshAndMaterial/Material.hpp"



class ParticleEmitter
{
private:
	ParticleEmitter(ParticleEmitterDefinition* particleEmitterDefinition, const Vector2& spawnPosition);
	~ParticleEmitter();

	void UpdateExistingParticles(float deltaTimeInSeconds);
	void DestroyDeadParticles();
	void SpawnNewParticles(float deltaTimeInSeconds);

	void UpdateEmitterMesh() const;

public:
	static ParticleEmitter* CreateParticleEmitterFromDefinition(ParticleEmitterDefinition* particleEmitterDefinition, const Vector2& spawnPosition);
	static void DestroyParticleEmitter(ParticleEmitter* particleEmitter);

	void SpawnInitialParticles();

	void Update(float deltaTimeInSeconds);
	void Render() const;

	bool HasTerminated() const;
	void TerminateEmitter();

	bool HasFinishedPlaying() const;
	bool IsLooping() const;

public:
	float m_TimeElapsedSinceLastSpawn;
	Vector2 m_SpawnPosition;

	Mesh* m_EmitterMesh;
	Material* m_EmitterMaterial;

	ParticleEmitterDefinition* m_ParticleEmitterDefinition;
	std::set<Particle*> m_Particles;

private:
	bool m_HasTerminated;
};