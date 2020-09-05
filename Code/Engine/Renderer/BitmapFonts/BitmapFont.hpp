#pragma once

#include "Engine/Math/VectorMath/2D/AABB2.hpp"
#include "Engine/Math/VectorMath/2D/IntVector2.hpp"
#include "Engine/Renderer/SpriteRendering/SpriteSheet.hpp"
#include "Engine/ErrorHandling/StringUtils.hpp"
#include "Engine/DebugTools/MemoryAnalytics/UntrackedAllocator.hpp"



const size_t MAXIMUM_FONT_FILE_NAME_LENGTH = 128U;



class MonospaceFont
{
private:
	MonospaceFont(const char* monospaceFontName, const SamplerData& samplerData);
	static MonospaceFont* GetMonospaceFontByName(const char* monospaceFontName);

public:
	static MonospaceFont* CreateOrGetMonospaceFont(const char* monospaceFontName, const SamplerData& samplerData);
	static void DestroyAllMonospaceFonts();

	AABB2 GetTextureCoordsForGlyph(int glyphUnicode) const;
	Texture* GetFontTexture() const;

private:
	static std::map<const char*, MonospaceFont*, CompareCStrings, UntrackedAllocator<std::pair<const char*, MonospaceFont*>>> s_MonospaceFontRegistry;
	SpriteSheet m_FontSpriteSheet;
};



class ProportionalGlyph
{
public:
	ProportionalGlyph();

	Vector2 GetOffset() const;
	Vector2 GetSize() const;
	Vector2 GetAdvance() const;

public:
	int m_glyphUnicode;
	IntVector2 m_glyphTopLeft;
	IntVector2 m_glyphSize;
	IntVector2 m_glyphOffset;
	IntVector2 m_glyphAdvance;
	int m_glyphTextureID;
};



struct CompareKerningPairs
{
	bool operator()(const IntVector2& firstIntVector, const IntVector2& secondIntVector) const;
};



class ProportionalFont
{
private:
	ProportionalFont(const char* proportionalFontName);
	~ProportionalFont();
	static ProportionalFont* GetProportionalFontByName(const char* proportionalFontName);

public:
	static ProportionalFont* CreateOrGetProportionalFont(const char* proportionalFontName);
	static void DestroyAllProportionalFonts();
	
	Texture* GetFontTextureForGlyph(const ProportionalGlyph& glyph) const;
	AABB2 GetTextureBoundsForGlyph(const ProportionalGlyph& glyph) const;
	ProportionalGlyph* GetProportionalGlyphForGlyphUnicode(int glyphUnicode) const;

	float GetKerningForGlyphPair(int firstGlyphUnicode, int secondGlyphUnicode) const;
	float GetCharacterScaleForCellHeight(float cellHeight) const;
	float GetLineSpacingForCellHeight(float cellHeight) const;

	static float CalculateTextWidth(const char* asciiText, float cellHeight, const ProportionalFont* font = nullptr);

private:
	static std::map<const char*, ProportionalFont*, CompareCStrings, UntrackedAllocator<std::pair<const char*, ProportionalFont*>>> s_ProportionalFontRegistry;

	Vector2 m_TextureSize;
	float m_CharacterBaseHeight;
	float m_LineHeight;
	std::map<int, Texture*> m_AllFontTextures;
	std::map<int, ProportionalGlyph*> m_AllGlyphsForFont;
	std::map<IntVector2, int, CompareKerningPairs> m_AllGlyphKernings;
};