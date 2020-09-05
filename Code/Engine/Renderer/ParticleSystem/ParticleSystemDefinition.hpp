#pragma once

#include <set>
#include "Engine/Renderer/ParticleSystem/ParticleEmitterDefinition.hpp"



class ParticleSystemDefinition
{
private:
	ParticleSystemDefinition(const char* definitionID);
	~ParticleSystemDefinition();

public:
	static ParticleSystemDefinition* CreateParticleSystemDefinition(const char* definitionID);
	static void DestroyParticleSystemDefinition(ParticleSystemDefinition* particleSystemDefinition);

	ParticleEmitterDefinition* AddEmitterDefinition(ParticleEmitterDefinition* particleEmitterDefinition);

	bool IsLooping() const;

public:
	const char* m_DefinitionID;
	std::set<ParticleEmitterDefinition*> m_EmitterDefinitions;
};