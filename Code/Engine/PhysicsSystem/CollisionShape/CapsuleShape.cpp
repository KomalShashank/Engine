#include "Engine/PhysicsSystem/CollisionShape/CapsuleShape.hpp"
#include "Engine/PhysicsSystem/RigidBody/RigidBody.hpp"
#include "Engine/DataStructures/BlockMemoryAllocator.hpp"
#include "Engine/Renderer/RenderUtilities/AdvancedRenderer.hpp"



const size_t NUMBER_OF_POINTS = 38U;
const size_t NUMBER_OF_HALF_CIRCLE_POINTS = NUMBER_OF_POINTS / 2U;
const float SECTOR_OFFSET = 10.0f;



CapsuleShape::CapsuleShape() :
	CollisionShape(),
	m_FirstExtent(Vector2D::ZERO),
	m_SecondExtent(Vector2D::ZERO),
	m_CapsuleRadius(0.0f)
{
	
}



CapsuleShape::~CapsuleShape()
{

}



CollisionShape* CapsuleShape::CloneShape(BlockMemoryAllocator* blockAllocator) const
{
	void* allocatedMemory = blockAllocator->AllocateBlockMemory(sizeof(CapsuleShape));
	CapsuleShape* clonedShape = new(allocatedMemory) CapsuleShape;
	*clonedShape = *this;

	return clonedShape;
}



uint8_t CapsuleShape::GetShapeType() const
{
	return CAPSULE_SHAPE;
}



bool CapsuleShape::IsPointInside(const Transform2D& shapeTransform, const Vector2D& testPoint) const
{
	Vector2D localTestPoint = MultiplyTranspose(shapeTransform, testPoint);
	Vector2D closestPoint = GetClosestPointOnCapsuleToExternalPoint(localTestPoint);
	Vector2D displacement = localTestPoint - closestPoint;

	return (Vector2D::DotProduct(displacement, displacement) <= (m_CapsuleRadius * m_CapsuleRadius));
}



bool CapsuleShape::Raycast(const Transform2D& shapeTransform, const RaycastInput& raycastInput, RaycastResult& raycastResult) const
{
	// TODO: Define this.

	return false;
}



AABB2D CapsuleShape::GetAABB(const Transform2D& shapeTransform) const
{
	Vector2D firstExtent = Multiply(shapeTransform, m_FirstExtent);
	Vector2D secondExtent = Multiply(shapeTransform, m_SecondExtent);

	Vector2D minimums = GetMinimum(firstExtent, secondExtent);
	Vector2D maximums = GetMaximum(firstExtent, secondExtent);

	AABB2D shapeAABB;
	shapeAABB.minimums = Vector2D(minimums.X - m_BoundingRadius, minimums.Y - m_BoundingRadius);
	shapeAABB.maximums = Vector2D(maximums.X + m_BoundingRadius, maximums.Y + m_BoundingRadius);

	return shapeAABB;
}



void CapsuleShape::CalculateMassInfo(BodyMassInfo* bodyMassInfo, float shapeDensity) const // TODO: Confirm.
{
	float extentWidth = GetCapsuleWidth();
	float extentLength = (m_SecondExtent - m_FirstExtent).GetVector2DMagnitude();

	float massOfRectangle = shapeDensity * (extentLength * extentWidth);
	float momentOfInertiaOfRectangle = (massOfRectangle / 12.0f) * ((extentLength * extentLength) + (extentWidth * extentWidth));

	float massOfCircle = shapeDensity * (PI_VALUE * m_CapsuleRadius * m_CapsuleRadius);
	float momentOfInertiaOfCircle = 0.5f * massOfCircle * m_CapsuleRadius * m_CapsuleRadius;
	float inertiaAroundCircleCenter = momentOfInertiaOfCircle * 0.5f;

	float massOfSemicircle = massOfCircle * 0.5f;
	float distanceToSemicircleCenterOfMass = (3.0f / 8.0f) * m_CapsuleRadius;
	float inertiaAroundCenterOfMass = inertiaAroundCircleCenter - (massOfSemicircle * distanceToSemicircleCenterOfMass * distanceToSemicircleCenterOfMass);

	float distanceToCapsuleCenterOfMass = distanceToSemicircleCenterOfMass + (extentLength * 0.5f);
	float momentOfInertiaOfSemiCircle = inertiaAroundCenterOfMass + (massOfSemicircle * distanceToCapsuleCenterOfMass * distanceToCapsuleCenterOfMass);
	float totalMomentOfInertia = momentOfInertiaOfRectangle + (momentOfInertiaOfSemiCircle * 2.0f);

	bodyMassInfo->m_Mass = massOfRectangle + (massOfSemicircle * 2.0f);
	bodyMassInfo->m_CenterOfMass = (m_FirstExtent + m_SecondExtent) * 0.5f;
	bodyMassInfo->m_MomentOfInertia = totalMomentOfInertia + (bodyMassInfo->m_Mass * Vector2D::DotProduct(bodyMassInfo->m_CenterOfMass, bodyMassInfo->m_CenterOfMass));
}



