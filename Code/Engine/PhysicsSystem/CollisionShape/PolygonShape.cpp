#include "Engine/PhysicsSystem/CollisionShape/PolygonShape.hpp"
#include "Engine/PhysicsSystem/RigidBody/RigidBody.hpp"
#include "Engine/DataStructures/BlockMemoryAllocator.hpp"
#include "Engine/Renderer/RenderUtilities/AdvancedRenderer.hpp"



const float ONE_OVER_THREE = 1.0f / 3.0f;



PolygonShape::PolygonShape() :
	CollisionShape(),
	m_PolygonCentroid(Vector2D::ZERO),
	m_NumberOfVertices(0U)
{

}



PolygonShape::~PolygonShape()
{

}



CollisionShape* PolygonShape::CloneShape(BlockMemoryAllocator* blockAllocator) const
{
	void* allocatedMemory = blockAllocator->AllocateBlockMemory(sizeof(PolygonShape));
	PolygonShape* clonedShape = new(allocatedMemory) PolygonShape;
	*clonedShape = *this;

	return clonedShape;
}



uint8_t PolygonShape::GetShapeType() const
{
	return POLYGON_SHAPE;
}



bool PolygonShape::IsPointInside(const Transform2D& shapeTransform, const Vector2D& testPoint) const
{
	Vector2D localTestPoint = MultiplyTranspose(shapeTransform.m_Rotation, testPoint - shapeTransform.m_Position);

	for (size_t vertexIndex = 0; vertexIndex < m_NumberOfVertices; ++vertexIndex)
	{
		float dotProductResult = Vector2D::DotProduct(m_EdgeNormals[vertexIndex], localTestPoint - m_PolygonVertices[vertexIndex]);
		if (dotProductResult > 0.0f)
		{
			return false;
		}
	}

	return true;
}



bool PolygonShape::Raycast(const Transform2D& shapeTransform, const RaycastInput& raycastInput, RaycastResult& raycastResult) const
{
	Vector2D startingPosition = MultiplyTranspose(shapeTransform.m_Rotation, raycastInput.m_StartingPosition - shapeTransform.m_Position);
	Vector2D endingPosition = MultiplyTranspose(shapeTransform.m_Rotation, raycastInput.m_EndingPosition - shapeTransform.m_Position);
	Vector2D rayVector = endingPosition - startingPosition;

	float minimumImpactFraction = 0.0f;
	float maximumImpactFraction = raycastInput.m_MaximumImpactFraction;

	int32_t normalIndex = -1;
	for (size_t vertexIndex = 0; vertexIndex < m_NumberOfVertices; ++vertexIndex)
	{
		float fractionNumerator = Vector2D::DotProduct(m_EdgeNormals[vertexIndex], m_PolygonVertices[vertexIndex] - startingPosition);
		float fractionDenominator = Vector2D::DotProduct(m_EdgeNormals[vertexIndex], rayVector);

		if (fractionDenominator <= FLT_EPSILON)
		{
			if (fractionNumerator < 0.0f)
			{
				return false;
			}
		}
		else
		{
			if (fractionDenominator < 0.0f && fractionNumerator < (minimumImpactFraction * fractionDenominator))
			{
				minimumImpactFraction = fractionNumerator / fractionDenominator;
				normalIndex = vertexIndex;
			}
			else if (fractionDenominator > 0.0f && fractionNumerator < (maximumImpactFraction * fractionDenominator))
			{
				maximumImpactFraction = fractionNumerator / fractionDenominator;
			}
		}

		if (maximumImpactFraction < minimumImpactFraction)
		{
			return false;
		}
	}

	ASSERT_OR_DIE(minimumImpactFraction >= 0.0f && minimumImpactFraction <= raycastInput.m_MaximumImpactFraction, "Fraction is out of range.");

	if (normalIndex >= 0)
	{
		raycastResult.m_ImpactFraction = minimumImpactFraction;
		raycastResult.m_ImpactNormal = Multiply(shapeTransform.m_Rotation, m_EdgeNormals[normalIndex]);

		return true;
	}

	return false;
}



AABB2D PolygonShape::GetAABB(const Transform2D& shapeTransform) const
{
	ASSERT_OR_DIE(m_NumberOfVertices >= 3U, "Number of vertices is less than minimum.");
	
	Vector2D minimums = Multiply(shapeTransform, m_PolygonVertices[0]);
	Vector2D maximums = minimums;

	for (size_t vertexIndex = 1; vertexIndex < m_NumberOfVertices; ++vertexIndex)
	{
		Vector2D currentVertex = Multiply(shapeTransform, m_PolygonVertices[vertexIndex]);
		minimums = GetMinimum(minimums, currentVertex);
		maximums = GetMaximum(maximums, currentVertex);
	}
	
	AABB2D shapeAABB;
	shapeAABB.minimums = Vector2D(minimums.X - m_BoundingRadius, minimums.Y - m_BoundingRadius);
	shapeAABB.maximums = Vector2D(maximums.X + m_BoundingRadius, maximums.Y + m_BoundingRadius);

	return shapeAABB;
}



