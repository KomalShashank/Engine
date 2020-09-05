#include "Engine/Renderer/MeshAndMaterial/MeshBuilder.hpp"
#include "Engine/Renderer/OpenGL/OpenGLExtensions.hpp"
#include "Engine/ErrorHandling/ErrorWarningAssert.hpp"



MeshBuilder::MeshBuilder() :
m_DataMask(0)
{

}



void MeshBuilder::BeginMeshBuilding()
{
	m_StartIndex = m_AllVertices.size();
}



void MeshBuilder::EndMeshBuilding(const std::string& materialID /*= ""*/)
{
	if (m_StartIndex < m_AllVertices.size())
	{
		MeshRenderInstruction currentInstruction;
		currentInstruction.m_PrimitiveType = GL_TRIANGLES;
		currentInstruction.m_StartIndex = m_StartIndex;
		currentInstruction.m_ElementCount = m_AllVertices.size() - m_StartIndex;
		currentInstruction.m_RenderUsingIndexBuffer = false;
		currentInstruction.m_MaterialID = materialID;

		m_RenderInstructions.push_back(currentInstruction);

		m_StartIndex = m_AllVertices.size();
	}
}



void MeshBuilder::ResetVertex()
{
	m_CurrentVertex = Vertex3D();
}



void MeshBuilder::SetVertexPosition(const Vector3& vertexPosition)
{
	m_CurrentVertex.m_Position = vertexPosition;
	SetDataFlagToDataMask(POSITION_DATA);
}



void MeshBuilder::SetVertexColor(const RGBA& vertexColor)
{
	m_CurrentVertex.m_Color = vertexColor;
	SetDataFlagToDataMask(COLOR_DATA);
}



void MeshBuilder::SetVertexTextureCoordinates(const Vector2& vertexTextureCoordinates)
{
	m_CurrentVertex.m_TextureCoordinates = vertexTextureCoordinates;
	SetDataFlagToDataMask(TEXTURE_COORDINATES_DATA);
}



void MeshBuilder::SetVertexTangent(const Vector3& vertexTangent)
{
	m_CurrentVertex.m_Tangent = vertexTangent;
	SetDataFlagToDataMask(TANGENT_DATA);
}



void MeshBuilder::SetVertexBitangent(const Vector3& vertexBitangent)
{
	m_CurrentVertex.m_Bitangent = vertexBitangent;
	SetDataFlagToDataMask(BITANGENT_DATA);
}



void MeshBuilder::SetVertexNormal(const Vector3& vertexNormal)
{
	m_CurrentVertex.m_Normal = vertexNormal;
	SetDataFlagToDataMask(NORMAL_DATA);
}



void MeshBuilder::SetBoneWeights(const Vector4& boneWeights, const IntVector4& boneIndices)
{
	m_CurrentVertex.m_BoneWeights = boneWeights;
	RenormalizeBoneWeights(m_CurrentVertex.m_BoneWeights);
	m_CurrentVertex.m_BoneIndices = boneIndices;

	SetDataFlagToDataMask(BONE_WEIGHT_DATA);
	SetDataFlagToDataMask(BONE_INDEX_DATA);
}



void MeshBuilder::RenormalizeBoneWeights(Vector4& boneWeights)
{
	float totalWeight = boneWeights.X + boneWeights.Y + boneWeights.Z + boneWeights.T;
	if (totalWeight > 0.0f)
	{
		boneWeights /= totalWeight;
	}
	else
	{
		boneWeights = Vector4(1.0f, 0.0f, 0.0f, 0.0f);
	}
}



void MeshBuilder::AddVertexToAllVertices()
{
	m_AllVertices.push_back(m_CurrentVertex);
}



size_t MeshBuilder::GetNumberOfVertices() const
{
	return m_AllVertices.size();
}



size_t MeshBuilder::GetNumberOfIndices() const
{
	return m_AllIndices.size();
}



size_t MeshBuilder::GetNumberOfRenderInstructions() const
{
	return m_RenderInstructions.size();
}



void MeshBuilder::SetDataFlagToDataMask(const MeshDataFlag& dataFlag)
{
	m_DataMask |= (1 << dataFlag);
}



