#include <vector>

#include "Engine/Renderer/SpriteRendering/Sprite.hpp"
#include "Engine/Renderer/SpriteRendering/SpriteDatabase.hpp"
#include "Engine/Renderer/SpriteRendering/SpriteRenderer.hpp"
#include "Engine/Renderer/RenderUtilities/AdvancedRenderer.hpp"
#include "Engine/Renderer/Vertex/Vertex.hpp"
#include "Engine/ErrorHandling/ErrorWarningAssert.hpp"
#include "Engine/Math/MathUtilities/MathUtilities.hpp"



const size_t NUMBER_OF_VERTICES = 4;
const size_t NUMBER_OF_INDICES = 6;



Sprite::Sprite() :
m_Scale(Vector2::ONE),
m_Rotation(0.0f),
m_Position(Vector2::ZERO),
m_LayerID(0),
m_IsEnabled(false)
{
	m_Mesh = new Mesh();
}



Sprite::Sprite(const char* spriteResourceID) :
m_Scale(Vector2::ONE),
m_Rotation(0.0f),
m_Position(Vector2::ZERO),
m_LayerID(0),
m_IsEnabled(false)
{
	m_SpriteResource = SpriteDatabase::SingletonInstance()->GetConstantSpriteResource(spriteResourceID);
	ASSERT_OR_DIE(m_SpriteResource != nullptr, "No Sprite Resource exists for this Sprite.");

	m_Mesh = new Mesh();
	m_Material = m_SpriteResource->m_Material;
}



Sprite::~Sprite()
{
	SpriteRenderer::SingletonInstance()->UnregisterSprite(this);

	delete m_Mesh;
}



void Sprite::SetSpriteScale(const Vector2& spriteScale)
{
	m_Scale = spriteScale;
}



Vector2 Sprite::GetSpriteScale() const
{
	return m_Scale;
}



void Sprite::SetSpriteRotation(float spriteRotation)
{
	m_Rotation = spriteRotation;
}



float Sprite::GetSpriteRotation() const
{
	return m_Rotation;
}



void Sprite::SetSpritePosition(const Vector2& spritePosition)
{
	m_Position = spritePosition;
}



Vector2 Sprite::GetSpritePosition() const
{
	return m_Position;
}



AABB2 Sprite::GetSpriteWorldBounds() const
{
	Vector2 spriteMinimums;
	Vector2 spriteMaximums;

	spriteMinimums.X = m_Position.X - (m_SpriteResource->m_Pivot.X * m_Scale.X);
	spriteMinimums.Y = m_Position.Y - (m_SpriteResource->m_Pivot.Y * m_Scale.Y);
	
	spriteMaximums.X = spriteMinimums.X + (m_SpriteResource->m_Dimensions.X * m_Scale.X);
	spriteMaximums.Y = spriteMinimums.Y + (m_SpriteResource->m_Dimensions.Y * m_Scale.Y);

	return AABB2(spriteMinimums, spriteMaximums);
}



Vector2 Sprite::GetSpriteDimensions() const
{
	float spriteWidth = m_SpriteResource->m_Dimensions.X * m_Scale.X;
	float spriteHeight = m_SpriteResource->m_Dimensions.Y * m_Scale.Y;

	return Vector2(spriteWidth, spriteHeight);
}



void Sprite::SetLayerID(uint8_t layerID)
{
	if (layerID == m_LayerID)
	{
		return;
	}

	if (m_IsEnabled)
	{
		SpriteRenderer::SingletonInstance()->UnregisterSprite(this);
		m_LayerID = layerID;
		SpriteRenderer::SingletonInstance()->RegisterSprite(this);
	}
	else
	{
		m_LayerID = layerID;
	}
}



uint8_t Sprite::GetLayerID() const
{
	return m_LayerID;
}



void Sprite::EnableSprite()
{
	if (!m_IsEnabled)
	{
		m_IsEnabled = true;
		SpriteRenderer::SingletonInstance()->RegisterSprite(this);
	}
}



void Sprite::DisableSprite()
{
	if (m_IsEnabled)
	{
		m_IsEnabled = false;
		SpriteRenderer::SingletonInstance()->UnregisterSprite(this);
	}
}



