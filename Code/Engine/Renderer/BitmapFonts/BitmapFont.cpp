#include "Engine/Renderer/BitmapFonts/BitmapFont.hpp"
#include "Engine/Renderer/RenderUtilities/RenderConstants.hpp"
#include "Engine/FileUtilities/FileUtilities.hpp"

#include <sstream>



std::map<const char*, MonospaceFont*, CompareCStrings, UntrackedAllocator<std::pair<const char*, MonospaceFont*>>> MonospaceFont::s_MonospaceFontRegistry;



MonospaceFont::MonospaceFont(const char* monospaceFontName, const SamplerData& samplerData) :
m_FontSpriteSheet(SpriteSheet(monospaceFontName, 16, 16, samplerData))
{

}



MonospaceFont* MonospaceFont::GetMonospaceFontByName(const char* monospaceFontName)
{
	MonospaceFont* existingMonospaceFont = nullptr;
	auto fontRegistryIterator = s_MonospaceFontRegistry.find(monospaceFontName);
	if (fontRegistryIterator != s_MonospaceFontRegistry.end())
	{
		existingMonospaceFont = fontRegistryIterator->second;
	}

	return existingMonospaceFont;
}



MonospaceFont* MonospaceFont::CreateOrGetMonospaceFont(const char* monospaceFontName, const SamplerData& samplerData)
{
	MonospaceFont* newMonospaceFont = GetMonospaceFontByName(monospaceFontName);
	if (newMonospaceFont == nullptr)
	{
		newMonospaceFont = new MonospaceFont(monospaceFontName, samplerData);
		s_MonospaceFontRegistry[monospaceFontName] = newMonospaceFont;
	}

	return newMonospaceFont;
}



void MonospaceFont::DestroyAllMonospaceFonts()
{
	for (auto currentMonospaceFont = s_MonospaceFontRegistry.begin(); currentMonospaceFont != s_MonospaceFontRegistry.end();)
	{
		delete currentMonospaceFont->second;
		currentMonospaceFont = s_MonospaceFontRegistry.erase(currentMonospaceFont);
	}
}



AABB2 MonospaceFont::GetTextureCoordsForGlyph(int glyphUnicode) const
{
	return m_FontSpriteSheet.GetTextureCoordsForSpriteIndex(glyphUnicode);
}



Texture* MonospaceFont::GetFontTexture() const
{
	return m_FontSpriteSheet.GetSpriteSheet();
}



bool CompareKerningPairs::operator()(const IntVector2& firstIntVector, const IntVector2& secondIntVector) const
{
	if (firstIntVector.X < secondIntVector.X)
	{
		return true;
	}
	if (secondIntVector.X < firstIntVector.X)
	{
		return false;
	}

	return firstIntVector.Y < secondIntVector.Y;
}



std::map<const char*, ProportionalFont*, CompareCStrings, UntrackedAllocator<std::pair<const char*, ProportionalFont*>>> ProportionalFont::s_ProportionalFontRegistry;



ProportionalGlyph::ProportionalGlyph() :
m_glyphUnicode(0),
m_glyphTopLeft(IntVector2::ZERO),
m_glyphSize(IntVector2::ZERO),
m_glyphOffset(IntVector2::ZERO),
m_glyphAdvance(IntVector2::ZERO),
m_glyphTextureID(0)
{

}



Vector2 ProportionalGlyph::GetOffset() const
{
	Vector2 glyphOffset;

	glyphOffset.X = static_cast<float>(m_glyphOffset.X);
	glyphOffset.Y = static_cast<float>(m_glyphOffset.Y);

	return glyphOffset;
}



Vector2 ProportionalGlyph::GetSize() const
{
	Vector2 glyphSize;

	glyphSize.X = static_cast<float>(m_glyphSize.X);
	glyphSize.Y = static_cast<float>(m_glyphSize.Y);

	return glyphSize;
}