uint32_t MeshBuilder::GetDataMask() const
{
	return m_DataMask;
}



void MeshBuilder::CopyToMesh(Mesh* copyableMesh)
{
	size_t vertexCount = m_AllVertices.size();
	if (vertexCount == 0)
	{
		return;
	}

	copyableMesh->WriteToMesh(m_AllVertices.data(), m_AllIndices.data(), m_AllVertices.size(), m_AllIndices.size());

	copyableMesh->m_RenderInstructions.clear();
	for (MeshRenderInstruction currentInstruction : m_RenderInstructions)
	{
		copyableMesh->m_RenderInstructions.push_back(currentInstruction);
	}
}



MeshBuilder* MeshBuilder::CombineMeshes(const std::vector<MeshBuilder*>& meshesList)
{
	MeshBuilder* singleMesh = new MeshBuilder();

	for (MeshBuilder* currentMesh : meshesList)
	{
		uint32_t previousNumberOfVertices = singleMesh->m_AllVertices.size();
		uint32_t previousNumberOfIndices = singleMesh->m_AllIndices.size();

		for (const Vertex3D& currentVertex : currentMesh->m_AllVertices)
		{
			singleMesh->m_AllVertices.push_back(currentVertex);
		}

		for (uint32_t currentIndex : currentMesh->m_AllIndices)
		{
			singleMesh->m_AllIndices.push_back(previousNumberOfVertices + currentIndex);
		}

		MeshRenderInstruction compositeInstruction;
		for (const MeshRenderInstruction& currentInstruction : currentMesh->m_RenderInstructions)
		{
			uint32_t indexOffset = (currentInstruction.m_RenderUsingIndexBuffer) ? previousNumberOfIndices : previousNumberOfVertices;
			
			compositeInstruction.m_PrimitiveType = currentInstruction.m_PrimitiveType;
			compositeInstruction.m_ElementCount = currentInstruction.m_ElementCount;
			compositeInstruction.m_StartIndex = currentInstruction.m_StartIndex + indexOffset;
			compositeInstruction.m_RenderUsingIndexBuffer = currentInstruction.m_RenderUsingIndexBuffer;
			compositeInstruction.m_MaterialID = compositeInstruction.m_MaterialID;

			singleMesh->m_RenderInstructions.push_back(compositeInstruction);
		}

		singleMesh->m_DataMask |= currentMesh->m_DataMask;
	}

	return singleMesh;
}



void MeshBuilder::WriteMeshToFile(const char* fileName, const MeshBuilder& meshBuilder)
{
	BinaryFileWriter fileWriter;
	
	fileWriter.OpenBinaryFile(fileName);
	WriteToStream(fileWriter, meshBuilder);
	fileWriter.CloseBinaryFile();
}



void MeshBuilder::ReadMeshFromFile(const char* fileName, MeshBuilder& meshBuilder)
{
	BinaryFileReader fileReader;

	fileReader.OpenBinaryFile(fileName);
	ReadFromStream(fileReader, meshBuilder);
	fileReader.CloseBinaryFile();
}



void MeshBuilder::WriteToStream(const BinaryWriteInterface& streamWriter, const MeshBuilder& meshBuilder)
{
	streamWriter.Write<uint32_t>(s_FileVersion);

	uint32_t numberOfVertices = meshBuilder.GetNumberOfVertices();
	uint32_t numberOfIndices = meshBuilder.GetNumberOfIndices();

	streamWriter.Write<uint32_t>(numberOfVertices);
	streamWriter.Write<uint32_t>(numberOfIndices);

	WriteDataMask(streamWriter, meshBuilder.m_DataMask);

	WriteVerticesToStream(streamWriter, meshBuilder.m_AllVertices, meshBuilder.m_DataMask);
	WriteIndicesToStream(streamWriter, meshBuilder.m_AllIndices);

	uint32_t numberOfRenderInstructions = meshBuilder.GetNumberOfRenderInstructions();
	streamWriter.Write<uint32_t>(numberOfRenderInstructions);

	WriteRenderInstructionsToStream(streamWriter, meshBuilder.m_RenderInstructions);
}



