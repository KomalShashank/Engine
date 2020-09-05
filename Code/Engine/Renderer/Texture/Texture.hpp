#pragma once

#include <map>
#include "Engine/DebugTools/MemoryAnalytics/UntrackedAllocator.hpp"
#include "Engine/Math/VectorMath/2D/IntVector2.hpp"
#include "Engine/ErrorHandling/StringUtils.hpp"
#include "Engine/Renderer/Color/RGBA.hpp"



enum TextureFormat
{
	RGBA_8,
	D_24S_8
};



class SamplerData
{
public:
	SamplerData();
	SamplerData(unsigned int horizontalWrapMode, unsigned int verticalWrapMode, unsigned int minificationFilter, unsigned int magnificationFilter);
	
public:
	unsigned int m_HorizontalWrapMode;
	unsigned int m_VerticalWrapMode;
	unsigned int m_MinificationFilter;
	unsigned int m_MagnificationFilter;
};



class Texture
{
private:
	Texture(const char* imageFilePath, const SamplerData& samplerData);
	Texture(const IntVector2& texelSize, const TextureFormat& texelFormat, const SamplerData& samplerData, bool hasColorData = false, const RGBA& texelColor = RGBA::WHITE);

	static Texture* GetTextureByName(const char* imageFilePath);

	void GetTexelDataForColor(const RGBA& texelColor, std::vector<unsigned char>& texelData, size_t numberOfTexels);
	void CreateSampler(const SamplerData& samplerData);

public:
	static Texture* CreateOrGetTexture(const char* imageFilePath, const SamplerData& samplerData);
	static Texture* CreateDefaultTexture(const IntVector2& texelSize, const TextureFormat& texelFormat, const SamplerData& samplerData, bool hasColorData = false, const RGBA& texelColor = RGBA::WHITE);

	static void DestroyAllTextures();

public:
	static std::map<const char*, Texture*, CompareCStrings, UntrackedAllocator<std::pair<const char*, Texture*>>> s_TextureRegistry;
	
	unsigned int m_TextureID;
	IntVector2 m_TexelSize;

	unsigned int m_SamplerID;
};