#include "Engine/PhysicsSystem/CollisionShape/LineShape.hpp"
#include "Engine/PhysicsSystem/RigidBody/RigidBody.hpp"
#include "Engine/DataStructures/BlockMemoryAllocator.hpp"
#include "Engine/Renderer/RenderUtilities/AdvancedRenderer.hpp"



LineShape::LineShape() :
	CollisionShape(),
	m_FirstEndPoint(Vector2D::ZERO),
	m_SecondEndPoint(Vector2D::ZERO)
{

}



LineShape::~LineShape()
{

}



CollisionShape* LineShape::CloneShape(BlockMemoryAllocator* blockAllocator) const
{
	void* allocatedMemory = blockAllocator->AllocateBlockMemory(sizeof(LineShape));
	LineShape* clonedShape = new(allocatedMemory) LineShape;
	*clonedShape = *this;

	return clonedShape;
}



uint8_t LineShape::GetShapeType() const
{
	return LINE_SHAPE;
}



bool LineShape::IsPointInside(const Transform2D& shapeTransform, const Vector2D& testPoint) const
{
	UNUSED(shapeTransform);
	UNUSED(testPoint);

	return false;
}



bool LineShape::Raycast(const Transform2D& shapeTransform, const RaycastInput& raycastInput, RaycastResult& raycastResult) const
{
	Vector2D startingPosition = MultiplyTranspose(shapeTransform.m_Rotation, raycastInput.m_StartingPosition - shapeTransform.m_Position);
	Vector2D endingPosition = MultiplyTranspose(shapeTransform.m_Rotation, raycastInput.m_EndingPosition - shapeTransform.m_Position);
	Vector2D rayVector = endingPosition - startingPosition;

	Vector2D firstPoint = m_FirstEndPoint;
	Vector2D secondPoint = m_SecondEndPoint;
	Vector2D lineShapeVector = secondPoint - firstPoint;
	Vector2D lineNormal = Vector2D(lineShapeVector.Y, -lineShapeVector.X);
	lineNormal = lineNormal.GetNormalizedVector2D();

	float numerator = Vector2D::DotProduct(lineNormal, firstPoint - startingPosition);
	float denominator = Vector2D::DotProduct(lineNormal, rayVector);

	if (denominator <= FLT_EPSILON)
	{
		return false;
	}

	float impactFraction = numerator / denominator;
	if (impactFraction < 0.0f || impactFraction > raycastInput.m_MaximumImpactFraction)
	{
		return false;
	}

	Vector2D fractionedRay = startingPosition + (rayVector * impactFraction);
	float lineSquaredDistance = Vector2D::DotProduct(lineShapeVector, lineShapeVector);
	if (lineSquaredDistance <= FLT_EPSILON)
	{
		return false;
	}

	float dotProductResult = Vector2D::DotProduct(fractionedRay - firstPoint, lineShapeVector) / lineSquaredDistance;
	if (dotProductResult < 0.0f || dotProductResult > 1.0f)
	{
		return false;
	}

	raycastResult.m_ImpactFraction = impactFraction;
	raycastResult.m_ImpactNormal = Multiply(shapeTransform.m_Rotation, lineNormal);
	raycastResult.m_ImpactNormal *= (numerator > 0.0f) ? -1.0f : 1.0f;

	return true;
}



AABB2D LineShape::GetAABB(const Transform2D& shapeTransform) const
{
	Vector2D firstPoint = Multiply(shapeTransform, m_FirstEndPoint);
	Vector2D secondPoint = Multiply(shapeTransform, m_SecondEndPoint);

	Vector2D minimums = GetMinimum(firstPoint, secondPoint);
	Vector2D maximums = GetMaximum(firstPoint, secondPoint);

	AABB2D shapeAABB;
	shapeAABB.minimums = Vector2D(minimums.X - m_BoundingRadius, minimums.Y - m_BoundingRadius);
	shapeAABB.maximums = Vector2D(maximums.X + m_BoundingRadius, maximums.Y + m_BoundingRadius);

	return shapeAABB;
}



void LineShape::CalculateMassInfo(BodyMassInfo* bodyMassInfo, float shapeDensity) const
{
	UNUSED(shapeDensity);

	bodyMassInfo->m_Mass = 0.0f;
	bodyMassInfo->m_CenterOfMass = (m_FirstEndPoint + m_SecondEndPoint) * 0.5f;
	bodyMassInfo->m_MomentOfInertia = 0.0f;
}



void LineShape::RenderShape(const Transform2D& shapeTransform, const RGBA* shapeColor) const
{
	const size_t NUMBER_OF_POINTS = 2U;
	Vector2D vertexPosition;

	Vertex3D lineVertices[NUMBER_OF_POINTS];
	uint32_t lineIndices[NUMBER_OF_POINTS] = { 0, 1 };

	vertexPosition = Multiply(shapeTransform, m_FirstEndPoint);
	lineVertices[0].m_Position = Vector3(vertexPosition.X, vertexPosition.Y, 0.0f);
	lineVertices[0].m_Color = *shapeColor;

	vertexPosition = Multiply(shapeTransform, m_SecondEndPoint);
	lineVertices[1].m_Position = Vector3(vertexPosition.X, vertexPosition.Y, 0.0f);
	lineVertices[1].m_Color = *shapeColor;

	Mesh lineMesh(&lineVertices[0], &lineIndices[0], NUMBER_OF_POINTS, NUMBER_OF_POINTS);
	AdvancedRenderer::SingletonInstance()->DrawLinesMesh(&lineMesh, NUMBER_OF_POINTS, NUMBER_OF_POINTS);
}



void LineShape::SetLineLocalPoints(const Vector2D& firstEndPoint, const Vector2D& secondEndPoint)
{
	m_FirstEndPoint = firstEndPoint;
	m_SecondEndPoint = secondEndPoint;
}



const Vector2D* LineShape::GetLineLocalPoints() const
{
	return &m_FirstEndPoint;
}