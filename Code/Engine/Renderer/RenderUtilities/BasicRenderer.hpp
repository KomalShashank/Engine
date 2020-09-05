#pragma once

#include "Engine/Renderer/RenderUtilities/MasterRenderer.hpp"



class BasicRenderer : public MasterRenderer
{
public:
	BasicRenderer();
	~BasicRenderer();

	void InitializeRenderer();
	void SetOrthographicProjection(const Vector2& bottomLeft, const Vector2& topRight);
	void SetPerspectiveProjection(float fovAngleInDegrees, float aspectRatio, float nearDistance, float farDistance);

	void TranslateViewMatrix(const Vector2& translatePosition);
	void TranslateViewMatrix(const Vector3& translatePosition);
	void RotateViewMatrix(float rotateAngleInDegrees, const Vector3& axisOfRotation);
	void RotateAroundOwnAxis(const Vector2& objectPosition, float rotateAngleInDegrees);
	void ScaleViewMatrix(float scalingFactor);

	void PushViewMatrix();
	void PopViewMatrix();

	void Draw2DVertexArrays(const Vertex2D* vertices, int arraySize, unsigned int primitiveType);
	void Draw3DVertexArrays(const Vertex3D* vertices, int arraySize, unsigned int primitiveType);

	void Draw2DTexturedVertexArrays(const Vertex2D* vertices, int arraySize, unsigned int primitiveType, const Texture* texture);
	void Draw3DTexturedVertexArrays(const Vertex3D* vertices, int arraySize, unsigned int primitiveType, const Texture* texture);

	void DrawSingle2DPointVA(const Vector2& pointPosition, float pointSize = 1.0f, const RGBA& pointColor = RGBA::WHITE);
	void DrawSingle3DPointVA(const Vector3& pointPosition, float pointSize = 1.0f, const RGBA& pointColor = RGBA::WHITE);

	void Draw2DPointsVA(const Vertex2D* pointVertices, int arraySize, float pointSize = 1.0f);
	void Draw3DPointsVA(const Vertex3D* pointVertices, int arraySize, float pointSize = 1.0f);

	void DrawSingle2DLineVA(const Vector2& startingPoint, const Vector2& endingPoint, float lineThickness = 1.0f, const RGBA& lineColor = RGBA::WHITE);
	void DrawSingle3DLineVA(const Vector3& startingPoint, const Vector3& endingPoint, float lineThickness = 1.0f, const RGBA& lineColor = RGBA::WHITE);

	void Draw2DLinesVA(const Vertex2D* lineVertices, int arraySize, float lineThickness = 1.0f);
	void Draw3DLinesVA(const Vertex3D* lineVertices, int arraySize, float lineThickness = 1.0f);

	void Draw2DOriginLines(const Vector2& originPoint, float lineLength, unsigned char lineAlpha = 255, float lineThickness = 1.0f);
	void Draw3DOriginLines(const Vector3& originPoint, float lineLength, unsigned char lineAlpha = 255, float lineThickness = 1.0f);

	void Draw2DGrid(const AABB2& boundingPoints, const RGBA& gridColor = RGBA::WHITE);
	void Draw3DGrid(const AABB3& boundingPoints, const RGBA& gridColor = RGBA::WHITE);

	void DrawSingle2DQuadVA(const AABB2& boundingPoints, bool wireframeMode = false, const RGBA& quadColor = RGBA::WHITE);
	void DrawSingle3DQuadVA(const Vector3& bottomLeftPoint, const Vector3& bottomRightPoint, const Vector3& topLeftPoint, const Vector3& topRightPoint, bool wireframeMode = false, const RGBA& quadColor = RGBA::WHITE);

	void Draw2DQuadsVA(const Vertex2D* quadVertices, int arraySize, bool wireframeMode = false);
	void Draw3DQuadsVA(const Vertex3D* quadVertices, int arraySize, bool wireframeMode = false);

	void DrawSingle2DTexturedQuadVA(const AABB2& boundingPoints, const AABB2& textureBounds, const Texture* texture, const RGBA& textureTint = RGBA::WHITE);
	void DrawSingle3DTexturedQuadVA(const Vector3& bottomLeftPoint, const Vector3& bottomRightPoint, const Vector3& topLeftPoint, const Vector3& topRightPoint, const AABB2& textureBounds, const Texture* texture, const RGBA& textureTint = RGBA::WHITE);

	void Draw2DTexturedQuadsVA(const Vertex2D* quadVertices, int arraySize, const Texture* texture);
	void Draw3DTexturedQuadsVA(const Vertex3D* quadVertices, int arraySize, const Texture* texture);

	void Draw3DSphereVA(const Vector3& sphereCentrePosition, float sphereRadius, bool wireframeMode = false, const RGBA& sphereColor = RGBA::WHITE);
	void Draw3DTexturedSphereVA(const Vector3& sphereCentrePosition, float sphereRadius, const Texture* texture, const RGBA& textureTint = RGBA::WHITE);

	void DrawVertexElementQuads(const std::vector<Vertex3D>& quadVertices, const std::vector<unsigned short>& quadIndices, bool wireframeMode = false);
	void DrawTexturedVertexElementQuads(const std::vector<Vertex3D>& quadVertices, const std::vector<unsigned short>& quadIndices, const Texture* texture);

	void Draw2DMonospacedText(const Vector2& startingLetterMinimums, const std::string& asciiText, float cellHeight, float cellAspectRatio, const MonospaceFont* font = nullptr, const RGBA& textTint = RGBA::WHITE);
	void Draw2DProportionalText(const Vector2& startingLetterMinimums, const std::string& asciiText, float cellHeight, const ProportionalFont* font = nullptr, const RGBA& textTint = RGBA::WHITE);
};

extern BasicRenderer* g_BasicRenderer;