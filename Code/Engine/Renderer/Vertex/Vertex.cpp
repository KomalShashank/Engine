#include "Engine/Renderer/Vertex/Vertex.hpp"



Vertex2D::Vertex2D(	const Vector2& position /*= Vector2::ZERO*/,
					const RGBA& color /*= RGBA::WHITE*/,
					const Vector2& textureCoordinates /*= Vector2::ZERO*/,
					const Vector2& tangent /*= Vector2::ZERO*/,
					const Vector2& bitangent /*= Vector2::ZERO*/,
					const Vector2& normal /*= Vector2::ZERO*/,
					const Vector4& boneWeights /*= Vector4::ZERO*/,
					const IntVector4& boneIndices /*= IntVector4::ZERO*/	) :
m_Position(position),
m_Color(color),
m_TextureCoordinates(textureCoordinates),
m_Tangent(tangent),
m_Bitangent(bitangent),
m_Normal(normal),
m_BoneWeights(boneWeights),
m_BoneIndices(boneIndices)
{

}



Vertex3D::Vertex3D(	const Vector3& position /*= Vector3::ZERO*/,
					const RGBA& color /*= RGBA::WHITE*/,
					const Vector2& textureCoordinates /*= Vector2::ZERO*/,
					const Vector3& tangent /*= Vector3::ZERO*/,
					const Vector3& bitangent /*= Vector3::ZERO*/,
					const Vector3& normal /*= Vector3::ZERO*/,
					const Vector4& boneWeights /*= Vector4::ZERO*/,
					const IntVector4& boneIndices /*= IntVector4::ZERO*/	) :
m_Position(position),
m_Color(color),
m_TextureCoordinates(textureCoordinates),
m_Tangent(tangent),
m_Bitangent(bitangent),
m_Normal(normal),
m_BoneWeights(boneWeights),
m_BoneIndices(boneIndices)
{

}