void PolygonShape::CalculateMassInfo(BodyMassInfo* bodyMassInfo, float shapeDensity) const
{
	ASSERT_OR_DIE(m_NumberOfVertices >= 3U && m_NumberOfVertices <= MAXIMUM_POLYGON_VERTICES, "Number of vertices are out of allowed range.");

	Vector2D centerOfMass = Vector2D::ZERO;
	float totalArea = 0.0f;
	float momentOfInertia = 0.0f;

	Vector2D displacement = Vector2D::ZERO;
	for (size_t vertexIndex = 0; vertexIndex < m_NumberOfVertices; ++vertexIndex)
	{
		displacement += m_PolygonVertices[vertexIndex];
	}

	displacement *= 1.0f / m_NumberOfVertices;

	for (size_t vertexIndex = 0; vertexIndex < m_NumberOfVertices; ++vertexIndex)
	{
		Vector2D firstTriangleVertex = m_PolygonVertices[vertexIndex] - displacement;
		Vector2D secondTriangleVertex = (vertexIndex < m_NumberOfVertices - 1U) ? m_PolygonVertices[vertexIndex + 1] - displacement : m_PolygonVertices[0] - displacement;

		float crossProductResult = Vector2D::CrossProduct(firstTriangleVertex, secondTriangleVertex);
		float triangleArea = crossProductResult * 0.5f;
		totalArea += triangleArea;

		centerOfMass += (firstTriangleVertex + secondTriangleVertex) * triangleArea * ONE_OVER_THREE;

		float A = (firstTriangleVertex.X * firstTriangleVertex.X) + (secondTriangleVertex.X * firstTriangleVertex.X) + (secondTriangleVertex.X * secondTriangleVertex.X);
		float B = (firstTriangleVertex.Y * firstTriangleVertex.Y) + (secondTriangleVertex.Y * firstTriangleVertex.Y) + (secondTriangleVertex.Y * secondTriangleVertex.Y);

		momentOfInertia += (A + B) * (0.25f * crossProductResult * ONE_OVER_THREE);
	}

	bodyMassInfo->m_Mass = shapeDensity * totalArea;

	ASSERT_OR_DIE(totalArea > FLT_EPSILON, "Total area of polygon is too small.");
	centerOfMass *= 1.0f / totalArea;
	bodyMassInfo->m_CenterOfMass = centerOfMass + displacement;

	bodyMassInfo->m_MomentOfInertia = shapeDensity * momentOfInertia;
	bodyMassInfo->m_MomentOfInertia += bodyMassInfo->m_Mass * (Vector2D::DotProduct(bodyMassInfo->m_CenterOfMass, bodyMassInfo->m_CenterOfMass) - Vector2D::DotProduct(centerOfMass, centerOfMass));
}



void PolygonShape::RenderShape(const Transform2D& shapeTransform, const RGBA* shapeColor) const
{
	Vertex3D polygonVertices[MAXIMUM_POLYGON_VERTICES];
	uint32_t polygonIndices[MAXIMUM_POLYGON_VERTICES];

	for (size_t vertexIndex = 0; vertexIndex < m_NumberOfVertices; ++vertexIndex)
	{
		Vector2D vertexPosition = Multiply(shapeTransform, m_PolygonVertices[vertexIndex]);
		polygonVertices[vertexIndex].m_Position = Vector3(vertexPosition.X, vertexPosition.Y, 0.0f);
		polygonVertices[vertexIndex].m_Color = *shapeColor;

		polygonIndices[vertexIndex] = vertexIndex;
	}

	Mesh polygonMesh(&polygonVertices[0], &polygonIndices[0], m_NumberOfVertices, m_NumberOfVertices);
	AdvancedRenderer::SingletonInstance()->DrawLineLoopMesh(&polygonMesh, m_NumberOfVertices, m_NumberOfVertices);
}



