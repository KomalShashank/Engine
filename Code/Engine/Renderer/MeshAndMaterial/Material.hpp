#pragma once

#include "Engine/Renderer/Texture/Texture.hpp"



class Material
{
public:
	Material();
	Material(const char* vertexShaderFilePath, const char* fragmentShaderFilePath);

	void SetIntToShaderProgram1D(const char* uniformName, const int* uniformArray, size_t numberOfElements = 1) const;
	void SetIntToShaderProgram2D(const char* uniformName, const int* uniformArray, size_t numberOfElements = 1) const;
	void SetIntToShaderProgram3D(const char* uniformName, const int* uniformArray, size_t numberOfElements = 1) const;
	void SetIntToShaderProgram4D(const char* uniformName, const int* uniformArray, size_t numberOfElements = 1) const;

	void SetFloatToShaderProgram1D(const char* uniformName, const float* uniformArray, size_t numberOfElements = 1) const;
	void SetFloatToShaderProgram2D(const char* uniformName, const float* uniformArray, size_t numberOfElements = 1) const;
	void SetFloatToShaderProgram3D(const char* uniformName, const float* uniformArray, size_t numberOfElements = 1) const;
	void SetFloatToShaderProgram4D(const char* uniformName, const float* uniformArray, size_t numberOfElements = 1) const;

	void SetMatrix4ToShaderProgram(const char* uniformName, const float* uniformArray, size_t numberOfElements = 1) const;

	void BindFloatAttributeFromShaderProgram(const char* attributeName, int attributeSize, uint32_t attributeType, uint8_t normalizeAttribute, int attributeStride, int attributeOffset) const;
	void BindIntAttributeFromShaderProgram(const char* attributeName, int attributeSize, uint32_t attributeType, int attributeStride, int attributeOffset) const;

	uint32_t GetShaderProgramID();

	void SetDiffuseTexture(const char* diffuseTextureFilePath, const SamplerData& samplerData);
	void SetDiffuseTexture(Texture* diffuseTexture);
	Texture* GetDiffuseTexture() const;

	void SetNormalTexture(const char* normalTextureFilePath, const SamplerData& samplerData);
	void SetNormalTexture(Texture* normalTexture);
	Texture* GetNormalTexture() const;
	
	void SetSpecularTexture(const char* specularTextureFilePath, const SamplerData& samplerData);
	void SetSpecularTexture(Texture* specularTexture);
	Texture* GetSpecularTexture() const;
	
	void SetAmbientTexture(const char* ambientTextureFilePath, const SamplerData& samplerData);
	void SetAmbientTexture(Texture* ambientTexture);
	Texture* GetAmbientTexture() const;
	
	void SetEmissiveTexture(const char* emissiveTextureFilePath, const SamplerData& samplerData);
	void SetEmissiveTexture(Texture* emissiveTexture);
	Texture* GetEmissiveTexture() const;

	void SetColorTargetTextures(Texture** colorTargetTextures, size_t numberOfColorTargetTextures);
	Texture** GetColorTargetTextures() const;

	void SetDepthStencilTexture(Texture* depthStencilTexture);
	Texture* GetDepthStencilTexture() const;

	size_t GetNumberOfColorTargetTextures() const;

private:
	void InitializeMaterial(const char* vertexShaderFilePath, const char* fragmentShaderFilePath);

	uint32_t LoadVertexShader(const char* vertexShaderFilePath);
	uint32_t LoadFragmentShader(const char* fragmentShaderFilePath);
	void DeleteShader(uint32_t shaderID);

	void CreateAndLinkShaderProgram(uint32_t vertexShaderID, uint32_t fragmentShaderID);

	void WriteErrorToShaderLog(uint32_t shaderID, const char* shaderFilePath);
	void WriteErrorToProgramLog(uint32_t programID);

private:
	uint32_t m_ShaderProgram;

	Texture* m_DiffuseTexture;
	Texture* m_NormalTexture;
	Texture* m_SpecularTexture;
	Texture* m_AmbientTexture;
	Texture* m_EmissiveTexture;

	Texture** m_ColorTargetTexture;
	Texture* m_DepthStencilTexture;

	size_t m_NumberOfColorTargetTextures;
};