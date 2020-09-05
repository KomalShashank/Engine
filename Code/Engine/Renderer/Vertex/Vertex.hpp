#pragma once

#include "Engine/Math/VectorMath/2D/Vector2.hpp"
#include "Engine/Math/VectorMath/3D/Vector3.hpp"
#include "Engine/Math/VectorMath/4D/Vector4.hpp"
#include "Engine/Math/VectorMath/4D/IntVector4.hpp"
#include "Engine/Renderer/Color/RGBA.hpp"



class Vertex2D
{
public:
	Vertex2D(	const Vector2& position = Vector2::ZERO,
				const RGBA& color = RGBA::WHITE,
				const Vector2& textureCoordinates = Vector2::ZERO,
				const Vector2& tangent = Vector2::ZERO,
				const Vector2& bitangent = Vector2::ZERO,
				const Vector2& normal = Vector2::ZERO,
				const Vector4& boneWeights = Vector4::ZERO,
				const IntVector4& boneIndices = IntVector4::ZERO	);

public:
	Vector2 m_Position;
	RGBA m_Color;
	Vector2 m_TextureCoordinates;

	Vector2 m_Tangent;
	Vector2 m_Bitangent;
	Vector2 m_Normal;

	Vector4 m_BoneWeights;
	IntVector4 m_BoneIndices;
};



class Vertex3D
{
public:
	Vertex3D(	const Vector3& position = Vector3::ZERO,
				const RGBA& color = RGBA::WHITE,
				const Vector2& textureCoordinates = Vector2::ZERO,
				const Vector3& tangent = Vector3::ZERO,
				const Vector3& bitangent = Vector3::ZERO,
				const Vector3& normal = Vector3::ZERO,
				const Vector4& boneWeights = Vector4::ZERO,
				const IntVector4& boneIndices = IntVector4::ZERO	);

public:
	Vector3 m_Position;
	RGBA m_Color;
	Vector2 m_TextureCoordinates;

	Vector3 m_Tangent;
	Vector3 m_Bitangent;
	Vector3 m_Normal;

	Vector4 m_BoneWeights;
	IntVector4 m_BoneIndices;
};