Vector2 ProportionalGlyph::GetAdvance() const
{
	Vector2 glyphAdvance;

	glyphAdvance.X = static_cast<float>(m_glyphAdvance.X);
	glyphAdvance.Y = static_cast<float>(m_glyphAdvance.Y);

	return glyphAdvance;
}



ProportionalFont::ProportionalFont(const char* proportionalFontName) :
m_TextureSize(Vector2::ZERO)
{
	std::string proportionalFontFile;

	bool fileFound = g_FileUtilities->LoadTextFileToBuffer(proportionalFontName, proportionalFontFile);
	if (fileFound)
	{
		std::stringstream fontFileStream(proportionalFontFile);
		std::vector<std::string> fontFileLines;

		std::string line;
		while (std::getline(fontFileStream, line))
		{
			fontFileLines.push_back(line);
		}

		for (auto& eachLine : fontFileLines)
		{
			std::stringstream fontFileLineStream(eachLine);
			std::vector<std::string> fontFileWords;

			std::string word;
			while (fontFileLineStream >> word)
			{
				fontFileWords.push_back(word);
			}

			auto fontFileWordIterator = fontFileWords.begin();
			std::string startingWord = *fontFileWordIterator;
			
			if (startingWord == "common")
			{
				for (auto& currentWord : fontFileWords)
				{
					size_t dataPosition = currentWord.find("=") + 1;

					if (currentWord.substr(0, 11) == "lineHeight=")
					{
						m_LineHeight = stof(currentWord.substr(dataPosition));
					}
					else if (currentWord.substr(0, 5) == "base=")
					{
						m_CharacterBaseHeight = stof(currentWord.substr(dataPosition));
					}
					else if (currentWord.substr(0, 7) == "scaleW=")
					{
						m_TextureSize.X = stof(currentWord.substr(dataPosition));
					}
					else if (currentWord.substr(0, 7) == "scaleH=")
					{
						m_TextureSize.Y = stof(currentWord.substr(dataPosition));
					}
				}
			}
			else if (startingWord == "page")
			{
				int currentTextureID = -1;
				Texture* currentTexture = nullptr;

				for (auto& currentWord : fontFileWords)
				{
					size_t dataPosition = currentWord.find("=") + 1;

					if (currentWord.substr(0, 3) == "id=")
					{
						currentTextureID = stoi(currentWord.substr(dataPosition));
					}
					else if (currentWord.substr(0, 5) == "file=")
					{
						std::string textureFileName = currentWord.substr(dataPosition);
						textureFileName.erase(0, 1);
						textureFileName.erase(textureFileName.size() - 1);

						std::string textureFilePath = textureFileName;
						std::string fontFileName = std::string(proportionalFontName);

						size_t directoryPathLastPosition = fontFileName.find_last_of("\\/") + 1;
						if (directoryPathLastPosition != std::string::npos)
						{
							std::string textureDirectoryPath = fontFileName.substr(0, directoryPathLastPosition);
							textureFilePath = textureDirectoryPath + textureFileName;
						}

						SamplerData samplerData = SamplerData(REPEAT_WRAP, REPEAT_WRAP, LINEAR_FILTER, NEAREST_FILTER);
						currentTexture = Texture::CreateOrGetTexture(textureFilePath.c_str(), samplerData);
					}
				}

				m_AllFontTextures[currentTextureID] = currentTexture;
			}
			else if (startingWord == "char")
			{
				ProportionalGlyph* currentGlyph = new ProportionalGlyph();
				int currentGlyphID;

				for (auto& currentWord : fontFileWords)
				{
					size_t dataPosition = currentWord.find("=") + 1;

					if (currentWord.substr(0, 3) == "id=")
					{
						currentGlyphID = stoi(currentWord.substr(dataPosition));
						currentGlyph->m_glyphUnicode = currentGlyphID;
					}
					else if (currentWord.substr(0, 2) == "x=")
					{
						currentGlyph->m_glyphTopLeft.X = stoi(currentWord.substr(dataPosition));
					}
					else if (currentWord.substr(0, 2) == "y=")
					{
						currentGlyph->m_glyphTopLeft.Y = stoi(currentWord.substr(dataPosition));
					}
					else if (currentWord.substr(0, 6) == "width=")
					{
						currentGlyph->m_glyphSize.X = stoi(currentWord.substr(dataPosition));
					}
					else if (currentWord.substr(0, 7) == "height=")
					{
						currentGlyph->m_glyphSize.Y = stoi(currentWord.substr(dataPosition));
					}
					else if (currentWord.substr(0, 8) == "xoffset=")
					{
						currentGlyph->m_glyphOffset.X = stoi(currentWord.substr(dataPosition));
					}
					else if (currentWord.substr(0, 8) == "yoffset=")
					{
						currentGlyph->m_glyphOffset.Y = stoi(currentWord.substr(dataPosition));
					}
					else if (currentWord.substr(0, 9) == "xadvance=")
					{
						currentGlyph->m_glyphAdvance.X = stoi(currentWord.substr(dataPosition));
					}
					else if (currentWord.substr(0, 9) == "yadvance=")
					{
						currentGlyph->m_glyphAdvance.Y = stoi(currentWord.substr(dataPosition));
					}
					else if (currentWord.substr(0, 5) == "page=")
					{
						currentGlyph->m_glyphTextureID = stoi(currentWord.substr(dataPosition));
					}
				}

				m_AllGlyphsForFont[currentGlyphID] = currentGlyph;
			}
			else if (startingWord == "kerning")
			{
				IntVector2 currentKerningPair = IntVector2::ZERO;
				int currentKerningAmount = 0;

				for (auto& currentWord : fontFileWords)
				{
					size_t dataPosition = currentWord.find("=") + 1;

					if (currentWord.substr(0, 6) == "first=")
					{
						currentKerningPair.X = stoi(currentWord.substr(dataPosition));
					}
					else if (currentWord.substr(0, 7) == "second=")
					{
						currentKerningPair.Y = stoi(currentWord.substr(dataPosition));
					}
					else if (currentWord.substr(0, 7) == "amount=")
					{
						currentKerningAmount = stoi(currentWord.substr(dataPosition));
					}
				}

				m_AllGlyphKernings[currentKerningPair] = currentKerningAmount;
			}
		}
	}
}