void MeshBuilder::ReadFromStream(const BinaryReadInterface& streamReader, MeshBuilder& meshBuilder)
{
	uint32_t fileVersion;
	streamReader.Read<uint32_t>(fileVersion);
	if (fileVersion != s_FileVersion)
	{
		ERROR_RECOVERABLE("File version conflict. Different version used.");
		return;
	}

	uint32_t numberOfVertices;
	uint32_t numberOfIndices;

	streamReader.Read<uint32_t>(numberOfVertices);
	streamReader.Read<uint32_t>(numberOfIndices);

	meshBuilder.m_AllVertices.resize(numberOfVertices);
	meshBuilder.m_AllIndices.resize(numberOfIndices);

	ReadDataMask(streamReader, meshBuilder.m_DataMask);

	ReadVerticesFromStream(streamReader, meshBuilder.m_AllVertices, meshBuilder.m_DataMask);
	ReadIndicesFromStream(streamReader, meshBuilder.m_AllIndices);

	uint32_t numberOfRenderInstructions;
	streamReader.Read<uint32_t>(numberOfRenderInstructions);
	meshBuilder.m_RenderInstructions.resize(numberOfRenderInstructions);

	ReadRenderInstructionsFromStream(streamReader, meshBuilder.m_RenderInstructions);
}



void MeshBuilder::WriteDataMask(const BinaryWriteInterface& streamWriter, const uint32_t& dataMask)
{
	if ((dataMask & (1 << POSITION_DATA)) != 0)
	{
		streamWriter.WriteBinaryString("Position");
	}
	if ((dataMask & (1 << COLOR_DATA)) != 0)
	{
		streamWriter.WriteBinaryString("Colour");
	}
	if ((dataMask & (1 << TEXTURE_COORDINATES_DATA)) != 0)
	{
		streamWriter.WriteBinaryString("TextureCoordinates");
	}
	if ((dataMask & (1 << TANGENT_DATA)) != 0)
	{
		streamWriter.WriteBinaryString("Tangent");
	}
	if ((dataMask & (1 << BITANGENT_DATA)) != 0)
	{
		streamWriter.WriteBinaryString("Bitangent");
	}
	if ((dataMask & (1 << NORMAL_DATA)) != 0)
	{
		streamWriter.WriteBinaryString("Normal");
	}
	if ((dataMask & (1 << BONE_WEIGHT_DATA)) != 0)
	{
		streamWriter.WriteBinaryString("BoneWeights");
	}
	if ((dataMask & (1 << BONE_INDEX_DATA)) != 0)
	{
		streamWriter.WriteBinaryString("BoneIndices");
	}

	streamWriter.WriteBinaryString("");
}



void MeshBuilder::ReadDataMask(const BinaryReadInterface& streamReader, uint32_t& dataMask)
{
	dataMask = 0;

	std::string stringData;
	while (streamReader.ReadBinaryString(stringData))
	{
		if (stringData == "Position")
		{
			dataMask |= (1 << POSITION_DATA);
		}
		else if (stringData == "Colour")
		{
			dataMask |= (1 << COLOR_DATA);
		}
		else if (stringData == "TextureCoordinates")
		{
			dataMask |= (1 << TEXTURE_COORDINATES_DATA);
		}
		else if (stringData == "Tangent")
		{
			dataMask |= (1 << TANGENT_DATA);
		}
		else if (stringData == "Bitangent")
		{
			dataMask |= (1 << BITANGENT_DATA);
		}
		else if (stringData == "Normal")
		{
			dataMask |= (1 << NORMAL_DATA);
		}
		else if (stringData == "BoneWeights")
		{
			dataMask |= (1 << BONE_WEIGHT_DATA);
		}
		else if (stringData == "BoneIndices")
		{
			dataMask |= (1 << BONE_INDEX_DATA);
		}
		else if (stringData == "")
		{
			break;
		}
	}
}



