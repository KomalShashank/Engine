#include "Engine/Renderer/ParticleSystem/ParticleSystem.hpp"
#include "Engine/Renderer/ParticleSystem/ParticleSystemDatabase.hpp"



ParticleSystem::ParticleSystem(const char* definitionID, const Vector2& spawnPosition) :
m_ParticleSystemDefinition(ParticleSystemDatabase::SingletonInstance()->GetEditableParticleSystemDefinition(definitionID)),
m_HasTerminated(false)
{
	for (ParticleEmitterDefinition* currentEmitterDefinition : m_ParticleSystemDefinition->m_EmitterDefinitions)
	{
		ParticleEmitter* particleEmitter = ParticleEmitter::CreateParticleEmitterFromDefinition(currentEmitterDefinition, spawnPosition);
		m_ParticleEmitters.insert(particleEmitter);
	}
}



ParticleSystem::~ParticleSystem()
{
	for (auto particleEmitterIterator = m_ParticleEmitters.begin(); particleEmitterIterator != m_ParticleEmitters.end();)
	{
		ParticleEmitter* currentParticleEmitter = *particleEmitterIterator;
		particleEmitterIterator = m_ParticleEmitters.erase(particleEmitterIterator);
		ParticleEmitter::DestroyParticleEmitter(currentParticleEmitter);
	}
}



ParticleSystem* ParticleSystem::CreateParticleSystemFromDefinition(const char* definitionID, const Vector2& spawnPosition)
{
	return new ParticleSystem(definitionID, spawnPosition);
}



void ParticleSystem::DestroyParticleSystem(ParticleSystem* particleSystem)
{
	delete particleSystem;
	particleSystem = nullptr;
}



void ParticleSystem::SpawnInitialParticles()
{
	for (ParticleEmitter* currentParticleEmitter : m_ParticleEmitters)
	{
		currentParticleEmitter->SpawnInitialParticles();
	}
}



void ParticleSystem::Update(float deltaTimeInSeconds)
{
	for (ParticleEmitter* currentParticleEmitter : m_ParticleEmitters)
	{
		currentParticleEmitter->Update(deltaTimeInSeconds);
	}
}



void ParticleSystem::Render() const
{
	for (ParticleEmitter* currentParticleEmitter : m_ParticleEmitters)
	{
		currentParticleEmitter->Render();
	}
}



bool ParticleSystem::HasTerminated() const
{
	return m_HasTerminated;
}



void ParticleSystem::TerminateSystem()
{
	m_HasTerminated = true;

	for (ParticleEmitter* currentParticleEmitter : m_ParticleEmitters)
	{
		currentParticleEmitter->TerminateEmitter();
	}
}



bool ParticleSystem::HasFinishedPlaying() const
{
	for (ParticleEmitter* currentParticleEmitter : m_ParticleEmitters)
	{
		if (!currentParticleEmitter->HasFinishedPlaying())
		{
			return false;
		}
	}

	return true;
}



bool ParticleSystem::IsLooping() const
{
	return m_ParticleSystemDefinition->IsLooping();
}