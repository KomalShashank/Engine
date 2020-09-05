#pragma once

#include "Engine/PhysicsSystem/General/PhysicsCommons.hpp"



class BodyMassInfo;
class BlockMemoryAllocator;
class RGBA;



enum ShapeType : uint8_t
{
	LINE_SHAPE,
	CIRCLE_SHAPE,
	CAPSULE_SHAPE,
	POLYGON_SHAPE,
	NUMBER_OF_SHAPE_TYPES,
	INVALID_SHAPE = 255U
};



class CollisionShape
{
public:
	CollisionShape();
	virtual ~CollisionShape();

	virtual CollisionShape* CloneShape(BlockMemoryAllocator* blockAllocator) const = 0;

	virtual uint8_t GetShapeType() const = 0;
	virtual bool IsOfType(uint8_t currentShapeType) const;
	virtual bool IsPointInside(const Transform2D& shapeTransform, const Vector2D& testPoint) const = 0;
	virtual bool Raycast(const Transform2D& shapeTransform, const RaycastInput& raycastInput, RaycastResult& raycastResult) const = 0;

	virtual AABB2D GetAABB(const Transform2D& shapeTransform) const = 0;
	virtual void CalculateMassInfo(BodyMassInfo* bodyMassInfo, float shapeDensity) const = 0;

	virtual void RenderShape(const Transform2D& shapeTransform, const RGBA* shapeColor) const = 0;

	float GetBoundingRadius() const;

protected:
	float m_BoundingRadius;
};