ProportionalFont::~ProportionalFont()
{
	for (auto currentGlyph : m_AllGlyphsForFont)
	{
		delete currentGlyph.second;
	}
}



ProportionalFont* ProportionalFont::GetProportionalFontByName(const char* proportionalFontName)
{
	ProportionalFont* existingProportionalFont = nullptr;
	auto fontRegistryIterator = s_ProportionalFontRegistry.find(proportionalFontName);
	if (fontRegistryIterator != s_ProportionalFontRegistry.end())
	{
		existingProportionalFont = fontRegistryIterator->second;
	}

	return existingProportionalFont;
}



ProportionalFont* ProportionalFont::CreateOrGetProportionalFont(const char* proportionalFontName)
{
	ProportionalFont* newProportionalFont = GetProportionalFontByName(proportionalFontName);
	if (newProportionalFont == nullptr)
	{
		char proportionalFontFileName[MAXIMUM_FONT_FILE_NAME_LENGTH] = "Data/Fonts/";
		strncat_s(proportionalFontFileName, proportionalFontName, strlen(proportionalFontName));

		const char* proportionalFontFileType = ".fnt";
		strncat_s(proportionalFontFileName, proportionalFontFileType, strlen(proportionalFontFileType));

		newProportionalFont = new ProportionalFont(proportionalFontFileName);
		s_ProportionalFontRegistry[proportionalFontName] = newProportionalFont;
	}

	return newProportionalFont;
}



