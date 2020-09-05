#pragma once

#include <map>
#include "Engine/Renderer/SpriteRendering/SpriteResource.hpp"
#include "Engine/Renderer/SpriteRendering/AnimatedSprite.hpp"
#include "Engine/ErrorHandling/StringUtils.hpp"
#include "Engine/DebugTools/MemoryAnalytics/UntrackedAllocator.hpp"



class SpriteDatabase
{
private:
	SpriteDatabase();
	SpriteDatabase(const SpriteDatabase&) = delete;

public:
	static void InitializeSpriteDatabase();
	static void UninitializeSpriteDatabase();

	static SpriteDatabase* SingletonInstance();

	SpriteResource* AddSpriteResource(const char* spriteResourceID, const char* textureFilePath, const AABB2& textureCoordinates);
	AnimationSequence* AddAnimationSequence(const char* animationSequenceID, const SpriteAnimationMode& animationMode);

	static void DestroyAllSpriteResources();
	static void DestroyAllAnimationSequences();

	const SpriteResource* GetConstantSpriteResource(const char* spriteResourceID);
	SpriteResource* GetEditableSpriteResource(const char* spriteResourceID);

	const AnimationSequence* GetConstantAnimationSequence(const char* animationSequenceID);
	AnimationSequence* GetEditableAnimationSequence(const char* animationSequenceID);

public:
	static std::map<const char*, SpriteResource*, CompareCStrings, UntrackedAllocator<std::pair<const char*, SpriteResource*>>> s_SpriteRegistry;
	static std::map<const char*, AnimationSequence*, CompareCStrings, UntrackedAllocator<std::pair<const char*, AnimationSequence*>>> s_AnimationRegistry;
};