#pragma once

#include <map>
#include "Engine/Renderer/ParticleSystem/ParticleSystemDefinition.hpp"
#include "Engine/ErrorHandling/StringUtils.hpp"
#include "Engine/DebugTools/MemoryAnalytics/UntrackedAllocator.hpp"



class ParticleSystemDatabase
{
private:
	ParticleSystemDatabase();
	ParticleSystemDatabase(const ParticleSystemDatabase&) = delete;

	static void DestroyAllParticleSystemDefinitions();

public:
	static ParticleSystemDatabase* SingletonInstance();
	static void DestroyInstance();

	ParticleSystemDefinition* AddParticleSystemDefinition(const char* definitionID);

	const ParticleSystemDefinition* GetConstantParticleSystemDefinition(const char* definitionID);
	ParticleSystemDefinition* GetEditableParticleSystemDefinition(const char* definitionID);

public:
	static std::map<const char*, ParticleSystemDefinition*, CompareCStrings, UntrackedAllocator<std::pair<const char*, ParticleSystemDefinition*>>> s_ParticleSystemRegistry;
};