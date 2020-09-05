#include "Engine/Renderer/SpriteRendering/SpriteRenderer.hpp"
#include "Engine/Renderer/RenderUtilities/AdvancedRenderer.hpp"
#include "Engine/Math/MatrixMath/Matrix4.hpp"
#include "Engine/DeveloperConsole/DeveloperConsole.hpp"
#include "Engine/ErrorHandling/ErrorWarningAssert.hpp"



SpriteRenderer* g_SpriteRenderer = nullptr;



bool CompareLayerIDs::operator()(SpriteLayer* firstLayer, SpriteLayer* secondLayer) const
{
	return (firstLayer->GetLayerID() < secondLayer->GetLayerID());
}



SpriteRenderer::SpriteRenderer()
{
	m_RendererClock = new Clock();
	m_DefaultMaterial = new Material("Data/Shaders/DefaultShader.vert", "Data/Shaders/DefaultShader.frag");

	DeveloperConsole::RegisterCommands("EnableSpriteLayer", "Enables the given layer. Takes layer ID as argument.", EnableLayerCommand);
	DeveloperConsole::RegisterCommands("DisableSpriteLayer", "Disables the given layer. Takes layer ID as argument.", DisableLayerCommand);
}



SpriteRenderer::~SpriteRenderer()
{
	delete m_RendererClock;
	delete m_DefaultMaterial;

	for (auto spriteLayerIterator = m_SpriteLayers.begin(); spriteLayerIterator != m_SpriteLayers.end();)
	{
		SpriteLayer* currentSpriteLayer = *spriteLayerIterator;
		spriteLayerIterator = m_SpriteLayers.erase(spriteLayerIterator);
		delete currentSpriteLayer;
	}

	for (auto animatedSpriteIterator = m_AnimatedSprites.begin(); animatedSpriteIterator != m_AnimatedSprites.end();)
	{
		AnimatedSprite* currentAnimatedSprite = *animatedSpriteIterator;
		animatedSpriteIterator = m_AnimatedSprites.erase(animatedSpriteIterator);
		delete currentAnimatedSprite;
	}
}



void SpriteRenderer::InitializeSpriteRenderer()
{
	if (g_SpriteRenderer == nullptr)
	{
		g_SpriteRenderer = new SpriteRenderer();
	}
}



void SpriteRenderer::UninitializeSpriteRenderer()
{
	if (g_SpriteRenderer != nullptr)
	{
		delete g_SpriteRenderer;
		g_SpriteRenderer = nullptr;
	}
}



SpriteRenderer* SpriteRenderer::SingletonInstance()
{
	ASSERT_OR_DIE(g_SpriteRenderer != nullptr, "Sprite Renderer instance does not exist.");

	return g_SpriteRenderer;
}



void SpriteRenderer::Update()
{
	float deltaTimeInSeconds = m_RendererClock->GetDeltaTimeFloat();
	for (AnimatedSprite* currentAnimatedSprite : m_AnimatedSprites)
	{
		currentAnimatedSprite->Update(deltaTimeInSeconds);
	}

	UpdateAllLayers(deltaTimeInSeconds);
}



void SpriteRenderer::Render() const
{
	AdvancedRenderer::SingletonInstance()->ClearScreen(m_ClearColor);
	AdvancedRenderer::SingletonInstance()->EnableBackFaceCulling(true);
	AdvancedRenderer::SingletonInstance()->EnableDepthTesting(false);

	Vector2 screenBottomLeft = GetScreenBounds().minimums;
	Vector2 screenTopRight = GetScreenBounds().maximums;
	
	Matrix4 modelMatrix = Matrix4::IdentityMatrix4();
	Matrix4 viewMatrix = Matrix4::IdentityMatrix4();
	Matrix4 projectionMatrix = AdvancedRenderer::SingletonInstance()->GetOrthographicProjectionMatrix(screenBottomLeft, screenTopRight, -1.0f, 1.0f);

	AdvancedRenderer::SingletonInstance()->UpdateModelMatrix(modelMatrix);
	AdvancedRenderer::SingletonInstance()->UpdateViewMatrix(viewMatrix);
	AdvancedRenderer::SingletonInstance()->UpdateProjectionMatrix(projectionMatrix);

	RenderAllLayers();
}



void SpriteRenderer::SetClearColor(const RGBA& clearColor)
{
	m_ClearColor = clearColor;
}



void SpriteRenderer::SetVirtualSize(float virtualSize)
{
	m_VirtualSize = virtualSize;
}



float SpriteRenderer::GetVirtualSize() const
{
	return m_VirtualSize;
}



