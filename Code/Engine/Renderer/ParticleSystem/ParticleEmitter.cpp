#include <vector>
#include "Engine/Renderer/ParticleSystem/ParticleEmitter.hpp"
#include "Engine/Renderer/RenderUtilities/AdvancedRenderer.hpp"



ParticleEmitter::ParticleEmitter(ParticleEmitterDefinition* particleEmitterDefinition, const Vector2& spawnPosition) :
m_TimeElapsedSinceLastSpawn(0.0f),
m_SpawnPosition(spawnPosition),
m_EmitterMesh(new Mesh()),
m_ParticleEmitterDefinition(particleEmitterDefinition),
m_HasTerminated(false)
{
	m_EmitterMaterial = particleEmitterDefinition->m_SpriteResource->m_Material;
}



ParticleEmitter::~ParticleEmitter()
{
	delete m_EmitterMesh;
}



void ParticleEmitter::UpdateExistingParticles(float deltaTimeInSeconds)
{
	m_ParticleEmitterDefinition->UpdateParticles(m_Particles, deltaTimeInSeconds);
}



void ParticleEmitter::DestroyDeadParticles()
{
	for (auto particleIterator = m_Particles.begin(); particleIterator != m_Particles.end();)
	{
		Particle* currentParticle = *particleIterator;
		if (currentParticle->IsDead())
		{
			particleIterator = m_Particles.erase(particleIterator);
			delete currentParticle;
			continue;
		}

		++particleIterator;
	}
}



void ParticleEmitter::SpawnNewParticles(float deltaTimeInSeconds)
{
	if (m_ParticleEmitterDefinition->IsLooping() && !HasTerminated())
	{
		m_TimeElapsedSinceLastSpawn += deltaTimeInSeconds;
		float timeTakenPerParticle = m_ParticleEmitterDefinition->GetTimePerParticleInSeconds();

		while (m_TimeElapsedSinceLastSpawn >= timeTakenPerParticle)
		{
			Particle* emittedParticle = m_ParticleEmitterDefinition->SpawnNewParticle();
			emittedParticle->m_Position = m_SpawnPosition;
			m_Particles.insert(emittedParticle);
			m_TimeElapsedSinceLastSpawn -= timeTakenPerParticle;
		}
	}
}



void ParticleEmitter::UpdateEmitterMesh() const
{
	std::vector<Vertex3D> meshVertices;
	std::vector<uint32_t> meshIndices;

	Vertex3D meshVertex;
	AABB2 textureCoordinates = m_ParticleEmitterDefinition->GetParticleTextureCoordinates();
	Vector2 particleMinimums = m_ParticleEmitterDefinition->GetParticleMinimums();
	Vector2 particleMaximums = m_ParticleEmitterDefinition->GetParticleMaximums();

	for (const Particle* currentParticle : m_Particles)
	{
		meshIndices.push_back(meshVertices.size() + 0);
		meshIndices.push_back(meshVertices.size() + 1);
		meshIndices.push_back(meshVertices.size() + 2);
		meshIndices.push_back(meshVertices.size() + 2);
		meshIndices.push_back(meshVertices.size() + 3);
		meshIndices.push_back(meshVertices.size() + 0);

		meshVertex.m_Color = currentParticle->m_Tint;

		Vector3 boundingPoint = Vector3::ZERO;

		boundingPoint.X = currentParticle->m_Position.X + (particleMinimums.X * currentParticle->m_Scale.X);
		boundingPoint.Y = currentParticle->m_Position.Y + (particleMinimums.Y * currentParticle->m_Scale.Y);
		meshVertex.m_Position = boundingPoint;
		meshVertex.m_TextureCoordinates = Vector2(textureCoordinates.minimums.X, textureCoordinates.maximums.Y);
		meshVertices.push_back(meshVertex);

		boundingPoint.X = currentParticle->m_Position.X + (particleMaximums.X * currentParticle->m_Scale.X);
		boundingPoint.Y = currentParticle->m_Position.Y + (particleMinimums.Y * currentParticle->m_Scale.Y);
		meshVertex.m_Position = boundingPoint;
		meshVertex.m_TextureCoordinates = Vector2(textureCoordinates.maximums.X, textureCoordinates.maximums.Y);
		meshVertices.push_back(meshVertex);

		boundingPoint.X = currentParticle->m_Position.X + (particleMaximums.X * currentParticle->m_Scale.X);
		boundingPoint.Y = currentParticle->m_Position.Y + (particleMaximums.Y * currentParticle->m_Scale.Y);
		meshVertex.m_Position = boundingPoint;
		meshVertex.m_TextureCoordinates = Vector2(textureCoordinates.maximums.X, textureCoordinates.minimums.Y);
		meshVertices.push_back(meshVertex);

		boundingPoint.X = currentParticle->m_Position.X + (particleMinimums.X * currentParticle->m_Scale.X);
		boundingPoint.Y = currentParticle->m_Position.Y + (particleMaximums.Y * currentParticle->m_Scale.Y);
		meshVertex.m_Position = boundingPoint;
		meshVertex.m_TextureCoordinates = Vector2(textureCoordinates.minimums.X, textureCoordinates.minimums.Y);
		meshVertices.push_back(meshVertex);
	}

	m_EmitterMesh->WriteToMesh(meshVertices.data(), meshIndices.data(), meshVertices.size(), meshIndices.size());
}



ParticleEmitter* ParticleEmitter::CreateParticleEmitterFromDefinition(ParticleEmitterDefinition* particleEmitterDefinition, const Vector2& spawnPosition)
{
	return new ParticleEmitter(particleEmitterDefinition, spawnPosition);
}



void ParticleEmitter::DestroyParticleEmitter(ParticleEmitter* particleEmitter)
{
	delete particleEmitter;
	particleEmitter = nullptr;
}



void ParticleEmitter::SpawnInitialParticles()
{
	for (uint32_t particleIndex = 0; particleIndex < m_ParticleEmitterDefinition->m_InitialNumberOfSpawnParticles; ++particleIndex)
	{
		Particle* emittedParticle = m_ParticleEmitterDefinition->SpawnNewParticle();
		emittedParticle->m_Position = m_SpawnPosition;
		m_Particles.insert(emittedParticle);
	}
}



void ParticleEmitter::Update(float deltaTimeInSeconds)
{
	UpdateExistingParticles(deltaTimeInSeconds);
	DestroyDeadParticles();
	SpawnNewParticles(deltaTimeInSeconds);
}



void ParticleEmitter::Render() const
{
	UpdateEmitterMesh();
	
	size_t numberOfVertices = m_EmitterMesh->m_VertexBufferObject->GetElementCount();
	size_t numberOfIndices = m_EmitterMesh->m_IndexBufferObject->GetElementCount();

	m_EmitterMaterial->SetDiffuseTexture(m_ParticleEmitterDefinition->m_SpriteResource->m_Texture);
	AdvancedRenderer::SingletonInstance()->DrawPolygonMesh(m_EmitterMesh, numberOfVertices, numberOfIndices, m_EmitterMaterial);
}



bool ParticleEmitter::HasTerminated() const
{
	return m_HasTerminated;
}



void ParticleEmitter::TerminateEmitter()
{
	m_HasTerminated = true;
}



bool ParticleEmitter::HasFinishedPlaying() const
{
	return (m_Particles.empty());
}



bool ParticleEmitter::IsLooping() const
{
	return m_ParticleEmitterDefinition->IsLooping();
}