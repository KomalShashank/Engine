#pragma once

#include "Engine/Renderer/RenderUtilities/MasterRenderer.hpp"
#include "Engine/Renderer/MeshAndMaterial/MeshRenderer.hpp"
#include "Engine/Renderer/FrameBuffers/FrameBuffer.hpp"
#include "Engine/Math/VectorMath/2D/IntVector2.hpp"
#include "Engine/Math/MatrixMath/Matrix4.hpp"



class AdvancedRenderer : public MasterRenderer
{
private:
	AdvancedRenderer();
	AdvancedRenderer(const AdvancedRenderer&) = delete;
	~AdvancedRenderer();

	void InitializeOpenGL();

public:
	static void InitializeRenderer(const IntVector2& windowDimensions);
	static void UninitializeRenderer();

	static AdvancedRenderer* SingletonInstance();

	void UpdateModelMatrix(const Matrix4& modelMatrix);
	void UpdateViewMatrix(const Matrix4& viewMatrix);
	void UpdateProjectionMatrix(const Matrix4& projectionMatrix);

	Matrix4 GetModelMatrix(const Vector3& scalingFactor, const EulerAngles& rotationAngles, const Vector3& translationDisplacement);
	Matrix4 GetViewMatrix(const Vector3& viewTranslation, const EulerAngles& viewRotation);
	
	Matrix4 GetOrthographicProjectionMatrix(const Vector2& bottomLeft, const Vector2& topRight, float nearDistance = -1.0f, float farDistance = 1.0f);
	Matrix4 GetPerspectiveProjectionMatrix(float fovAngleInDegrees, float aspectRatio, float nearDistance, float farDistance);

	Matrix4 GetScalingMatrix(const Vector3& scalingFactor);
	Matrix4 GetRotationMatrix(const EulerAngles& rotationAngles);
	Matrix4 GetTranslationMatrix(const Vector3& translationDisplacement);

	static Matrix4 GetLocalBasis();

	void DrawMeshWithVAO(Mesh* drawableMesh, Material* meshMaterial = nullptr);

	void DrawPointsMesh(Mesh* pointsMesh, size_t numberOfVertices, size_t numberOfIndices, float pointSize = 1.0f);
	void DrawLinesMesh(Mesh* linesMesh, size_t numberOfVertices, size_t numberOfIndices, float lineThickness = 1.0f);
	void DrawLineLoopMesh(Mesh* lineLoopMesh, size_t numberOfVertices, size_t numberOfIndices, float lineThickness = 1.0f);
	void DrawPolygonMesh(Mesh* polygonMesh, size_t numberOfVertices, size_t numberOfIndices, Material* meshMaterial = nullptr);

	void TriangulatePolygon(size_t numberOfVertices, std::vector<uint32_t>& polygonIndices);

	void Draw2DMonospacedText(const Vector2& startingLetterMinimums, const char* asciiText, float cellHeight, float cellAspectRatio, const MonospaceFont* font = nullptr, const RGBA& textTint = RGBA::WHITE);
	void Draw2DProportionalText(const Vector2& startingLetterMinimums, const char* asciiText, float cellHeight, const ProportionalFont* font = nullptr, const RGBA& textTint = RGBA::WHITE);

	void BindFrameBuffer(FrameBuffer* frameBuffer, size_t colorTargetIndex = 0U);
	void UnbindFrameBuffer();
	void CopyFrameBufferToBackBuffer(FrameBuffer* frameBuffer);

private:
	Mesh* m_DefaultMesh;
	Material* m_DefaultMaterial;
	Texture* m_DefaultTexture;
	MeshRenderer* m_MeshRenderer;

	Matrix4 m_ModelMatrix;
	Matrix4 m_ViewMatrix;
	Matrix4 m_ProjectionMatrix;

	IntVector2 m_WindowDimensions;
};