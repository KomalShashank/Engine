#include "Engine/PhysicsSystem/CollisionShape/CollisionShape.hpp"



CollisionShape::CollisionShape() :
	m_BoundingRadius(2.0f * LINEAR_DELTA)
{

}



CollisionShape::~CollisionShape()
{

}



bool CollisionShape::IsOfType(uint8_t currentShapeType) const
{
	return (currentShapeType == GetShapeType());
}



float CollisionShape::GetBoundingRadius() const
{
	return m_BoundingRadius;
}