void ProportionalFont::DestroyAllProportionalFonts()
{
	for (auto currentProportionalFont = s_ProportionalFontRegistry.begin(); currentProportionalFont != s_ProportionalFontRegistry.end();)
	{
		delete currentProportionalFont->second;
		currentProportionalFont = s_ProportionalFontRegistry.erase(currentProportionalFont);
	}
}



Texture* ProportionalFont::GetFontTextureForGlyph(const ProportionalGlyph& glyph) const
{
	Texture* glyphTexture = nullptr;
	auto fontSpriteSheetIterator = m_AllFontTextures.find(glyph.m_glyphTextureID);
	if (fontSpriteSheetIterator != m_AllFontTextures.end())
	{
		glyphTexture = fontSpriteSheetIterator->second;
	}

	return glyphTexture;
}



AABB2 ProportionalFont::GetTextureBoundsForGlyph(const ProportionalGlyph& glyph) const
{
	AABB2 textureBounds;
	textureBounds.minimums.X = static_cast<float>(glyph.m_glyphTopLeft.X);
	textureBounds.minimums.Y = static_cast<float>(glyph.m_glyphTopLeft.Y);

	textureBounds.maximums = textureBounds.minimums + glyph.GetSize();

	textureBounds.minimums.X /= m_TextureSize.X;
	textureBounds.minimums.Y /= m_TextureSize.Y;

	textureBounds.maximums.X /= m_TextureSize.X;
	textureBounds.maximums.Y /= m_TextureSize.Y;

	return textureBounds;
}



ProportionalGlyph* ProportionalFont::GetProportionalGlyphForGlyphUnicode(int glyphUnicode) const
{
	ProportionalGlyph* resultantGlyph = nullptr;

	auto glyphIterator = m_AllGlyphsForFont.find(glyphUnicode);
	if (glyphIterator != m_AllGlyphsForFont.end())
	{
		resultantGlyph = glyphIterator->second;
	}

	return resultantGlyph;
}



float ProportionalFont::GetKerningForGlyphPair(int firstGlyphUnicode, int secondGlyphUnicode) const
{
	float kerningForGlyphPair = 0.0f;
	auto glyphKerningIterator = m_AllGlyphKernings.find(IntVector2(firstGlyphUnicode, secondGlyphUnicode));
	if (glyphKerningIterator != m_AllGlyphKernings.end())
	{
		kerningForGlyphPair = static_cast<float>(glyphKerningIterator->second);
	}

	return kerningForGlyphPair;
}



float ProportionalFont::GetCharacterScaleForCellHeight(float cellHeight) const
{
	return (cellHeight / m_CharacterBaseHeight);
}



float ProportionalFont::GetLineSpacingForCellHeight(float cellHeight) const
{
	float characterScale = GetCharacterScaleForCellHeight(cellHeight);
	
	return characterScale * m_LineHeight;
}



float ProportionalFont::CalculateTextWidth(const char* asciiText, float cellHeight, const ProportionalFont* font /*= nullptr*/)
{
	float textWidth = 0.0f;
	float textScale = font->GetCharacterScaleForCellHeight(cellHeight);

	ProportionalGlyph* previousGlyph = nullptr;

	size_t textLength = strlen(asciiText);
	for (size_t glyphIndex = 0; glyphIndex < textLength; ++glyphIndex)
	{
		char asciiGlyph = asciiText[glyphIndex];
		ProportionalGlyph* currentGlyph = font->GetProportionalGlyphForGlyphUnicode(asciiGlyph);
		if (currentGlyph == nullptr)
		{
			continue;
		}

		float currentGlyphKerning = 0.0f;
		if (previousGlyph != nullptr && currentGlyph != nullptr)
		{
			currentGlyphKerning = font->GetKerningForGlyphPair(previousGlyph->m_glyphUnicode, currentGlyph->m_glyphUnicode);
		}

		textWidth += currentGlyphKerning * textScale;

		textWidth += currentGlyph->GetAdvance().X * textScale;
		previousGlyph = currentGlyph;
	}

	return textWidth;
}