#pragma once

#include <vector>

#include "Engine/Renderer/RenderUtilities/RenderConstants.hpp"
#include "Engine/Renderer/RenderUtilities/RenderBuffer.hpp"
#include "Engine/Renderer/Vertex/Vertex.hpp"



struct MeshRenderInstruction
{
	unsigned int m_PrimitiveType;
	size_t m_StartIndex;
	size_t m_ElementCount;
	bool m_RenderUsingIndexBuffer;
	std::string m_MaterialID;
};



typedef void (MeshLoader)(std::vector<Vertex3D>& meshVertices, std::vector<uint32_t>& meshIndices);



class Mesh
{
public:
	Mesh();
	Mesh(const Mesh&) = delete;
	Mesh(const Vertex3D* meshVertices, const uint32_t* meshIndices, size_t numberOfVertices, size_t numberOfIndices);
	~Mesh();

	static Mesh* CreateMesh(MeshLoader* LoadMesh);

	void InitializeMesh(const Vertex3D* meshVertices, const uint32_t* meshIndices, size_t numberOfVertices, size_t numberOfIndices);
	void WriteToMesh(const Vertex3D* meshVertices, const uint32_t* meshIndices, size_t numberOfVertices, size_t numberOfIndices);

	void AddRenderInstruction(size_t numberOfVertices, size_t numberOfIndices, unsigned int primitiveType = TRIANGLES_PRIMITIVE, const std::string& materialID = "");

	void RenderUsingInstructions() const;

public:
	RenderBuffer* m_VertexBufferObject;
	RenderBuffer* m_IndexBufferObject;

	std::vector<MeshRenderInstruction> m_RenderInstructions;
};