void CapsuleShape::RenderShape(const Transform2D& shapeTransform, const RGBA* shapeColor) const
{
	Vector2D firstExtent = Multiply(shapeTransform, m_FirstExtent);
	Vector2D secondExtent = Multiply(shapeTransform, m_SecondExtent);

	Vector2D directionToSecondExtent = secondExtent - firstExtent;
	Vector2D normalVector = Vector2D(-directionToSecondExtent.Y, directionToSecondExtent.X);
	normalVector = normalVector.GetNormalizedVector2D();
	Vector2D displacement = normalVector * m_CapsuleRadius;

	Vertex3D capsuleVertices[NUMBER_OF_POINTS];
	uint32_t capsuleIndices[NUMBER_OF_POINTS];

	for (size_t sectorIndex = 0; sectorIndex < NUMBER_OF_HALF_CIRCLE_POINTS; ++sectorIndex)
	{
		Rotator2D sectorRotation = Rotator2D(SECTOR_OFFSET * sectorIndex);
		Vector2D vertexPosition = firstExtent + Multiply(sectorRotation, displacement);
		capsuleVertices[sectorIndex].m_Position = Vector3(vertexPosition.X, vertexPosition.Y, 0.0f);
		capsuleVertices[sectorIndex].m_Color = *shapeColor;

		capsuleIndices[sectorIndex] = sectorIndex;
	}

	for (size_t sectorIndex = 0; sectorIndex < NUMBER_OF_HALF_CIRCLE_POINTS; ++sectorIndex)
	{
		Rotator2D sectorRotation = Rotator2D(SECTOR_OFFSET * sectorIndex);
		Vector2D vertexPosition = secondExtent - Multiply(sectorRotation, displacement);
		capsuleVertices[sectorIndex + NUMBER_OF_HALF_CIRCLE_POINTS].m_Position = Vector3(vertexPosition.X, vertexPosition.Y, 0.0f);
		capsuleVertices[sectorIndex + NUMBER_OF_HALF_CIRCLE_POINTS].m_Color = *shapeColor;

		capsuleIndices[sectorIndex + NUMBER_OF_HALF_CIRCLE_POINTS] = sectorIndex + NUMBER_OF_HALF_CIRCLE_POINTS;
	}

	Mesh capsuleMesh(&capsuleVertices[0], &capsuleIndices[0], NUMBER_OF_POINTS, NUMBER_OF_POINTS);
	AdvancedRenderer::SingletonInstance()->DrawLineLoopMesh(&capsuleMesh, NUMBER_OF_POINTS, NUMBER_OF_POINTS);
}



void CapsuleShape::SetCapsuleLocalExtents(const Vector2D& firstExtent, const Vector2D& secondExtent)
{
	m_FirstExtent = firstExtent;
	m_SecondExtent = secondExtent;
}



const Vector2D* CapsuleShape::GetCapsuleLocalExtents() const
{
	return &m_FirstExtent;
}



void CapsuleShape::SetCapsuleRadius(float capsuleRadius)
{
	m_CapsuleRadius = capsuleRadius;
	m_BoundingRadius = capsuleRadius;
}



float CapsuleShape::GetCapsuleRadius() const
{
	return m_CapsuleRadius;
}



float CapsuleShape::GetCapsuleLength() const
{
	float extentLength = (m_SecondExtent - m_FirstExtent).GetVector2DMagnitude();

	return ((m_CapsuleRadius * 2.0f) + extentLength);
}



float CapsuleShape::GetCapsuleWidth() const
{
	return (m_CapsuleRadius * 2.0f);
}



