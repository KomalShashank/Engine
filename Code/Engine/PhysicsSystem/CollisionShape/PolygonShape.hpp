#pragma once

#include "Engine/PhysicsSystem/General/PhysicsCommons.hpp"
#include "Engine/PhysicsSystem/CollisionShape/CollisionShape.hpp"



class PolygonShape final : public CollisionShape
{
public:
	PolygonShape();
	~PolygonShape();

	CollisionShape* CloneShape(BlockMemoryAllocator* blockAllocator) const override;

	uint8_t GetShapeType() const override;
	bool IsPointInside(const Transform2D& shapeTransform, const Vector2D& testPoint) const override;
	bool Raycast(const Transform2D& shapeTransform, const RaycastInput& raycastInput, RaycastResult& raycastResult) const override;

	AABB2D GetAABB(const Transform2D& shapeTransform) const override;
	void CalculateMassInfo(BodyMassInfo* bodyMassInfo, float shapeDensity) const override;

	void RenderShape(const Transform2D& shapeTransform, const RGBA* shapeColor) const override;

	void SetVerticesAndCreateNormals(const Vector2D* polygonVertices, size_t numberOfVertices);
	void CreateAsSimpleQuad(const Vector2D& quadHalfDimensions);
	void CreateAsSimpleQuad(const Vector2D& quadHalfDimensions, const Transform2D& quadTransform);

	const Vector2D* GetVertices() const;
	Vector2D GetVertex(size_t vertexIndex) const;

	const Vector2D* GetEdgeNormals() const;
	Vector2D GetEdgeNormal(size_t normalIndex) const;

	Vector2D GetPolygonCentroid() const;
	size_t GetNumberOfVertices() const;

	bool IsConvex() const;

private:
	void ComputePolygonCentroid();

private:
	Vector2D m_PolygonVertices[MAXIMUM_POLYGON_VERTICES];
	Vector2D m_EdgeNormals[MAXIMUM_POLYGON_VERTICES];
	Vector2D m_PolygonCentroid;

	size_t m_NumberOfVertices;
};