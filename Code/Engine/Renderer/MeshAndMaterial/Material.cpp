#include <vector>

#include "Engine/Renderer/MeshAndMaterial/Material.hpp"
#include "Engine/Renderer/OpenGL/OpenGLExtensions.hpp"
#include "Engine/ErrorHandling/ErrorWarningAssert.hpp"
#include "Engine/IO Utilities/BinaryFileIO.hpp"



const size_t MAXIMUM_LOG_LENGTH = 1024U;
const size_t MAXIMUM_SHADER_FILE_SIZE = 16384U;



Material::Material() :
m_ShaderProgram(NULL),
m_DiffuseTexture(nullptr),
m_NormalTexture(nullptr),
m_SpecularTexture(nullptr),
m_AmbientTexture(nullptr),
m_EmissiveTexture(nullptr),
m_ColorTargetTexture(nullptr),
m_DepthStencilTexture(nullptr)
{

}



Material::Material(const char* vertexShaderFilePath, const char* fragmentShaderFilePath) :
m_ShaderProgram(NULL),
m_DiffuseTexture(nullptr),
m_NormalTexture(nullptr),
m_SpecularTexture(nullptr),
m_AmbientTexture(nullptr),
m_EmissiveTexture(nullptr),
m_ColorTargetTexture(nullptr),
m_DepthStencilTexture(nullptr)
{
	InitializeMaterial(vertexShaderFilePath, fragmentShaderFilePath);
}



void Material::SetIntToShaderProgram1D(const char* uniformName, const int* uniformArray, size_t numberOfElements /*= 1*/) const
{
	glUseProgram(m_ShaderProgram);
	int uniformLocation = glGetUniformLocation(m_ShaderProgram, uniformName);

	if (uniformLocation >= 0)
	{
		glUniform1iv(uniformLocation, numberOfElements, uniformArray);
	}

	glUseProgram(NULL);
}



void Material::SetIntToShaderProgram2D(const char* uniformName, const int* uniformArray, size_t numberOfElements /*= 1*/) const
{
	glUseProgram(m_ShaderProgram);
	int uniformLocation = glGetUniformLocation(m_ShaderProgram, uniformName);

	if (uniformLocation >= 0)
	{
		glUniform2iv(uniformLocation, numberOfElements, uniformArray);
	}

	glUseProgram(NULL);
}



void Material::SetIntToShaderProgram3D(const char* uniformName, const int* uniformArray, size_t numberOfElements /*= 1*/) const
{
	glUseProgram(m_ShaderProgram);
	int uniformLocation = glGetUniformLocation(m_ShaderProgram, uniformName);

	if (uniformLocation >= 0)
	{
		glUniform3iv(uniformLocation, numberOfElements, uniformArray);
	}

	glUseProgram(NULL);
}



void Material::SetIntToShaderProgram4D(const char* uniformName, const int* uniformArray, size_t numberOfElements /*= 1*/) const
{
	glUseProgram(m_ShaderProgram);
	int uniformLocation = glGetUniformLocation(m_ShaderProgram, uniformName);

	if (uniformLocation >= 0)
	{
		glUniform4iv(uniformLocation, numberOfElements, uniformArray);
	}

	glUseProgram(NULL);
}



void Material::SetFloatToShaderProgram1D(const char* uniformName, const float* uniformArray, size_t numberOfElements /*= 1*/) const
{
	glUseProgram(m_ShaderProgram);
	int uniformLocation = glGetUniformLocation(m_ShaderProgram, uniformName);

	if (uniformLocation >= 0)
	{
		glUniform1fv(uniformLocation, numberOfElements, uniformArray);
	}

	glUseProgram(NULL);
}



void Material::SetFloatToShaderProgram2D(const char* uniformName, const float* uniformArray, size_t numberOfElements /*= 1*/) const
{
	glUseProgram(m_ShaderProgram);
	int uniformLocation = glGetUniformLocation(m_ShaderProgram, uniformName);

	if (uniformLocation >= 0)
	{
		glUniform2fv(uniformLocation, numberOfElements, uniformArray);
	}

	glUseProgram(NULL);
}



void Material::SetFloatToShaderProgram3D(const char* uniformName, const float* uniformArray, size_t numberOfElements /*= 1*/) const
{
	glUseProgram(m_ShaderProgram);
	int uniformLocation = glGetUniformLocation(m_ShaderProgram, uniformName);

	if (uniformLocation >= 0)
	{
		glUniform3fv(uniformLocation, numberOfElements, uniformArray);
	}

	glUseProgram(NULL);
}



void Material::SetFloatToShaderProgram4D(const char* uniformName, const float* uniformArray, size_t numberOfElements /*= 1*/) const
{
	glUseProgram(m_ShaderProgram);
	int uniformLocation = glGetUniformLocation(m_ShaderProgram, uniformName);

	if (uniformLocation >= 0)
	{
		glUniform4fv(uniformLocation, numberOfElements, uniformArray);
	}

	glUseProgram(NULL);
}



