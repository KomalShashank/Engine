#include "Engine/Renderer/OpenGL/OpenGLExtensions.hpp"
#include "Engine/Renderer/Texture/Texture.hpp"
#include "Engine/Renderer/RenderUtilities/RenderConstants.hpp"
#include "ThirdParty/stb/STB_Image.hpp"
#include "Engine/ErrorHandling/ErrorWarningAssert.hpp"



std::map<const char*, Texture*, CompareCStrings, UntrackedAllocator<std::pair<const char*, Texture*>>> Texture::s_TextureRegistry;



SamplerData::SamplerData()
{

}



SamplerData::SamplerData(unsigned int horizontalWrapMode, unsigned int verticalWrapMode, unsigned int minificationFilter, unsigned int magnificationFilter) :
m_HorizontalWrapMode(horizontalWrapMode),
m_VerticalWrapMode(verticalWrapMode),
m_MinificationFilter(minificationFilter),
m_MagnificationFilter(magnificationFilter)
{

}



Texture::Texture(const char* imageFilePath, const SamplerData& samplerData) :
m_TextureID(0),
m_TexelSize(0, 0),
m_SamplerID(0)
{
	int numberOfComponents = 0;
	int numberOfComponentsRequested = 0;
	unsigned char* imageData = stbi_load(imageFilePath, &m_TexelSize.X, &m_TexelSize.Y, &numberOfComponents, numberOfComponentsRequested);
	GUARANTEE_OR_DIE(imageData != nullptr, "Image Does Not Exist");

	glEnable(GL_TEXTURE_2D);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, (GLuint*)&m_TextureID);
	glBindTexture(GL_TEXTURE_2D, m_TextureID);

	CreateSampler(samplerData);

	GLenum bufferFormat = GL_RGBA;
	if (numberOfComponents == 3)
	{
		bufferFormat = GL_RGB;
	}
	GLenum internalFormat = bufferFormat;

	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_TexelSize.X, m_TexelSize.Y, 0, bufferFormat, GL_UNSIGNED_BYTE, imageData);
	stbi_image_free(imageData);
	glDisable(GL_TEXTURE_2D);
}



Texture::Texture(const IntVector2& texelSize, const TextureFormat& texelFormat, const SamplerData& samplerData, bool hasColorData /*= false*/, const RGBA& texelColor /*= RGBA::WHITE*/)
{
	glGenTextures(1, (GLuint*)&m_TextureID);

	GLenum bufferChannels;
	GLenum bufferFormat;
	GLenum internalFormat;

	switch (texelFormat)
	{
	case RGBA_8:
		bufferChannels = GL_RGBA;
		bufferFormat = GL_UNSIGNED_INT_8_8_8_8;
		internalFormat = GL_RGBA8;

		break;

	case D_24S_8:
		bufferChannels = GL_DEPTH_STENCIL;
		bufferFormat = GL_UNSIGNED_INT_24_8;
		internalFormat = GL_DEPTH24_STENCIL8;
		
		break;

	default:
		return;
	}

	CreateSampler(samplerData);

	unsigned char* pixelData = nullptr;
	std::vector<unsigned char> texelData;

	if (hasColorData)
	{
		GetTexelDataForColor(texelColor, texelData, texelSize.X * texelSize.Y);
		pixelData = texelData.data();
	}

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_TextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, texelSize.X, texelSize.Y, 0, bufferChannels, bufferFormat, pixelData);

	m_TexelSize = texelSize;
}



Texture* Texture::GetTextureByName(const char* imageFilePath)
{
	Texture* existingTexture = nullptr;
	auto textureRegistryIterator = s_TextureRegistry.find(imageFilePath);
	if (textureRegistryIterator != s_TextureRegistry.end())
	{
		existingTexture = textureRegistryIterator->second;
	}

	return existingTexture;
}



void Texture::GetTexelDataForColor(const RGBA& texelColor, std::vector<unsigned char>& texelData, size_t numberOfTexels)
{
	std::vector<unsigned char> colorData;
	colorData.resize(sizeof(RGBA));

	colorData[0] = texelColor.m_Red;
	colorData[1] = texelColor.m_Green;
	colorData[2] = texelColor.m_Blue;
	colorData[3] = texelColor.m_Alpha;

	for (size_t texelIndex = 0; texelIndex < numberOfTexels; ++texelIndex)
	{
		texelData.insert(texelData.end(), colorData.begin(), colorData.end());
	}
}



void Texture::CreateSampler(const SamplerData& samplerData)
{
	glGenSamplers(1, &m_SamplerID);

	glSamplerParameteri(m_SamplerID, GL_TEXTURE_WRAP_S, samplerData.m_HorizontalWrapMode);
	glSamplerParameteri(m_SamplerID, GL_TEXTURE_WRAP_T, samplerData.m_VerticalWrapMode);
	glSamplerParameteri(m_SamplerID, GL_TEXTURE_MIN_FILTER, samplerData.m_MinificationFilter);
	glSamplerParameteri(m_SamplerID, GL_TEXTURE_MAG_FILTER, samplerData.m_MagnificationFilter);
}



Texture* Texture::CreateOrGetTexture(const char* imageFilePath, const SamplerData& samplerData)
{
	Texture* newTexture = GetTextureByName(imageFilePath);
	if (newTexture == nullptr)
	{
		newTexture = new Texture(imageFilePath, samplerData);
		s_TextureRegistry[imageFilePath] = newTexture;
	}

	return newTexture;
}



Texture* Texture::CreateDefaultTexture(const IntVector2& texelSize, const TextureFormat& texelFormat, const SamplerData& samplerData, bool hasColorData /*= false*/, const RGBA& texelColor /*= RGBA::WHITE*/)
{
	Texture* newDefaultTexture = new Texture(texelSize, texelFormat, samplerData, hasColorData, texelColor);

	return newDefaultTexture;
}



void Texture::DestroyAllTextures()
{
	for (auto currentTexture = s_TextureRegistry.begin(); currentTexture != s_TextureRegistry.end();)
	{
		delete currentTexture->second;
		currentTexture = s_TextureRegistry.erase(currentTexture);
	}
}