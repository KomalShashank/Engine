#pragma once

#include "Engine/PhysicsSystem/General/PhysicsCommons.hpp"
#include "Engine/PhysicsSystem/CollisionShape/CollisionShape.hpp"



class LineShape final : public CollisionShape
{
public:
	LineShape();
	~LineShape();

	CollisionShape* CloneShape(BlockMemoryAllocator* blockAllocator) const override;

	uint8_t GetShapeType() const override;
	bool IsPointInside(const Transform2D& shapeTransform, const Vector2D& testPoint) const override;
	bool Raycast(const Transform2D& shapeTransform, const RaycastInput& raycastInput, RaycastResult& raycastResult) const override;

	AABB2D GetAABB(const Transform2D& shapeTransform) const override;
	void CalculateMassInfo(BodyMassInfo* bodyMassInfo, float shapeDensity) const override;

	void RenderShape(const Transform2D& shapeTransform, const RGBA* shapeColor) const override;

	void SetLineLocalPoints(const Vector2D& firstEndPoint, const Vector2D& secondEndPoint);
	const Vector2D* GetLineLocalPoints() const;

private:
	Vector2D m_FirstEndPoint;
	Vector2D m_SecondEndPoint;
};