void MeshBuilder::WriteVerticesToStream(const BinaryWriteInterface& streamWriter, const std::vector<Vertex3D>& meshVertices, const uint32_t& dataMask)
{
	for (const Vertex3D& currentVertex : meshVertices)
	{
		if ((dataMask & (1 << POSITION_DATA)) != 0)
		{
			streamWriter.Write<float>(currentVertex.m_Position.X);
			streamWriter.Write<float>(currentVertex.m_Position.Y);
			streamWriter.Write<float>(currentVertex.m_Position.Z);
		}
		if ((dataMask & (1 << COLOR_DATA)) != 0)
		{
			streamWriter.Write<unsigned char>(currentVertex.m_Color.m_Red);
			streamWriter.Write<unsigned char>(currentVertex.m_Color.m_Green);
			streamWriter.Write<unsigned char>(currentVertex.m_Color.m_Blue);
			streamWriter.Write<unsigned char>(currentVertex.m_Color.m_Alpha);
		}
		if ((dataMask & (1 << TEXTURE_COORDINATES_DATA)) != 0)
		{
			streamWriter.Write<float>(currentVertex.m_TextureCoordinates.X);
			streamWriter.Write<float>(currentVertex.m_TextureCoordinates.Y);
		}
		if ((dataMask & (1 << TANGENT_DATA)) != 0)
		{
			streamWriter.Write<float>(currentVertex.m_Tangent.X);
			streamWriter.Write<float>(currentVertex.m_Tangent.Y);
			streamWriter.Write<float>(currentVertex.m_Tangent.Z);
		}
		if ((dataMask & (1 << BITANGENT_DATA)) != 0)
		{
			streamWriter.Write<float>(currentVertex.m_Bitangent.X);
			streamWriter.Write<float>(currentVertex.m_Bitangent.Y);
			streamWriter.Write<float>(currentVertex.m_Bitangent.Z);
		}
		if ((dataMask & (1 << NORMAL_DATA)) != 0)
		{
			streamWriter.Write<float>(currentVertex.m_Normal.X);
			streamWriter.Write<float>(currentVertex.m_Normal.Y);
			streamWriter.Write<float>(currentVertex.m_Normal.Z);
		}
		if ((dataMask & (1 << BONE_WEIGHT_DATA)) != 0)
		{
			streamWriter.Write<float>(currentVertex.m_BoneWeights.X);
			streamWriter.Write<float>(currentVertex.m_BoneWeights.Y);
			streamWriter.Write<float>(currentVertex.m_BoneWeights.Z);
			streamWriter.Write<float>(currentVertex.m_BoneWeights.T);
		}
		if ((dataMask & (1 << BONE_INDEX_DATA)) != 0)
		{
			streamWriter.Write<int32_t>(currentVertex.m_BoneIndices.X);
			streamWriter.Write<int32_t>(currentVertex.m_BoneIndices.Y);
			streamWriter.Write<int32_t>(currentVertex.m_BoneIndices.Z);
			streamWriter.Write<int32_t>(currentVertex.m_BoneIndices.T);
		}
	}
}