void PolygonShape::SetVerticesAndCreateNormals(const Vector2D* polygonVertices, size_t numberOfVertices)
{
	ASSERT_OR_DIE(numberOfVertices >= 3U && numberOfVertices <= MAXIMUM_POLYGON_VERTICES, "Number of vertices are out of allowed range.");

	Vector2D temporaryVertices[MAXIMUM_POLYGON_VERTICES];
	size_t numberOfTemporaryVertices = 0U;

	for (size_t vertexIndex = 0; vertexIndex < numberOfVertices; ++vertexIndex)
	{
		Vector2D currentVertex = polygonVertices[vertexIndex];
		bool vertexIsUnique = true;

		for (size_t temporaryVertexIndex = 0; temporaryVertexIndex < numberOfTemporaryVertices; ++temporaryVertexIndex)
		{
			Vector2D displacement = currentVertex - temporaryVertices[temporaryVertexIndex];
			if (Vector2D::DotProduct(displacement, displacement) < ((0.5f * LINEAR_DELTA) * (0.5f * LINEAR_DELTA)))
			{
				vertexIsUnique = false;
				break;
			}
		}

		if (vertexIsUnique)
		{
			temporaryVertices[numberOfTemporaryVertices] = currentVertex;
			++numberOfTemporaryVertices;
		}
	}

	if (numberOfTemporaryVertices < 3U)
	{
		CreateAsSimpleQuad(Vector2D::ONE);

		return;
	}

	size_t rightMostVertexIndex = 0U;
	float rightMostVertexX = temporaryVertices[rightMostVertexIndex].X;
	for (size_t vertexIndex = 1; vertexIndex < numberOfTemporaryVertices; ++vertexIndex)
	{
		float currentVertexX = temporaryVertices[vertexIndex].X;
		float currentVertexY = temporaryVertices[vertexIndex].Y;

		float rightMostVertexY = temporaryVertices[rightMostVertexIndex].Y;
		if (currentVertexX > rightMostVertexX || (currentVertexX == rightMostVertexX && currentVertexY < rightMostVertexY))
		{
			rightMostVertexIndex = vertexIndex;
			rightMostVertexX = currentVertexX;
		}
	}

	size_t convexHullIndices[MAXIMUM_POLYGON_VERTICES];
	size_t numberOfHullVertices = 0U;
	size_t hullIndex = rightMostVertexIndex;

	while (true)
	{
		convexHullIndices[numberOfHullVertices] = hullIndex;

		size_t currentVertexIndex = 0;
		for (size_t vertexIndex = 1; vertexIndex < numberOfTemporaryVertices; ++vertexIndex)
		{
			if (currentVertexIndex == hullIndex)
			{
				currentVertexIndex = vertexIndex;
				continue;
			}

			Vector2D firstVector = temporaryVertices[currentVertexIndex] - temporaryVertices[convexHullIndices[numberOfHullVertices]];
			Vector2D secondVector = temporaryVertices[vertexIndex] - temporaryVertices[convexHullIndices[numberOfHullVertices]];
			
			float crossProductResult = Vector2D::CrossProduct(firstVector, secondVector);
			if (crossProductResult < 0.0f)
			{
				currentVertexIndex = vertexIndex;
			}

			if (crossProductResult == 0.0f && secondVector.GetSquaredVector2DMagnitude() > firstVector.GetSquaredVector2DMagnitude())
			{
				currentVertexIndex = vertexIndex;
			}
		}

		++numberOfHullVertices;
		hullIndex = currentVertexIndex;

		if (hullIndex == rightMostVertexIndex)
		{
			break;
		}
	}

	if (numberOfHullVertices < 3U)
	{
		CreateAsSimpleQuad(Vector2D::ONE);

		return;
	}

	m_NumberOfVertices = numberOfHullVertices;

	for (size_t vertexIndex = 0; vertexIndex < m_NumberOfVertices; ++vertexIndex)
	{
		m_PolygonVertices[vertexIndex] = temporaryVertices[convexHullIndices[vertexIndex]];
	}

	for (size_t vertexIndex = 0; vertexIndex < m_NumberOfVertices; ++vertexIndex)
	{
		size_t firstVertexIndex = vertexIndex;
		size_t secondVertexIndex = (vertexIndex < m_NumberOfVertices - 1U) ? firstVertexIndex + 1U : 0U;

		Vector2D edgeVector = m_PolygonVertices[secondVertexIndex] - m_PolygonVertices[firstVertexIndex];
		ASSERT_OR_DIE(edgeVector.GetSquaredVector2DMagnitude() > FLT_EPSILON * FLT_EPSILON, "Edge vector is too small.");

		m_EdgeNormals[vertexIndex] = Vector2D::CrossProduct(edgeVector, 1.0f);
		m_EdgeNormals[vertexIndex] = m_EdgeNormals[vertexIndex].GetNormalizedVector2D();
	}

	ComputePolygonCentroid();
}