void Material::SetMatrix4ToShaderProgram(const char* uniformName, const float* uniformArray, size_t numberOfElements /*= 1*/) const
{
	glUseProgram(m_ShaderProgram);
	int uniformLocation = glGetUniformLocation(m_ShaderProgram, uniformName);

	if (uniformLocation >= 0)
	{
		glUniformMatrix4fv(uniformLocation, numberOfElements, true, uniformArray);
	}

	glUseProgram(NULL);
}



void Material::BindFloatAttributeFromShaderProgram(const char* attributeName, int attributeSize, uint32_t attributeType, uint8_t normalizeAttribute, int attributeStride, int attributeOffset) const
{
	int attributeLocation = glGetAttribLocation(m_ShaderProgram, attributeName);
	if (attributeLocation >= 0)
	{
		glEnableVertexAttribArray(attributeLocation);
		glVertexAttribPointer(attributeLocation, attributeSize, attributeType, normalizeAttribute, attributeStride, (void*)attributeOffset);
	}
}



void Material::BindIntAttributeFromShaderProgram(const char* attributeName, int attributeSize, uint32_t attributeType, int attributeStride, int attributeOffset) const
{
	int attributeLocation = glGetAttribLocation(m_ShaderProgram, attributeName);
	if (attributeLocation >= 0)
	{
		glEnableVertexAttribArray(attributeLocation);
		glVertexAttribIPointer(attributeLocation, attributeSize, attributeType, attributeStride, (void*)attributeOffset);
	}
}



uint32_t Material::GetShaderProgramID()
{
	return m_ShaderProgram;
}



void Material::SetDiffuseTexture(const char* diffuseTextureFilePath, const SamplerData& samplerData)
{
	m_DiffuseTexture = Texture::CreateOrGetTexture(diffuseTextureFilePath, samplerData);
}



void Material::SetDiffuseTexture(Texture* diffuseTexture)
{
	m_DiffuseTexture = diffuseTexture;
}



Texture* Material::GetDiffuseTexture() const
{
	return m_DiffuseTexture;
}



void Material::SetNormalTexture(const char* normalTextureFilePath, const SamplerData& samplerData)
{
	m_NormalTexture = Texture::CreateOrGetTexture(normalTextureFilePath, samplerData);
}



void Material::SetNormalTexture(Texture* normalTexture)
{
	m_NormalTexture = normalTexture;
}



Texture* Material::GetNormalTexture() const
{
	return m_NormalTexture;
}



void Material::SetSpecularTexture(const char* specularTextureFilePath, const SamplerData& samplerData)
{
	m_SpecularTexture = Texture::CreateOrGetTexture(specularTextureFilePath, samplerData);
}



void Material::SetSpecularTexture(Texture* specularTexture)
{
	m_SpecularTexture = specularTexture;
}



Texture* Material::GetSpecularTexture() const
{
	return m_SpecularTexture;
}



void Material::SetAmbientTexture(const char* ambientTextureFilePath, const SamplerData& samplerData)
{
	m_AmbientTexture = Texture::CreateOrGetTexture(ambientTextureFilePath, samplerData);
}



void Material::SetAmbientTexture(Texture* ambientTexture)
{
	m_AmbientTexture = ambientTexture;
}



Texture* Material::GetAmbientTexture() const
{
	return m_AmbientTexture;
}



void Material::SetEmissiveTexture(const char* emissiveTextureFilePath, const SamplerData& samplerData)
{
	m_EmissiveTexture = Texture::CreateOrGetTexture(emissiveTextureFilePath, samplerData);
}



void Material::SetEmissiveTexture(Texture* emissiveTexture)
{
	m_EmissiveTexture = emissiveTexture;
}



Texture* Material::GetEmissiveTexture() const
{
	return m_EmissiveTexture;
}



void Material::SetColorTargetTextures(Texture** colorTargetTextures, size_t numberOfColorTargetTextures)
{
	m_ColorTargetTexture = colorTargetTextures;
	m_NumberOfColorTargetTextures = numberOfColorTargetTextures;
}



Texture** Material::GetColorTargetTextures() const
{
	return m_ColorTargetTexture;
}



void Material::SetDepthStencilTexture(Texture* depthStencilTexture)
{
	m_DepthStencilTexture = depthStencilTexture;
}



Texture* Material::GetDepthStencilTexture() const
{
	return m_DepthStencilTexture;
}



size_t Material::GetNumberOfColorTargetTextures() const
{
	return m_NumberOfColorTargetTextures;
}



void Material::InitializeMaterial(const char* vertexShaderFilePath, const char* fragmentShaderFilePath)
{
	uint32_t vertexShaderID = LoadVertexShader(vertexShaderFilePath);
	uint32_t fragmentShaderID = LoadFragmentShader(fragmentShaderFilePath);

	CreateAndLinkShaderProgram(vertexShaderID, fragmentShaderID);

	DeleteShader(vertexShaderID);
	DeleteShader(fragmentShaderID);
}



