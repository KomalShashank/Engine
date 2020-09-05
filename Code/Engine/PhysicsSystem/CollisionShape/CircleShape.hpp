#pragma once

#include "Engine/PhysicsSystem/General/PhysicsCommons.hpp"
#include "Engine/PhysicsSystem/CollisionShape/CollisionShape.hpp"



class CircleShape final : public CollisionShape
{
public:
	CircleShape();
	~CircleShape();

	CollisionShape* CloneShape(BlockMemoryAllocator* blockAllocator) const override;

	uint8_t GetShapeType() const override;
	bool IsPointInside(const Transform2D& shapeTransform, const Vector2D& testPoint) const override;
	bool Raycast(const Transform2D& shapeTransform, const RaycastInput& raycastInput, RaycastResult& raycastResult) const override;

	AABB2D GetAABB(const Transform2D& shapeTransform) const override;
	void CalculateMassInfo(BodyMassInfo* bodyMassInfo, float shapeDensity) const override;

	void RenderShape(const Transform2D& shapeTransform, const RGBA* shapeColor) const override;

	void SetLocalCenter(const Vector2D& localCenter);
	Vector2D GetLocalCenter() const;

	const Vector2D* GetCircleLocalPoint() const;

	void SetCircleRadius(float circleRadius);
	float GetCircleRadius() const;

private:
	void RenderHollowCircle(const Transform2D& shapeTransform, const RGBA* shapeColor) const;
	void RenderRadiusLine(const Transform2D& shapeTransform, const RGBA* shapeColor) const;

private:
	Vector2D m_LocalCenter;
	float m_CircleRadius;
};