bool Sprite::IsSpriteEnabled() const
{
	return m_IsEnabled;
}



void Sprite::SetMaterial(Material* spriteMaterial)
{
	m_Material = spriteMaterial;
}



void Sprite::Render() const
{
	UpdateSpriteMesh();
	m_Material->SetDiffuseTexture(m_SpriteResource->m_Texture);
	AdvancedRenderer::SingletonInstance()->DrawPolygonMesh(m_Mesh, NUMBER_OF_VERTICES, NUMBER_OF_INDICES, m_Material);
}



void Sprite::UpdateSpriteMesh() const
{
	Vertex3D spriteVertices[NUMBER_OF_VERTICES];
	uint32_t spriteIndices[NUMBER_OF_INDICES] = { 0, 1, 2, 2, 3, 0 };

	Vertex3D spriteVertex;
	spriteVertex.m_Color = RGBA::WHITE;

	Vector2 spriteMinimums = m_SpriteResource->m_Pivot.GetNegatedVector2();
	Vector2 spriteMaximums = spriteMinimums + m_SpriteResource->m_Dimensions;

	Vector2 spriteTextureMinimums = m_SpriteResource->m_TextureCoordinates.minimums;
	Vector2 spriteTextureMaximums = m_SpriteResource->m_TextureCoordinates.maximums;

	float cosAngle = CosineOfDegrees(m_Rotation);
	float sinAngle = SineOfDegrees(m_Rotation);

	Vector3 boundingPoint = Vector3::ZERO;

	boundingPoint.X = (m_Scale.X * spriteMinimums.X * cosAngle) - (m_Scale.Y * spriteMinimums.Y * sinAngle) + m_Position.X;
	boundingPoint.Y = (m_Scale.X * spriteMinimums.X * sinAngle) + (m_Scale.Y * spriteMinimums.Y * cosAngle) + m_Position.Y;
	spriteVertex.m_Position = boundingPoint;
	spriteVertex.m_TextureCoordinates = Vector2(spriteTextureMinimums.X, spriteTextureMaximums.Y);
	spriteVertices[0] = spriteVertex;

	boundingPoint.X = (m_Scale.X * spriteMaximums.X * cosAngle) - (m_Scale.Y * spriteMinimums.Y * sinAngle) + m_Position.X;
	boundingPoint.Y = (m_Scale.X * spriteMaximums.X * sinAngle) + (m_Scale.Y * spriteMinimums.Y * cosAngle) + m_Position.Y;
	spriteVertex.m_Position = boundingPoint;
	spriteVertex.m_TextureCoordinates = Vector2(spriteTextureMaximums.X, spriteTextureMaximums.Y);
	spriteVertices[1] = spriteVertex;

	boundingPoint.X = (m_Scale.X * spriteMaximums.X * cosAngle) - (m_Scale.Y * spriteMaximums.Y * sinAngle) + m_Position.X;
	boundingPoint.Y = (m_Scale.X * spriteMaximums.X * sinAngle) + (m_Scale.Y * spriteMaximums.Y * cosAngle) + m_Position.Y;
	spriteVertex.m_Position = boundingPoint;
	spriteVertex.m_TextureCoordinates = Vector2(spriteTextureMaximums.X, spriteTextureMinimums.Y);
	spriteVertices[2] = spriteVertex;

	boundingPoint.X = (m_Scale.X * spriteMinimums.X * cosAngle) - (m_Scale.Y * spriteMaximums.Y * sinAngle) + m_Position.X;
	boundingPoint.Y = (m_Scale.X * spriteMinimums.X * sinAngle) + (m_Scale.Y * spriteMaximums.Y * cosAngle) + m_Position.Y;
	spriteVertex.m_Position = boundingPoint;
	spriteVertex.m_TextureCoordinates = Vector2(spriteTextureMinimums.X, spriteTextureMinimums.Y);
	spriteVertices[3] = spriteVertex;

	m_Mesh->WriteToMesh(&spriteVertices[0], &spriteIndices[0], NUMBER_OF_VERTICES, NUMBER_OF_INDICES);
}