Vector2D CapsuleShape::GetClosestPointOnCapsuleToExternalPoint(const Vector2D& externalPoint) const
{
	Vector2D directionToExternalPoint = externalPoint - m_FirstExtent;
	Vector2D directionToSecondExtent = (m_SecondExtent - m_FirstExtent).GetNormalizedVector2D();

	float dotProductResult = Vector2D::DotProduct(directionToExternalPoint, directionToSecondExtent);

	if (dotProductResult <= 0.0f)
	{
		return m_FirstExtent;
	}
	else if (dotProductResult >= (m_SecondExtent - m_FirstExtent).GetVector2DMagnitude())
	{
		return m_SecondExtent;
	}
	else
	{
		return (m_FirstExtent + (directionToSecondExtent * dotProductResult));
	}
}



void CapsuleShape::GetClosestPointsOnCapsuleAndExternalLine(const Vector2D& firstExternalPoint, const Vector2D& secondExternalPoint, Vector2D& closestPointOnCapsule, Vector2D& closestPointOnExternalLine) const
{
	Vector2D firstPointOnCapsule = Vector2D::ZERO;
	float firstPointOnCapsuleDistance = 0.0f;
	GetClosestPointToExternalPoint(m_FirstExtent, m_SecondExtent, firstExternalPoint, firstPointOnCapsule, firstPointOnCapsuleDistance);

	Vector2D secondPointOnCapsule = Vector2D::ZERO;
	float secondPointOnCapsuleDistance = 0.0f;
	GetClosestPointToExternalPoint(m_FirstExtent, m_SecondExtent, secondExternalPoint, secondPointOnCapsule, secondPointOnCapsuleDistance);

	Vector2D firstPointOnExternalLine = Vector2D::ZERO;
	float firstPointOnExternalLineDistance = 0.0f;
	GetClosestPointToExternalPoint(firstExternalPoint, secondExternalPoint, m_FirstExtent, firstPointOnExternalLine, firstPointOnExternalLineDistance);

	Vector2D secondPointOnExternalLine = Vector2D::ZERO;
	float secondPointOnExternalLineDistance = 0.0f;
	GetClosestPointToExternalPoint(firstExternalPoint, secondExternalPoint, m_SecondExtent, secondPointOnExternalLine, secondPointOnExternalLineDistance);

	float shortestDistance = FLT_MAX;

	if (firstPointOnCapsuleDistance < shortestDistance)
	{
		shortestDistance = firstPointOnCapsuleDistance;
	}
	
	if (secondPointOnCapsuleDistance < shortestDistance)
	{
		shortestDistance = secondPointOnCapsuleDistance;
	}

	if (firstPointOnExternalLineDistance < shortestDistance)
	{
		shortestDistance = firstPointOnExternalLineDistance;
	}

	if (secondPointOnExternalLineDistance < shortestDistance)
	{
		shortestDistance = secondPointOnExternalLineDistance;
	}

	if (shortestDistance == firstPointOnCapsuleDistance)
	{
		closestPointOnCapsule = firstPointOnCapsule;
		closestPointOnExternalLine = firstExternalPoint;
	}
	else if (shortestDistance == secondPointOnCapsuleDistance)
	{
		closestPointOnCapsule = secondPointOnCapsule;
		closestPointOnExternalLine = secondExternalPoint;
	}
	else if (shortestDistance == firstPointOnExternalLineDistance)
	{
		closestPointOnCapsule = m_FirstExtent;
		closestPointOnExternalLine = firstPointOnExternalLine;
	}
	else if (shortestDistance == secondPointOnExternalLineDistance)
	{
		closestPointOnCapsule = m_SecondExtent;
		closestPointOnExternalLine = secondPointOnExternalLine;
	}
}



void CapsuleShape::GetClosestPointToExternalPoint(const Vector2D& firstExtent, const Vector2D& secondExtent, const Vector2D& externalPoint, Vector2D& closestPoint, float& distanceToClosestPoint) const
{
	Vector2D directionToExternalPoint = externalPoint - firstExtent;
	Vector2D directionToSecondExtent = (secondExtent - firstExtent).GetNormalizedVector2D();

	float dotProductResult = Vector2D::DotProduct(directionToExternalPoint, directionToSecondExtent);

	if (dotProductResult <= 0.0f)
	{
		closestPoint = firstExtent;
	}
	else if (dotProductResult >= (secondExtent - firstExtent).GetVector2DMagnitude())
	{
		closestPoint = secondExtent;
	}
	else
	{
		closestPoint = (firstExtent + (directionToSecondExtent * dotProductResult));
	}

	distanceToClosestPoint = (externalPoint - closestPoint).GetVector2DMagnitude();
}