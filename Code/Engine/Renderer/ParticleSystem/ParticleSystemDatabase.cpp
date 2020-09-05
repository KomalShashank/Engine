#include "Engine/Renderer/ParticleSystem/ParticleSystemDatabase.hpp"



ParticleSystemDatabase* g_ParticleSystemDatabase = nullptr;



std::map<const char*, ParticleSystemDefinition*, CompareCStrings, UntrackedAllocator<std::pair<const char*, ParticleSystemDefinition*>>> ParticleSystemDatabase::s_ParticleSystemRegistry;



ParticleSystemDatabase::ParticleSystemDatabase()
{

}



void ParticleSystemDatabase::DestroyAllParticleSystemDefinitions()
{
	for (auto particleSystemRegistryIterator = s_ParticleSystemRegistry.begin(); particleSystemRegistryIterator != s_ParticleSystemRegistry.end();)
	{
		ParticleSystemDefinition::DestroyParticleSystemDefinition(particleSystemRegistryIterator->second);
		particleSystemRegistryIterator = s_ParticleSystemRegistry.erase(particleSystemRegistryIterator);
	}
}



ParticleSystemDatabase* ParticleSystemDatabase::SingletonInstance()
{
	if (g_ParticleSystemDatabase == nullptr)
	{
		g_ParticleSystemDatabase = new ParticleSystemDatabase();
	}

	return g_ParticleSystemDatabase;
}



void ParticleSystemDatabase::DestroyInstance()
{
	if (g_ParticleSystemDatabase != nullptr)
	{
		delete g_ParticleSystemDatabase;
		g_ParticleSystemDatabase = nullptr;

		ParticleSystemDatabase::DestroyAllParticleSystemDefinitions();
	}
}



ParticleSystemDefinition* ParticleSystemDatabase::AddParticleSystemDefinition(const char* definitionID)
{
	ParticleSystemDefinition* particleSystemDefinition = ParticleSystemDefinition::CreateParticleSystemDefinition(definitionID);
	s_ParticleSystemRegistry[definitionID] = particleSystemDefinition;

	return particleSystemDefinition;
}



const ParticleSystemDefinition* ParticleSystemDatabase::GetConstantParticleSystemDefinition(const char* definitionID)
{
	auto particleSystemRegistryIterator = s_ParticleSystemRegistry.find(definitionID);
	if (particleSystemRegistryIterator != s_ParticleSystemRegistry.end())
	{
		return particleSystemRegistryIterator->second;
	}

	return nullptr;
}



ParticleSystemDefinition* ParticleSystemDatabase::GetEditableParticleSystemDefinition(const char* definitionID)
{
	auto particleSystemRegistryIterator = s_ParticleSystemRegistry.find(definitionID);
	if (particleSystemRegistryIterator != s_ParticleSystemRegistry.end())
	{
		return particleSystemRegistryIterator->second;
	}

	return nullptr;
}