void PolygonShape::CreateAsSimpleQuad(const Vector2D& quadHalfDimensions)
{
	m_PolygonVertices[0] = Vector2D(-quadHalfDimensions.X, -quadHalfDimensions.Y);
	m_PolygonVertices[1] = Vector2D(quadHalfDimensions.X, -quadHalfDimensions.Y);
	m_PolygonVertices[2] = Vector2D(quadHalfDimensions.X, quadHalfDimensions.Y);
	m_PolygonVertices[3] = Vector2D(-quadHalfDimensions.X, quadHalfDimensions.Y);

	m_EdgeNormals[0] = Vector2D(0.0f, -1.0f);
	m_EdgeNormals[1] = Vector2D(1.0f, 0.0f);
	m_EdgeNormals[2] = Vector2D(0.0f, 1.0f);
	m_EdgeNormals[3] = Vector2D(-1.0f, 0.0f);

	m_NumberOfVertices = 4U;
}



void PolygonShape::CreateAsSimpleQuad(const Vector2D& quadHalfDimensions, const Transform2D& quadTransform)
{
	CreateAsSimpleQuad(quadHalfDimensions);
	m_PolygonCentroid = quadTransform.m_Position;

	for (size_t vertexIndex = 0; vertexIndex < m_NumberOfVertices; ++vertexIndex)
	{
		m_PolygonVertices[vertexIndex] = Multiply(quadTransform, m_PolygonVertices[vertexIndex]);
		m_EdgeNormals[vertexIndex] = Multiply(quadTransform.m_Rotation, m_EdgeNormals[vertexIndex]);
	}
}



const Vector2D* PolygonShape::GetVertices() const
{
	return m_PolygonVertices;
}



Vector2D PolygonShape::GetVertex(size_t vertexIndex) const
{
	ASSERT_OR_DIE(vertexIndex >= 0U && vertexIndex < m_NumberOfVertices, "Vertex index out of range.");

	return m_PolygonVertices[vertexIndex];
}



const Vector2D* PolygonShape::GetEdgeNormals() const
{
	return m_EdgeNormals;
}



Vector2D PolygonShape::GetEdgeNormal(size_t normalIndex) const
{
	ASSERT_OR_DIE(normalIndex >= 0U && normalIndex < m_NumberOfVertices, "Normal index out of range.");
	
	return m_EdgeNormals[normalIndex];
}



Vector2D PolygonShape::GetPolygonCentroid() const
{
	return m_PolygonCentroid;
}



size_t PolygonShape::GetNumberOfVertices() const
{
	return m_NumberOfVertices;
}



bool PolygonShape::IsConvex() const
{
	for (size_t vertexIndex = 0; vertexIndex < m_NumberOfVertices; ++vertexIndex)
	{
		size_t firstVertexIndex = vertexIndex;
		size_t secondVertexIndex = (vertexIndex < m_NumberOfVertices - 1U) ? firstVertexIndex + 1U : 0U;

		Vector2D directionToSecondVertex = m_PolygonVertices[secondVertexIndex] - m_PolygonVertices[firstVertexIndex];
		for (size_t thirdVertex = 0; thirdVertex < m_NumberOfVertices; ++thirdVertex)
		{
			if (thirdVertex == firstVertexIndex || thirdVertex == secondVertexIndex)
			{
				continue;
			}

			Vector2D directionToThirdVertex = m_PolygonVertices[thirdVertex] - m_PolygonVertices[firstVertexIndex];
			float crossProductResult = Vector2D::CrossProduct(directionToSecondVertex, directionToThirdVertex);
			if (crossProductResult < 0.0f)
			{
				return false;
			}
		}
	}

	return true;
}



void PolygonShape::ComputePolygonCentroid()
{
	ASSERT_OR_DIE(m_NumberOfVertices >= 3U, "Number of vertices is less than minimum.");

	Vector2D polygonCentroid = Vector2D::ZERO;
	float totalArea = 0.0f;

	for (size_t vertexIndex = 0; vertexIndex < m_NumberOfVertices; ++vertexIndex)
	{
		Vector2D firstVertex = Vector2D::ZERO;
		Vector2D secondVertex = m_PolygonVertices[vertexIndex];
		Vector2D thirdVertex = (vertexIndex + 1 < m_NumberOfVertices) ? m_PolygonVertices[vertexIndex + 1] : m_PolygonVertices[0];

		Vector2D directionToSecondVertex = secondVertex - firstVertex;
		Vector2D directionToThirdVertex = thirdVertex - firstVertex;
		float crossProductResult = Vector2D::CrossProduct(directionToSecondVertex, directionToThirdVertex);

		float currentTriangleArea = crossProductResult * 0.5f;
		totalArea += currentTriangleArea;

		polygonCentroid += (firstVertex + secondVertex + thirdVertex) * currentTriangleArea * ONE_OVER_THREE;
	}

	ASSERT_OR_DIE(totalArea > FLT_EPSILON, "Total area of polygon is too small.");
	polygonCentroid *= 1.0f / totalArea;

	m_PolygonCentroid = polygonCentroid;
}