uint32_t Material::LoadVertexShader(const char* vertexShaderFilePath)
{
	uint8_t fileBuffer[MAXIMUM_SHADER_FILE_SIZE];
	BinaryFileReader fileReader;

	fileReader.OpenBinaryFile(vertexShaderFilePath);
	size_t fileSize = fileReader.GetBinaryFileSize();
	fileSize = fileReader.ReadBytes(fileBuffer, fileSize);

	ASSERT_OR_DIE(fileSize < MAXIMUM_SHADER_FILE_SIZE, "File size exceeded.");

	fileBuffer[fileSize] = '\0';
	++fileSize;

	uint32_t shaderID = glCreateShader(GL_VERTEX_SHADER);
	if (shaderID == NULL)
	{
		ERROR_RECOVERABLE("Invalid Shader ID");
	}

	char* shaderSourceData = (char*)&fileBuffer;
	int shaderSourceLength = (int)fileSize;
	glShaderSource(shaderID, 1, &shaderSourceData, &shaderSourceLength);

	glCompileShader(shaderID);

	int shaderStatus;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &shaderStatus);
	if (shaderStatus == NULL)
	{
		WriteErrorToShaderLog(shaderID, vertexShaderFilePath);
		ERROR_RECOVERABLE("Failed to compile Shader.");
	}

	return shaderID;
}



uint32_t Material::LoadFragmentShader(const char* fragmentShaderFilePath)
{
	uint8_t fileBuffer[MAXIMUM_SHADER_FILE_SIZE];
	BinaryFileReader fileReader;

	fileReader.OpenBinaryFile(fragmentShaderFilePath);
	size_t fileSize = fileReader.GetBinaryFileSize();
	fileSize = fileReader.ReadBytes(fileBuffer, fileSize);

	ASSERT_OR_DIE(fileSize < MAXIMUM_SHADER_FILE_SIZE, "File size exceeded.");

	fileBuffer[fileSize] = '\0';
	++fileSize;

	uint32_t shaderID = glCreateShader(GL_FRAGMENT_SHADER);
	if (shaderID == NULL)
	{
		ERROR_RECOVERABLE("Invalid Shader ID");
	}

	char* shaderSourceData = (char*)&fileBuffer;
	int shaderSourceLength = (int)fileSize;
	glShaderSource(shaderID, 1, &shaderSourceData, &shaderSourceLength);

	glCompileShader(shaderID);

	int shaderStatus;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &shaderStatus);
	if (shaderStatus == NULL)
	{
		WriteErrorToShaderLog(shaderID, fragmentShaderFilePath);
		ERROR_RECOVERABLE("Failed to compile Shader.");
	}

	return shaderID;
}



void Material::DeleteShader(uint32_t shaderID)
{
	glDeleteShader(shaderID);
}



void Material::CreateAndLinkShaderProgram(uint32_t vertexShaderID, uint32_t fragmentShaderID)
{
	m_ShaderProgram = glCreateProgram();
	if (m_ShaderProgram == NULL)
	{
		ERROR_RECOVERABLE("Invalid Program ID.");
	}

	glAttachShader(m_ShaderProgram, vertexShaderID);
	glAttachShader(m_ShaderProgram, fragmentShaderID);

	glLinkProgram(m_ShaderProgram);

	int programStatus;
	glGetProgramiv(m_ShaderProgram, GL_LINK_STATUS, &programStatus);
	if (programStatus == NULL)
	{
		WriteErrorToProgramLog(m_ShaderProgram);
		ERROR_RECOVERABLE("Failed to link Shader Program.");
	}

	glDetachShader(m_ShaderProgram, vertexShaderID);
	glDetachShader(m_ShaderProgram, fragmentShaderID);
}



void Material::WriteErrorToShaderLog(uint32_t shaderID, const char* shaderFilePath)
{
	size_t logLength = 0U;
	glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, (int*)&logLength);

	char shaderInfoLog[MAXIMUM_LOG_LENGTH];
	glGetShaderInfoLog(shaderID, MAXIMUM_LOG_LENGTH, (int*)&logLength, shaderInfoLog);

	sprintf_s(shaderInfoLog, "%s\t%s", shaderInfoLog, shaderFilePath);
	logLength = strlen(shaderInfoLog);

	BinaryFileWriter fileWriter;
	fileWriter.OpenBinaryFile("Logs/ShaderInfoLog.txt");
	fileWriter.WriteBytes(shaderInfoLog, logLength);
	fileWriter.CloseBinaryFile();
}



void Material::WriteErrorToProgramLog(uint32_t programID)
{
	size_t logLength = 0U;
	glGetProgramiv(programID, GL_INFO_LOG_LENGTH, (int*)&logLength);

	char programInfoLog[MAXIMUM_LOG_LENGTH];
	glGetProgramInfoLog(programID, MAXIMUM_LOG_LENGTH, (int*)&logLength, programInfoLog);

	BinaryFileWriter fileWriter;
	fileWriter.OpenBinaryFile("Logs/ProgramInfoLog.txt");
	fileWriter.WriteBytes(programInfoLog, logLength);
	fileWriter.CloseBinaryFile();
}