#include "Engine/PhysicsSystem/CollisionShape/CircleShape.hpp"
#include "Engine/PhysicsSystem/RigidBody/RigidBody.hpp"
#include "Engine/DataStructures/BlockMemoryAllocator.hpp"
#include "Engine/Renderer/RenderUtilities/AdvancedRenderer.hpp"



const size_t NUMBER_OF_SECTORS = 36U;
const float SECTOR_OFFSET = 360.0f / (float)NUMBER_OF_SECTORS;



CircleShape::CircleShape() :
	CollisionShape(),
	m_LocalCenter(Vector2D::ZERO),
	m_CircleRadius(0.0f)
{
	m_BoundingRadius = m_CircleRadius;
}



CircleShape::~CircleShape()
{

}



CollisionShape* CircleShape::CloneShape(BlockMemoryAllocator* blockAllocator) const
{
	void* allocatedMemory = blockAllocator->AllocateBlockMemory(sizeof(CircleShape));
	CircleShape* clonedShape = new(allocatedMemory) CircleShape;
	*clonedShape = *this;

	return clonedShape;
}



uint8_t CircleShape::GetShapeType() const
{
	return CIRCLE_SHAPE;
}



bool CircleShape::IsPointInside(const Transform2D& shapeTransform, const Vector2D& testPoint) const
{
	Vector2D circleCenter = Multiply(shapeTransform, m_LocalCenter);
	Vector2D displacement = testPoint - circleCenter;

	return (Vector2D::DotProduct(displacement, displacement) <= (m_CircleRadius * m_CircleRadius));
}



bool CircleShape::Raycast(const Transform2D& shapeTransform, const RaycastInput& raycastInput, RaycastResult& raycastResult) const
{
	Vector2D circlePosition = Multiply(shapeTransform, m_LocalCenter);
	Vector2D displacement = raycastInput.m_StartingPosition - circlePosition;
	float squaredDistanceToCircle = Vector2D::DotProduct(displacement, displacement) - (m_CircleRadius * m_CircleRadius);

	Vector2D rayVector = raycastInput.m_EndingPosition - raycastInput.m_StartingPosition;
	float squaredDistance = Vector2D::DotProduct(displacement, rayVector);
	float squaredLengthOfRay = Vector2D::DotProduct(rayVector, rayVector);
	float difference = (squaredDistance * squaredDistance) - (squaredLengthOfRay * squaredDistanceToCircle);

	if (difference < 0.0f || difference < FLT_EPSILON)
	{
		return false;
	}

	float impactFraction = -(squaredDistance + SquareRootOfValue(difference));

	if (impactFraction >= 0.0f && impactFraction <= (raycastInput.m_MaximumImpactFraction * squaredLengthOfRay))
	{
		impactFraction /= squaredLengthOfRay;
		raycastResult.m_ImpactFraction = impactFraction;
		raycastResult.m_ImpactNormal = (displacement + (rayVector * impactFraction)).GetNormalizedVector2D();

		return true;
	}

	return false;
}



AABB2D CircleShape::GetAABB(const Transform2D& shapeTransform) const
{
	Vector2D aabbCenter = Multiply(shapeTransform, m_LocalCenter);

	AABB2D shapeAABB;
	shapeAABB.minimums = Vector2D(aabbCenter.X - m_BoundingRadius, aabbCenter.Y - m_BoundingRadius);
	shapeAABB.maximums = Vector2D(aabbCenter.X + m_BoundingRadius, aabbCenter.Y + m_BoundingRadius);

	return shapeAABB;
}



void CircleShape::CalculateMassInfo(BodyMassInfo* bodyMassInfo, float shapeDensity) const
{
	bodyMassInfo->m_Mass = shapeDensity * (PI_VALUE * m_CircleRadius * m_CircleRadius);
	bodyMassInfo->m_CenterOfMass = m_LocalCenter;
	bodyMassInfo->m_MomentOfInertia = bodyMassInfo->m_Mass * (0.5f * m_CircleRadius * m_CircleRadius + Vector2D::DotProduct(m_LocalCenter, m_LocalCenter));
}



void CircleShape::RenderShape(const Transform2D& shapeTransform, const RGBA* shapeColor) const
{
	RenderHollowCircle(shapeTransform, shapeColor);
	RenderRadiusLine(shapeTransform, shapeColor);
}



void CircleShape::SetLocalCenter(const Vector2D& localCenter)
{
	m_LocalCenter = localCenter;
}



Vector2D CircleShape::GetLocalCenter() const
{
	return m_LocalCenter;
}



const Vector2D* CircleShape::GetCircleLocalPoint() const
{
	return &m_LocalCenter;
}



void CircleShape::SetCircleRadius(float circleRadius)
{
	m_CircleRadius = circleRadius;
	m_BoundingRadius = circleRadius;
}



float CircleShape::GetCircleRadius() const
{
	return m_CircleRadius;
}



void CircleShape::RenderHollowCircle(const Transform2D& shapeTransform, const RGBA* shapeColor) const
{
	Vertex3D circleVertices[NUMBER_OF_SECTORS];
	uint32_t circleIndices[NUMBER_OF_SECTORS];

	Vector2D radialDisplacement = Vector2D(m_CircleRadius, 0.0f);
	Vector2D worldPosition = Multiply(shapeTransform, m_LocalCenter);
	for (size_t sectorIndex = 0; sectorIndex < NUMBER_OF_SECTORS; ++sectorIndex)
	{
		Rotator2D sectorRotation = Multiply(shapeTransform.m_Rotation, Rotator2D(SECTOR_OFFSET * sectorIndex));
		Vector2D vertexPosition = worldPosition + Multiply(sectorRotation, radialDisplacement);
		circleVertices[sectorIndex].m_Position = Vector3(vertexPosition.X, vertexPosition.Y, 0.0f);
		circleVertices[sectorIndex].m_Color = *shapeColor;

		circleIndices[sectorIndex] = sectorIndex;
	}

	Mesh circleMesh(&circleVertices[0], &circleIndices[0], NUMBER_OF_SECTORS, NUMBER_OF_SECTORS);
	AdvancedRenderer::SingletonInstance()->DrawLineLoopMesh(&circleMesh, NUMBER_OF_SECTORS, NUMBER_OF_SECTORS);
}



void CircleShape::RenderRadiusLine(const Transform2D& shapeTransform, const RGBA* shapeColor) const
{
	const size_t NUMBER_OF_POINTS = 2U;
	Vector2D vertexPosition;
	
	Vertex3D radiusVertices[NUMBER_OF_POINTS];
	uint32_t radiusIndices[NUMBER_OF_POINTS] = { 0, 1 };

	vertexPosition = Multiply(shapeTransform, m_LocalCenter);
	radiusVertices[0].m_Position = Vector3(vertexPosition.X, vertexPosition.Y, 0.0f);
	radiusVertices[0].m_Color = *shapeColor;

	vertexPosition = Multiply(shapeTransform, m_LocalCenter + Vector2D(m_CircleRadius, 0.0f));
	radiusVertices[1].m_Position = Vector3(vertexPosition.X, vertexPosition.Y, 0.0f);
	radiusVertices[1].m_Color = *shapeColor;

	Mesh radiusMesh(&radiusVertices[0], &radiusIndices[0], NUMBER_OF_POINTS, NUMBER_OF_POINTS);
	AdvancedRenderer::SingletonInstance()->DrawLinesMesh(&radiusMesh, NUMBER_OF_POINTS, NUMBER_OF_POINTS);
}