#include "Engine/Renderer/MeshAndMaterial/MeshRenderer.hpp"
#include "Engine/Renderer/OpenGL/OpenGLExtensions.hpp"
#include "Engine/ErrorHandling/ErrorWarningAssert.hpp"



MeshRenderer::MeshRenderer() :
m_VertexArrayObject(NULL),
m_Mesh(nullptr),
m_Material(nullptr)
{

}



MeshRenderer::MeshRenderer(Mesh* mesh, Material* material) :
m_VertexArrayObject(NULL),
m_Mesh(mesh),
m_Material(material)
{
	InitializeMeshRenderer();
}



void MeshRenderer::SetMesh(Mesh* mesh)
{
	m_Mesh = mesh;
}



void MeshRenderer::SetMaterial(Material* material)
{
	m_Material = material;
	BindToVertexArrayObject();
}



void MeshRenderer::RenderMeshWithMaterial(const Matrix4& modelMatrix, const Matrix4& viewMatrix, const Matrix4& projectionMatrix) const
{
	SetModelViewProjectionMatrices(modelMatrix, viewMatrix, projectionMatrix);
	GenerateMaterial();
	RenderMesh();
}



void MeshRenderer::InitializeMeshRenderer()
{
	glGenVertexArrays(1, &m_VertexArrayObject);
	if (m_VertexArrayObject == NULL)
	{
		ERROR_RECOVERABLE("VAO failed to generate.");
	}

	BindToVertexArrayObject();
}



void MeshRenderer::BindToVertexArrayObject()
{
	glBindVertexArray(m_VertexArrayObject);
	glBindBuffer(GL_ARRAY_BUFFER, m_Mesh->m_VertexBufferObject->GetRenderBufferID());

	m_Material->BindFloatAttributeFromShaderProgram("inPosition", 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), offsetof(Vertex3D, m_Position));
	m_Material->BindFloatAttributeFromShaderProgram("inColor", 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex3D), offsetof(Vertex3D, m_Color));
	m_Material->BindFloatAttributeFromShaderProgram("inTextureCoordinates", 2, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), offsetof(Vertex3D, m_TextureCoordinates));
	m_Material->BindFloatAttributeFromShaderProgram("inTangent", 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), offsetof(Vertex3D, m_Tangent));
	m_Material->BindFloatAttributeFromShaderProgram("inBitangent", 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), offsetof(Vertex3D, m_Bitangent));
	m_Material->BindFloatAttributeFromShaderProgram("inNormal", 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), offsetof(Vertex3D, m_Normal));
	m_Material->BindFloatAttributeFromShaderProgram("inBoneWeights", 4, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), offsetof(Vertex3D, m_BoneWeights));
	m_Material->BindIntAttributeFromShaderProgram("inBoneIndices", 4, GL_UNSIGNED_INT, sizeof(Vertex3D), offsetof(Vertex3D, m_BoneIndices));

	glBindBuffer(GL_ARRAY_BUFFER, NULL);

	if (m_Mesh->m_IndexBufferObject != nullptr)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Mesh->m_IndexBufferObject->GetRenderBufferID());
	}

	glBindVertexArray(NULL);
}



void MeshRenderer::SetModelViewProjectionMatrices(const Matrix4& modelMatrix, const Matrix4& viewMatrix, const Matrix4& projectionMatrix) const
{
	m_Material->SetMatrix4ToShaderProgram("g_Model", modelMatrix.m_Matrix4);
	m_Material->SetMatrix4ToShaderProgram("g_View", viewMatrix.m_Matrix4);
	m_Material->SetMatrix4ToShaderProgram("g_Projection", projectionMatrix.m_Matrix4);
}



