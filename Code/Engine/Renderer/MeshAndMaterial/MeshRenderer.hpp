#pragma once

#include <string>
#include <vector>

#include "Engine/Renderer/MeshAndMaterial/Mesh.hpp"
#include "Engine/Renderer/MeshAndMaterial/Material.hpp"
#include "Engine/Math/VectorMath/3D/Vector3.hpp"
#include "Engine/Math/EulerAngles/EulerAngles.hpp"
#include "Engine/Math/MatrixMath/Matrix4.hpp"



class MeshRenderer
{
public:
	MeshRenderer();
	MeshRenderer(Mesh* mesh, Material* material);

	void SetMesh(Mesh* mesh);
	void SetMaterial(Material* material);

	void RenderMeshWithMaterial(const Matrix4& modelMatrix, const Matrix4& viewMatrix, const Matrix4& projectionMatrix) const;

private:
	void InitializeMeshRenderer();
	void BindToVertexArrayObject();

	void SetModelViewProjectionMatrices(const Matrix4& modelMatrix, const Matrix4& viewMatrix, const Matrix4& projectionMatrix) const;

	void GenerateMaterial() const;
	void RenderMesh() const;

public:
	Mesh* m_Mesh;
	Material* m_Material;

private:
	unsigned int m_VertexArrayObject;
};