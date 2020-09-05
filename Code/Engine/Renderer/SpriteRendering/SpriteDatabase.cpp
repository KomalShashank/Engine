#include "Engine/Renderer/SpriteRendering/SpriteDatabase.hpp"
#include "Engine/Renderer/SpriteRendering/SpriteRenderer.hpp"
#include "Engine/Renderer/RenderUtilities/RenderConstants.hpp"
#include "Engine/ErrorHandling/ErrorWarningAssert.hpp"



SpriteDatabase* g_SpriteDatabase = nullptr;



std::map<const char*, SpriteResource*, CompareCStrings, UntrackedAllocator<std::pair<const char*, SpriteResource*>>> SpriteDatabase::s_SpriteRegistry;
std::map<const char*, AnimationSequence*, CompareCStrings, UntrackedAllocator<std::pair<const char*, AnimationSequence*>>> SpriteDatabase::s_AnimationRegistry;



SpriteDatabase::SpriteDatabase()
{

}



void SpriteDatabase::InitializeSpriteDatabase()
{
	if (g_SpriteDatabase == nullptr)
	{
		g_SpriteDatabase = new SpriteDatabase();
	}
}



void SpriteDatabase::UninitializeSpriteDatabase()
{
	if (g_SpriteDatabase != nullptr)
	{
		SpriteDatabase::DestroyAllSpriteResources();
		SpriteDatabase::DestroyAllAnimationSequences();

		delete g_SpriteDatabase;
		g_SpriteDatabase = nullptr;
	}
}



SpriteDatabase* SpriteDatabase::SingletonInstance()
{
	ASSERT_OR_DIE(g_SpriteDatabase != nullptr, "Sprite Database instance does not exist.");

	return g_SpriteDatabase;
}



SpriteResource* SpriteDatabase::AddSpriteResource(const char* spriteResourceID, const char* textureFilePath, const AABB2& textureCoordinates)
{
	SpriteResource* spriteResource = new SpriteResource();
	spriteResource->m_ID = spriteResourceID;
	spriteResource->m_Material = SpriteRenderer::SingletonInstance()->GetDefaultMaterial();

	SamplerData samplerData = SamplerData(REPEAT_WRAP, REPEAT_WRAP, LINEAR_FILTER, NEAREST_FILTER);
	spriteResource->m_Texture = Texture::CreateOrGetTexture(textureFilePath, samplerData);

	IntVector2 texelSize = spriteResource->m_Texture->m_TexelSize;
	float spriteWidth = static_cast<float>(texelSize.X) * SpriteRenderer::SingletonInstance()->GetVirtualToImportRatio();
	float spriteHeight = static_cast<float>(texelSize.Y) * SpriteRenderer::SingletonInstance()->GetVirtualToImportRatio();

	spriteResource->m_Dimensions = Vector2(spriteWidth, spriteHeight);
	spriteResource->m_Pivot = Vector2(spriteWidth / 2.0f, spriteHeight / 2.0f);

	spriteResource->m_TextureCoordinates = textureCoordinates;

	s_SpriteRegistry[spriteResourceID] = spriteResource;

	return spriteResource;
}



AnimationSequence* SpriteDatabase::AddAnimationSequence(const char* animationSequenceID, const SpriteAnimationMode& animationMode)
{
	AnimationSequence* animationSequence = new AnimationSequence();
	animationSequence->m_ID = animationSequenceID;
	animationSequence->m_AnimationMode = animationMode;

	s_AnimationRegistry[animationSequenceID] = animationSequence;

	return animationSequence;
}



void SpriteDatabase::DestroyAllSpriteResources()
{
	for (auto currentSpriteResource = s_SpriteRegistry.begin(); currentSpriteResource != s_SpriteRegistry.end();)
	{
		delete currentSpriteResource->second;
		currentSpriteResource = s_SpriteRegistry.erase(currentSpriteResource);
	}
}



void SpriteDatabase::DestroyAllAnimationSequences()
{
	for (auto currentAnimationSequence = s_AnimationRegistry.begin(); currentAnimationSequence != s_AnimationRegistry.end();)
	{
		delete currentAnimationSequence->second;
		currentAnimationSequence = s_AnimationRegistry.erase(currentAnimationSequence);
	}
}



const SpriteResource* SpriteDatabase::GetConstantSpriteResource(const char* spriteResourceID)
{
	auto spriteRegistryIterator = s_SpriteRegistry.find(spriteResourceID);
	if (spriteRegistryIterator != s_SpriteRegistry.end())
	{
		return spriteRegistryIterator->second;
	}

	return nullptr;
}



SpriteResource* SpriteDatabase::GetEditableSpriteResource(const char* spriteResourceID)
{
	auto spriteRegistryIterator = s_SpriteRegistry.find(spriteResourceID);
	if (spriteRegistryIterator != s_SpriteRegistry.end())
	{
		return spriteRegistryIterator->second;
	}

	return nullptr;
}



const AnimationSequence* SpriteDatabase::GetConstantAnimationSequence(const char* animationSequenceID)
{
	auto animationRegistryIterator = s_AnimationRegistry.find(animationSequenceID);
	if (animationRegistryIterator != s_AnimationRegistry.end())
	{
		return animationRegistryIterator->second;
	}

	return nullptr;
}



AnimationSequence* SpriteDatabase::GetEditableAnimationSequence(const char* animationSequenceID)
{
	auto animationRegistryIterator = s_AnimationRegistry.find(animationSequenceID);
	if (animationRegistryIterator != s_AnimationRegistry.end())
	{
		return animationRegistryIterator->second;
	}

	return nullptr;
}