void MeshRenderer::GenerateMaterial() const
{
	unsigned int textureIndex = 0;

	glEnable(GL_TEXTURE_2D);

	Texture* diffuseTexture = m_Material->GetDiffuseTexture();
	if (diffuseTexture != nullptr)
	{
		glActiveTexture(GL_TEXTURE0 + textureIndex);
		glBindTexture(GL_TEXTURE_2D, diffuseTexture->m_TextureID);
		glBindSampler(textureIndex, diffuseTexture->m_SamplerID);
		m_Material->SetIntToShaderProgram1D("g_DiffuseTexture", (int*)&textureIndex);
		++textureIndex;
	}

	Texture* normalTexture = m_Material->GetNormalTexture();
	if (normalTexture != nullptr)
	{
		glActiveTexture(GL_TEXTURE0 + textureIndex);
		glBindTexture(GL_TEXTURE_2D, normalTexture->m_TextureID);
		glBindSampler(textureIndex, normalTexture->m_SamplerID);
		m_Material->SetIntToShaderProgram1D("g_NormalTexture", (int*)&textureIndex);
		++textureIndex;
	}

	Texture* specularTexture = m_Material->GetSpecularTexture();
	if (specularTexture != nullptr)
	{
		glActiveTexture(GL_TEXTURE0 + textureIndex);
		glBindTexture(GL_TEXTURE_2D, specularTexture->m_TextureID);
		glBindSampler(textureIndex, specularTexture->m_SamplerID);
		m_Material->SetIntToShaderProgram1D("g_SpecularTexture", (int*)&textureIndex);
		++textureIndex;
	}

	Texture* ambientTexture = m_Material->GetAmbientTexture();
	if (ambientTexture != nullptr)
	{
		glActiveTexture(GL_TEXTURE0 + textureIndex);
		glBindTexture(GL_TEXTURE_2D, ambientTexture->m_TextureID);
		glBindSampler(textureIndex, ambientTexture->m_SamplerID);
		m_Material->SetIntToShaderProgram1D("g_AmbientTexture", (int*)&textureIndex);
		++textureIndex;
	}

	Texture* emissiveTexture = m_Material->GetEmissiveTexture();
	if (emissiveTexture != nullptr)
	{
		glActiveTexture(GL_TEXTURE0 + textureIndex);
		glBindTexture(GL_TEXTURE_2D, emissiveTexture->m_TextureID);
		glBindSampler(textureIndex, emissiveTexture->m_SamplerID);
		m_Material->SetIntToShaderProgram1D("g_EmissiveTexture", (int*)&textureIndex);
		++textureIndex;
	}

	Texture** colorTargetTextures = m_Material->GetColorTargetTextures();
	if (colorTargetTextures != nullptr)
	{
		int textureIndices[8U];

		size_t numberOfColorTargetTextures = m_Material->GetNumberOfColorTargetTextures();
		for (size_t colorTargetIndex = 0; colorTargetIndex < numberOfColorTargetTextures; ++colorTargetIndex)
		{
			Texture* currentColorTargetTexture = colorTargetTextures[colorTargetIndex];

			glActiveTexture(GL_TEXTURE0 + textureIndex);
			glBindTexture(GL_TEXTURE_2D, currentColorTargetTexture->m_TextureID);
			glBindSampler(textureIndex, currentColorTargetTexture->m_SamplerID);

			textureIndices[colorTargetIndex] = textureIndex;
			++textureIndex;
		}
		
		m_Material->SetIntToShaderProgram1D("g_ColorTargets[0]", (int*)&textureIndices[0], numberOfColorTargetTextures);
	}

	Texture* depthStencilTexture = m_Material->GetDepthStencilTexture();
	if (depthStencilTexture != nullptr)
	{
		glActiveTexture(GL_TEXTURE0 + textureIndex);
		glBindTexture(GL_TEXTURE_2D, depthStencilTexture->m_TextureID);
		glBindSampler(textureIndex, depthStencilTexture->m_SamplerID);
		m_Material->SetIntToShaderProgram1D("g_DepthStencil", (int*)&textureIndex);
		++textureIndex;
	}

	glDisable(GL_TEXTURE_2D);
}



void MeshRenderer::RenderMesh() const
{
	glBindVertexArray(m_VertexArrayObject);
	glUseProgram(m_Material->GetShaderProgramID());

	m_Mesh->RenderUsingInstructions();

	glUseProgram(NULL);
	glBindVertexArray(NULL);
}