#include "Engine/Renderer/ParticleSystem/ParticleSystemDefinition.hpp"



ParticleSystemDefinition::ParticleSystemDefinition(const char* definitionID) :
m_DefinitionID(definitionID)
{

}



ParticleSystemDefinition::~ParticleSystemDefinition()
{

}



ParticleSystemDefinition* ParticleSystemDefinition::CreateParticleSystemDefinition(const char* definitionID)
{
	return new ParticleSystemDefinition(definitionID);
}



void ParticleSystemDefinition::DestroyParticleSystemDefinition(ParticleSystemDefinition* particleSystemDefinition)
{
	delete particleSystemDefinition;
	particleSystemDefinition = nullptr;
}



ParticleEmitterDefinition* ParticleSystemDefinition::AddEmitterDefinition(ParticleEmitterDefinition* particleEmitterDefinition)
{
	if (particleEmitterDefinition != nullptr)
	{
		m_EmitterDefinitions.insert(particleEmitterDefinition);
	}

	return particleEmitterDefinition;
}



bool ParticleSystemDefinition::IsLooping() const
{
	for (ParticleEmitterDefinition* currentEmitterDefinition : m_EmitterDefinitions)
	{
		if (currentEmitterDefinition->IsLooping())
		{
			return true;
		}
	}

	return false;
}