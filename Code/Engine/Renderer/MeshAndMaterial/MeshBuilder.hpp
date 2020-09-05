#pragma once

#include <vector>
#include <string>

#include "Engine/Math/VectorMath/2D/Vector2.hpp"
#include "Engine/Math/VectorMath/3D/Vector3.hpp"
#include "Engine/Math/VectorMath/4D/Vector4.hpp"
#include "Engine/Math/VectorMath/4D/IntVector4.hpp"
#include "Engine/Renderer/Color/RGBA.hpp"
#include "Engine/Renderer/Vertex/Vertex.hpp"
#include "Engine/Renderer/MeshAndMaterial/Mesh.hpp"
#include "Engine/IO Utilities/BinaryFileIO.hpp"



enum MeshDataFlag
{
	INVALID_MESH_DATA = -1,
	POSITION_DATA,
	COLOR_DATA,
	TEXTURE_COORDINATES_DATA,
	TANGENT_DATA,
	BITANGENT_DATA,
	NORMAL_DATA,
	BONE_WEIGHT_DATA,
	BONE_INDEX_DATA
};



class MeshBuilder
{
public:
	MeshBuilder();

	void BeginMeshBuilding();
	void EndMeshBuilding(const std::string& materialID = "");

	void ResetVertex();

	void SetVertexPosition(const Vector3& vertexPosition);
	void SetVertexColor(const RGBA& vertexColor);
	void SetVertexTextureCoordinates(const Vector2& vertexTextureCoordinates);
	void SetVertexTangent(const Vector3& vertexTangent);
	void SetVertexBitangent(const Vector3& vertexBitangent);
	void SetVertexNormal(const Vector3& vertexNormal);

	void SetBoneWeights(const Vector4& boneWeights, const IntVector4& boneIndices);
	void RenormalizeBoneWeights(Vector4& boneWeights);

	void AddVertexToAllVertices();

	size_t GetNumberOfVertices() const;
	size_t GetNumberOfIndices() const;
	size_t GetNumberOfRenderInstructions() const;

	void SetDataFlagToDataMask(const MeshDataFlag& dataFlag);
	uint32_t GetDataMask() const;

	void CopyToMesh(Mesh* copyableMesh);
	static MeshBuilder* CombineMeshes(const std::vector<MeshBuilder*>& meshesList);

	static void WriteMeshToFile(const char* fileName, const MeshBuilder& meshBuilder);
	static void ReadMeshFromFile(const char* fileName, MeshBuilder& meshBuilder);

private:
	static void WriteToStream(const BinaryWriteInterface& streamWriter, const MeshBuilder& meshBuilder);
	static void ReadFromStream(const BinaryReadInterface& streamReader, MeshBuilder& meshBuilder);

	static void WriteDataMask(const BinaryWriteInterface& streamWriter, const uint32_t& dataMask);
	static void ReadDataMask(const BinaryReadInterface& streamReader, uint32_t& dataMask);

	static void WriteVerticesToStream(const BinaryWriteInterface& streamWriter, const std::vector<Vertex3D>& meshVertices, const uint32_t& dataMask);
	static void ReadVerticesFromStream(const BinaryReadInterface& streamReader, std::vector<Vertex3D>& meshVertices, const uint32_t& dataMask);

	static void WriteIndicesToStream(const BinaryWriteInterface& streamWriter, const std::vector<uint32_t>& meshIndices);
	static void ReadIndicesFromStream(const BinaryReadInterface& streamReader, std::vector<uint32_t>& meshIndices);

	static void WriteRenderInstructionsToStream(const BinaryWriteInterface& streamWriter, const std::vector<MeshRenderInstruction>& renderInstructions);
	static void ReadRenderInstructionsFromStream(const BinaryReadInterface& streamReader, std::vector<MeshRenderInstruction>& renderInstructions);

private:
	static const uint32_t s_FileVersion = 1;

	Vertex3D m_CurrentVertex;
	std::vector<Vertex3D> m_AllVertices;
	std::vector<uint32_t> m_AllIndices;
	size_t m_StartIndex;

	uint32_t m_DataMask;

	std::vector<MeshRenderInstruction> m_RenderInstructions;
};