void MeshBuilder::ReadVerticesFromStream(const BinaryReadInterface& streamReader, std::vector<Vertex3D>& meshVertices, const uint32_t& dataMask)
{
	for (Vertex3D& currentVertex : meshVertices)
	{
		if ((dataMask & (1 << POSITION_DATA)) != 0)
		{
			streamReader.Read<float>(currentVertex.m_Position.X);
			streamReader.Read<float>(currentVertex.m_Position.Y);
			streamReader.Read<float>(currentVertex.m_Position.Z);
		}
		if ((dataMask & (1 << COLOR_DATA)) != 0)
		{
			streamReader.Read<unsigned char>(currentVertex.m_Color.m_Red);
			streamReader.Read<unsigned char>(currentVertex.m_Color.m_Green);
			streamReader.Read<unsigned char>(currentVertex.m_Color.m_Blue);
			streamReader.Read<unsigned char>(currentVertex.m_Color.m_Alpha);
		}
		if ((dataMask & (1 << TEXTURE_COORDINATES_DATA)) != 0)
		{
			streamReader.Read<float>(currentVertex.m_TextureCoordinates.X);
			streamReader.Read<float>(currentVertex.m_TextureCoordinates.Y);
		}
		if ((dataMask & (1 << TANGENT_DATA)) != 0)
		{
			streamReader.Read<float>(currentVertex.m_Tangent.X);
			streamReader.Read<float>(currentVertex.m_Tangent.Y);
			streamReader.Read<float>(currentVertex.m_Tangent.Z);
		}
		if ((dataMask & (1 << BITANGENT_DATA)) != 0)
		{
			streamReader.Read<float>(currentVertex.m_Bitangent.X);
			streamReader.Read<float>(currentVertex.m_Bitangent.Y);
			streamReader.Read<float>(currentVertex.m_Bitangent.Z);
		}
		if ((dataMask & (1 << NORMAL_DATA)) != 0)
		{
			streamReader.Read<float>(currentVertex.m_Normal.X);
			streamReader.Read<float>(currentVertex.m_Normal.Y);
			streamReader.Read<float>(currentVertex.m_Normal.Z);
		}
		if ((dataMask & (1 << BONE_WEIGHT_DATA)) != 0)
		{
			streamReader.Read<float>(currentVertex.m_BoneWeights.X);
			streamReader.Read<float>(currentVertex.m_BoneWeights.Y);
			streamReader.Read<float>(currentVertex.m_BoneWeights.Z);
			streamReader.Read<float>(currentVertex.m_BoneWeights.T);
		}
		if ((dataMask & (1 << BONE_INDEX_DATA)) != 0)
		{
			streamReader.Read<int32_t>(currentVertex.m_BoneIndices.X);
			streamReader.Read<int32_t>(currentVertex.m_BoneIndices.Y);
			streamReader.Read<int32_t>(currentVertex.m_BoneIndices.Z);
			streamReader.Read<int32_t>(currentVertex.m_BoneIndices.T);
		}
	}
}



void MeshBuilder::WriteIndicesToStream(const BinaryWriteInterface& streamWriter, const std::vector<uint32_t>& meshIndices)
{
	for (const uint32_t& currentIndex : meshIndices)
	{
		streamWriter.Write<uint32_t>(currentIndex);
	}
}



void MeshBuilder::ReadIndicesFromStream(const BinaryReadInterface& streamReader, std::vector<uint32_t>& meshIndices)
{
	for (uint32_t& currentIndex : meshIndices)
	{
		streamReader.Read<uint32_t>(currentIndex);
	}
}



void MeshBuilder::WriteRenderInstructionsToStream(const BinaryWriteInterface& streamWriter, const std::vector<MeshRenderInstruction>& renderInstructions)
{
	for (const MeshRenderInstruction& currentInstruction : renderInstructions)
	{
		streamWriter.Write<uint32_t>(currentInstruction.m_PrimitiveType);
		streamWriter.Write<uint32_t>(currentInstruction.m_StartIndex);
		streamWriter.Write<uint32_t>(currentInstruction.m_ElementCount);

		uint8_t renderUsingIndexBuffer = static_cast<uint8_t>(currentInstruction.m_RenderUsingIndexBuffer);
		streamWriter.Write<uint8_t>(renderUsingIndexBuffer);
		streamWriter.WriteBinaryString(currentInstruction.m_MaterialID);
	}
}



void MeshBuilder::ReadRenderInstructionsFromStream(const BinaryReadInterface& streamReader, std::vector<MeshRenderInstruction>& renderInstructions)
{
	for (MeshRenderInstruction& currentInstruction : renderInstructions)
	{
		streamReader.Read<uint32_t>(currentInstruction.m_PrimitiveType);
		streamReader.Read<uint32_t>(currentInstruction.m_StartIndex);
		streamReader.Read<uint32_t>(currentInstruction.m_ElementCount);

		uint8_t renderUsingIndexBuffer;
		streamReader.Read<uint8_t>(renderUsingIndexBuffer);
		currentInstruction.m_RenderUsingIndexBuffer = (renderUsingIndexBuffer > 0);
		streamReader.ReadBinaryString(currentInstruction.m_MaterialID);
	}
}