void SpriteRenderer::SetImportSize(float importSize)
{
	m_ImportSize = importSize;
}



float SpriteRenderer::GetImportSize() const
{
	return m_ImportSize;
}



void SpriteRenderer::SetPixelResolution(const IntVector2& pixelResolution)
{
	m_PixelResolution = pixelResolution;
}



IntVector2 SpriteRenderer::GetPixelResolution() const
{
	return m_PixelResolution;
}



void SpriteRenderer::SetWorldBounds(const Vector2& worldMinimums, const Vector2& worldMaximums)
{
	m_WorldBounds = AABB2(worldMinimums, worldMaximums);
}



AABB2 SpriteRenderer::GetWorldBounds() const
{
	return m_WorldBounds;
}



void SpriteRenderer::SetCameraPosition(const Vector2& cameraPosition)
{
	m_CameraPosition = cameraPosition;

	if (m_CameraPosition.X < m_WorldBounds.minimums.X + (GetScreenSize().X * 0.5f))
	{
		m_CameraPosition.X = m_WorldBounds.minimums.X + (GetScreenSize().X * 0.5f);
	}
	else if (m_CameraPosition.X > m_WorldBounds.maximums.X - (GetScreenSize().X * 0.5f))
	{
		m_CameraPosition.X = m_WorldBounds.maximums.X - (GetScreenSize().X * 0.5f);
	}

	if (m_CameraPosition.Y < m_WorldBounds.minimums.Y + (GetScreenSize().Y * 0.5f))
	{
		m_CameraPosition.Y = m_WorldBounds.minimums.Y + (GetScreenSize().Y * 0.5f);
	}
	else if (m_CameraPosition.Y > m_WorldBounds.maximums.Y - (GetScreenSize().Y * 0.5f))
	{
		m_CameraPosition.Y = m_WorldBounds.maximums.Y - (GetScreenSize().Y * 0.5f);
	}
}



Vector2 SpriteRenderer::GetCameraPosition() const
{
	return m_CameraPosition;
}



Vector2 SpriteRenderer::GetScreenSize() const
{
	float screenWidth = GetAspectRatio() * m_VirtualSize;
	float screenHeight = m_VirtualSize;

	return Vector2(screenWidth, screenHeight);
}



AABB2 SpriteRenderer::GetScreenBounds() const
{
	Vector2 screenSize = GetScreenSize();

	Vector2 bottomLeft = GetCameraPosition() - (GetScreenSize() * 0.5f);
	Vector2 topRight = GetCameraPosition() + (GetScreenSize() * 0.5f);

	return AABB2(bottomLeft, topRight);
}



float SpriteRenderer::GetAspectRatio() const
{
	return (static_cast<float>(m_PixelResolution.X) / static_cast<float>(m_PixelResolution.Y));
}



float SpriteRenderer::GetVirtualToImportRatio() const
{
	return m_VirtualSize / m_ImportSize;
}



Material* SpriteRenderer::GetDefaultMaterial() const
{
	return m_DefaultMaterial;
}



void SpriteRenderer::RegisterSprite(Sprite* currentSprite)
{
	if (currentSprite == nullptr)
	{
		return;
	}

	SpriteLayer* currentLayer = CreateOrGetSpriteLayer(currentSprite->GetLayerID());
	currentLayer->AddSprite(currentSprite);
}



void SpriteRenderer::UnregisterSprite(Sprite* currentSprite)
{
	if (currentSprite == nullptr)
	{
		return;
	}

	SpriteLayer* currentLayer = CreateOrGetSpriteLayer(currentSprite->GetLayerID());
	currentLayer->RemoveSprite(currentSprite);
}



void SpriteRenderer::RegisterAnimatedSprite(AnimatedSprite* currentAnimatedSprite)
{
	if (currentAnimatedSprite == nullptr)
	{
		return;
	}

	m_AnimatedSprites.insert(currentAnimatedSprite);
}



void SpriteRenderer::UnregisterAnimatedSprite(AnimatedSprite* currentAnimatedSprite)
{
	if (currentAnimatedSprite == nullptr)
	{
		return;
	}

	m_AnimatedSprites.erase(currentAnimatedSprite);
}



void SpriteRenderer::EnableSpriteLayer(uint8_t layerID)
{
	for (SpriteLayer* currentLayer : m_SpriteLayers)
	{
		if (currentLayer->GetLayerID() == layerID)
		{
			currentLayer->SetLayerEnabled(true);
		}
	}
}



