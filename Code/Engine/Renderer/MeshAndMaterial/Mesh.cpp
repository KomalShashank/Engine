#include "Engine/Renderer/MeshAndMaterial/Mesh.hpp"
#include "Engine/Renderer/OpenGL/OpenGLExtensions.hpp"



Mesh::Mesh() :
m_VertexBufferObject(new RenderBuffer()),
m_IndexBufferObject(new RenderBuffer())
{

}



Mesh::Mesh(const Vertex3D* meshVertices, const uint32_t* meshIndices, size_t numberOfVertices, size_t numberOfIndices)
{
	InitializeMesh(meshVertices, meshIndices, numberOfVertices, numberOfIndices);
}



Mesh::~Mesh()
{
	delete m_VertexBufferObject;
	delete m_IndexBufferObject;
}



Mesh* Mesh::CreateMesh(MeshLoader* LoadMesh)
{
	std::vector<Vertex3D> meshVertices;
	std::vector<uint32_t> meshIndices;
	LoadMesh(meshVertices, meshIndices);

	Mesh* newlyCreatedMesh = new Mesh(meshVertices.data(), meshIndices.data(), meshVertices.size(), meshIndices.size());

	return newlyCreatedMesh;
}



void Mesh::InitializeMesh(const Vertex3D* meshVertices, const uint32_t* meshIndices, size_t numberOfVertices, size_t numberOfIndices)
{
	m_VertexBufferObject = new RenderBuffer(GL_ARRAY_BUFFER, meshVertices, numberOfVertices, sizeof(Vertex3D), GL_STATIC_DRAW);
	m_IndexBufferObject = new RenderBuffer(GL_ELEMENT_ARRAY_BUFFER, meshIndices, numberOfIndices, sizeof(uint32_t), GL_STATIC_DRAW);

	AddRenderInstruction(numberOfVertices, numberOfIndices);
}



void Mesh::WriteToMesh(const Vertex3D* meshVertices, const uint32_t* meshIndices, size_t numberOfVertices, size_t numberOfIndices)
{
	if (m_VertexBufferObject != nullptr)
	{
		m_VertexBufferObject->WriteToRenderBuffer(GL_ARRAY_BUFFER, meshVertices, numberOfVertices, sizeof(Vertex3D), GL_STATIC_DRAW);
	}

	if (m_IndexBufferObject != nullptr)
	{
		m_IndexBufferObject->WriteToRenderBuffer(GL_ELEMENT_ARRAY_BUFFER, meshIndices, numberOfIndices, sizeof(uint32_t), GL_STATIC_DRAW);
	}
}



void Mesh::AddRenderInstruction(size_t numberOfVertices, size_t numberOfIndices, unsigned int primitiveType /*= TRIANGLES_PRIMITIVE*/, const std::string& materialID /*= ""*/)
{
	MeshRenderInstruction renderInstruction;

	renderInstruction.m_PrimitiveType = primitiveType;
	renderInstruction.m_ElementCount = (numberOfIndices > 0) ? numberOfIndices : numberOfVertices;
	renderInstruction.m_StartIndex = 0;
	renderInstruction.m_RenderUsingIndexBuffer = (numberOfIndices > 0);
	renderInstruction.m_MaterialID = materialID;

	m_RenderInstructions.push_back(renderInstruction);
}



void Mesh::RenderUsingInstructions() const
{
	for (const MeshRenderInstruction& currentInstruction : m_RenderInstructions)
	{
		if (currentInstruction.m_RenderUsingIndexBuffer)
		{
			glDrawElements(currentInstruction.m_PrimitiveType, currentInstruction.m_ElementCount, GL_UNSIGNED_INT, (void*)(currentInstruction.m_StartIndex * sizeof(uint32_t)));
		}
		else
		{
			glDrawArrays(currentInstruction.m_PrimitiveType, currentInstruction.m_StartIndex, currentInstruction.m_ElementCount);
		}
	}
}