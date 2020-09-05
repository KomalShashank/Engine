#pragma once

#include "Engine/PhysicsSystem/General/PhysicsCommons.hpp"
#include "Engine/PhysicsSystem/CollisionShape/CollisionShape.hpp"



class CapsuleShape final : public CollisionShape
{
public:
	CapsuleShape();
	~CapsuleShape();

	CollisionShape* CloneShape(BlockMemoryAllocator* blockAllocator) const override;

	uint8_t GetShapeType() const override;
	bool IsPointInside(const Transform2D& shapeTransform, const Vector2D& testPoint) const override;
	bool Raycast(const Transform2D& shapeTransform, const RaycastInput& raycastInput, RaycastResult& raycastResult) const override;

	AABB2D GetAABB(const Transform2D& shapeTransform) const override;
	void CalculateMassInfo(BodyMassInfo* bodyMassInfo, float shapeDensity) const override;

	void RenderShape(const Transform2D& shapeTransform, const RGBA* shapeColor) const override;

	void SetCapsuleLocalExtents(const Vector2D& firstExtent, const Vector2D& secondExtent);
	const Vector2D* GetCapsuleLocalExtents() const;

	void SetCapsuleRadius(float capsuleRadius);
	float GetCapsuleRadius() const;

	float GetCapsuleLength() const;
	float GetCapsuleWidth() const;

	Vector2D GetClosestPointOnCapsuleToExternalPoint(const Vector2D& externalPoint) const;
	void GetClosestPointsOnCapsuleAndExternalLine(const Vector2D& firstExternalPoint, const Vector2D& secondExternalPoint, Vector2D& closestPointOnCapsule, Vector2D& closestPointOnExternalLine) const;

private:
	void GetClosestPointToExternalPoint(const Vector2D& firstExtent, const Vector2D& secondExtent, const Vector2D& externalPoint, Vector2D& closestPoint, float& distanceToClosestPoint) const;

private:
	Vector2D m_FirstExtent;
	Vector2D m_SecondExtent;
	float m_CapsuleRadius;
};