void SpriteRenderer::DisableSpriteLayer(uint8_t layerID)
{
	for (SpriteLayer* currentLayer : m_SpriteLayers)
	{
		if (currentLayer->GetLayerID() == layerID)
		{
			currentLayer->SetLayerEnabled(false);
		}
	}
}



ParticleSystem* SpriteRenderer::CreateParticleSystem(const char* definitionID, const Vector2& spawnPosition)
{
	ParticleSystem* particleSystem = ParticleSystem::CreateParticleSystemFromDefinition(definitionID, spawnPosition);
	if (particleSystem != nullptr)
	{
		ASSERT_OR_DIE(particleSystem->IsLooping(), "Particle System needs to be looping.");
		SpriteLayer* currentLayer = CreateOrGetSpriteLayer(PARTICLE_LAYER);
		currentLayer->RegisterParticleSystem(particleSystem);

		particleSystem->SpawnInitialParticles();
	}

	return particleSystem;
}



void SpriteRenderer::DestroyParticleSystem(ParticleSystem* particleSystem)
{
	particleSystem->TerminateSystem();
}



void SpriteRenderer::PlayParticleSystem(const char* definitionID, const Vector2& spawnPosition)
{
	ParticleSystem* particleSystem = ParticleSystem::CreateParticleSystemFromDefinition(definitionID, spawnPosition);
	if (particleSystem != nullptr)
	{
		ASSERT_OR_DIE(!particleSystem->IsLooping(), "Particle System needs to be non-looping.");
		SpriteLayer* currentLayer = CreateOrGetSpriteLayer(PARTICLE_LAYER);
		currentLayer->RegisterParticleSystem(particleSystem);

		particleSystem->SpawnInitialParticles();
		particleSystem->TerminateSystem();
	}
}



SpriteLayer* SpriteRenderer::CreateOrGetSpriteLayer(uint8_t layerID)
{
	for (SpriteLayer* currentLayer : m_SpriteLayers)
	{
		if (currentLayer->GetLayerID() == layerID)
		{
			return currentLayer;
		}
	}

	SpriteLayer* newLayer = new SpriteLayer(layerID);
	m_SpriteLayers.insert(newLayer);

	return newLayer;
}



void SpriteRenderer::UpdateAllLayers(float deltaTimeInSeconds)
{
	for (SpriteLayer* currentLayer : m_SpriteLayers)
	{
		if (currentLayer->IsLayerEnabled())
		{
			currentLayer->UpdateAllParticleSystems(deltaTimeInSeconds);
		}
	}

	for (SpriteLayer* currentLayer : m_SpriteLayers)
	{
		if (currentLayer->IsLayerEnabled())
		{
			currentLayer->DestroyTerminatedParticleSystems();
		}
	}
}



void SpriteRenderer::RenderAllLayers() const
{
	for (const SpriteLayer* currentLayer : m_SpriteLayers)
	{
		if (currentLayer->IsLayerEnabled())
		{
			currentLayer->RenderAllSprites();
			currentLayer->RenderAllParticleSystems();
		}
	}
}



void EnableLayerCommand(Command& currentCommand)
{
	ConsoleLine enabledMessage;

	if (currentCommand.HasNoArguments())
	{
		enabledMessage = ConsoleLine("Cannot enable layer. Layer ID not given.", RGBA::RED);
	}
	else
	{
		std::vector<std::string> currentCommandArguments;
		currentCommand.GetCommandArguments(currentCommandArguments);
		std::string layerString = currentCommandArguments[0];

		uint8_t layerID = static_cast<uint8_t>(stoi(layerString));
		g_SpriteRenderer->EnableSpriteLayer(layerID);
		
		enabledMessage = ConsoleLine("Layer " + layerString + " enabled.", RGBA::GREEN);
	}

	DeveloperConsole::AddNewConsoleLine(enabledMessage);
}



void DisableLayerCommand(Command& currentCommand)
{
	ConsoleLine disabledMessage;

	if (currentCommand.HasNoArguments())
	{
		disabledMessage = ConsoleLine("Cannot disable layer. Layer ID not given.", RGBA::RED);
	}
	else
	{
		std::vector<std::string> currentCommandArguments;
		currentCommand.GetCommandArguments(currentCommandArguments);
		std::string layerString = currentCommandArguments[0];

		uint8_t layerID = static_cast<uint8_t>(stoi(layerString));
		g_SpriteRenderer->DisableSpriteLayer(layerID);

		disabledMessage = ConsoleLine("Layer " + layerString + " disabled.", RGBA::GREEN);
	}

	DeveloperConsole::AddNewConsoleLine(disabledMessage);
}