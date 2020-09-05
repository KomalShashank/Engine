#include "Engine/PhysicsSystem/CollisionDetection/NarrowPhaseCollision.hpp"

#include "Engine/PhysicsSystem/CollisionShape/LineShape.hpp"
#include "Engine/PhysicsSystem/CollisionShape/CircleShape.hpp"
#include "Engine/PhysicsSystem/CollisionShape/CapsuleShape.hpp"
#include "Engine/PhysicsSystem/CollisionShape/PolygonShape.hpp"

#include "Engine/PhysicsSystem/RigidBody/RigidBody.hpp"



GlobalContactCluster::GlobalContactCluster()
{

}



GlobalContactCluster::~GlobalContactCluster()
{

}



void GlobalContactCluster::GenerateGlobalCluster(const LocalContactCluster* localContactCluster, const Transform2D& firstFixtureTransform, float firstFixtureBoundingRadius, const Transform2D& secondFixtureTransform, float secondFixtureBoundingRadius)
{
	if (localContactCluster->m_NumberOfContactPoints == 0U)
	{
		return;
	}

	switch (localContactCluster->m_ClusterType)
	{
	case CIRCLE_CLUSTER_TYPE:
	{
		m_GlobalReferenceNormal = Vector2D::X_AXIS;
		Vector2D firstPoint = Multiply(firstFixtureTransform, localContactCluster->m_LocalReferencePoint);
		Vector2D secondPoint = Multiply(secondFixtureTransform, localContactCluster->m_AllContactPoints[0].m_PointOfContact);
		Vector2D displacement = secondPoint - firstPoint;
		if (Vector2D::DotProduct(displacement, displacement) > FLT_EPSILON * FLT_EPSILON)
		{
			m_GlobalReferenceNormal = displacement.GetNormalizedVector2D();
		}

		Vector2D point_1 = firstPoint + (m_GlobalReferenceNormal * firstFixtureBoundingRadius);
		Vector2D point_2 = secondPoint - (m_GlobalReferenceNormal * secondFixtureBoundingRadius);
		m_AllGlobalContactPoints[0] = (point_1 + point_2) * 0.5f;
		m_SeparationDistance[0] = Vector2D::DotProduct(point_2 - point_1, m_GlobalReferenceNormal);
	}
		break;

	case FIRST_EDGE_CLUSTER_TYPE:
	{
		m_GlobalReferenceNormal = Multiply(firstFixtureTransform.m_Rotation, localContactCluster->m_LocalReferenceNormal);
		Vector2D firstPoint = Multiply(firstFixtureTransform, localContactCluster->m_LocalReferencePoint);

		for (size_t contactIndex = 0; contactIndex < localContactCluster->m_NumberOfContactPoints; ++contactIndex)
		{
			Vector2D secondPoint = Multiply(secondFixtureTransform, localContactCluster->m_AllContactPoints[contactIndex].m_PointOfContact);
			Vector2D point_1 = secondPoint + (m_GlobalReferenceNormal * (firstFixtureBoundingRadius - Vector2D::DotProduct(secondPoint - firstPoint, m_GlobalReferenceNormal)));
			Vector2D point_2 = secondPoint - (m_GlobalReferenceNormal * secondFixtureBoundingRadius);
			m_AllGlobalContactPoints[contactIndex] = (point_1 + point_2) * 0.5f;
			m_SeparationDistance[contactIndex] = Vector2D::DotProduct(point_2 - point_1, m_GlobalReferenceNormal);
		}
	}
		break;

	case SECOND_EDGE_CLUSTER_TYPE:
	{
		m_GlobalReferenceNormal = Multiply(secondFixtureTransform.m_Rotation, localContactCluster->m_LocalReferenceNormal);
		Vector2D firstPoint = Multiply(secondFixtureTransform, localContactCluster->m_LocalReferencePoint);

		for (size_t contactIndex = 0; contactIndex < localContactCluster->m_NumberOfContactPoints; ++contactIndex)
		{
			Vector2D secondPoint = Multiply(firstFixtureTransform, localContactCluster->m_AllContactPoints[contactIndex].m_PointOfContact);
			Vector2D point_1 = secondPoint + (m_GlobalReferenceNormal * (secondFixtureBoundingRadius - Vector2D::DotProduct(secondPoint - firstPoint, m_GlobalReferenceNormal)));
			Vector2D point_2 = secondPoint - (m_GlobalReferenceNormal * firstFixtureBoundingRadius);
			m_AllGlobalContactPoints[contactIndex] = (point_1 + point_2) * 0.5f;
			m_SeparationDistance[contactIndex] = Vector2D::DotProduct(point_2 - point_1, m_GlobalReferenceNormal);
		}

		m_GlobalReferenceNormal = m_GlobalReferenceNormal.GetNegatedVector2D();
	}
		break;
	}
}



size_t PerformClipping(const ClipVertex* inputClipVertices, ClipVertex* outputClipVertices, const Vector2D& normalVector, float offsetAmount, size_t vertexIndex)
{
	size_t numberOfOutputPoints = 0U;

	float firstDistance = Vector2D::DotProduct(normalVector, inputClipVertices[0].m_Vertex) - offsetAmount;
	float secondDistance = Vector2D::DotProduct(normalVector, inputClipVertices[1].m_Vertex) - offsetAmount;

	if (firstDistance <= 0.0f)
	{
		outputClipVertices[numberOfOutputPoints] = inputClipVertices[0];
		++numberOfOutputPoints;
	}

	if (secondDistance <= 0.0f)
	{
		outputClipVertices[numberOfOutputPoints] = inputClipVertices[1];
		++numberOfOutputPoints;
	}

	if (firstDistance * secondDistance < 0.0f)
	{
		float interpolationFactor = firstDistance / (firstDistance - secondDistance);
		outputClipVertices[numberOfOutputPoints].m_Vertex = inputClipVertices[0].m_Vertex + ((inputClipVertices[1].m_Vertex - inputClipVertices[0].m_Vertex) * interpolationFactor);

		outputClipVertices[numberOfOutputPoints].m_TypeID.m_TypeInfo.m_FirstContactType = VERTEX_CONTACT;
		outputClipVertices[numberOfOutputPoints].m_TypeID.m_TypeInfo.m_SecondContactType = EDGE_CONTACT;
		outputClipVertices[numberOfOutputPoints].m_TypeID.m_TypeInfo.m_FirstContactTypeIndex = (uint8_t)vertexIndex;
		outputClipVertices[numberOfOutputPoints].m_TypeID.m_TypeInfo.m_SecondContactTypeIndex = inputClipVertices[0].m_TypeID.m_TypeInfo.m_SecondContactTypeIndex;
		
		++numberOfOutputPoints;
	}

	return numberOfOutputPoints;
}



bool AreShapesOverlapping(const CollisionShape* firstShape, const Transform2D& firstTransform, const CollisionShape* secondShape, const Transform2D& secondTransform)
{
	ShapeReference firstShapeReference = ShapeReference(firstShape);
	ShapeReference secondShapeReference = ShapeReference(secondShape);

	SimplexData simplexData;
	simplexData.m_NumberOfVertices = 0U;
	float distanceBetweenShapes = ComputeDistanceBetweenShapes(simplexData, true, &firstShapeReference, firstTransform, &secondShapeReference, secondTransform);

	return (distanceBetweenShapes < 10.0f * FLT_EPSILON);
}



class PolygonCollision
{
public:
	PolygonCollision()
	{

	}

	void GenerateContactCluster(LocalContactCluster* localContactCluster, const Vector2D* firstFixtureEndPoints, float firstFixtureRadius, const Transform2D& firstFixtureTransform, const PolygonShape* secondFixtureShape, const Transform2D& secondFixtureTransform);
	float GetLineOrCapsuleSeparationDistance(uint8_t& separationType, uint8_t& resultantIndex) const;
	float GetPolygonSeparationDistance(uint8_t& separationType, uint8_t& resultantIndex) const;

public:
	Transform2D m_CombinedTransform;

	Vector2D m_PolygonVertices[MAXIMUM_POLYGON_VERTICES];
	Vector2D m_EdgeNormals[MAXIMUM_POLYGON_VERTICES];
	Vector2D m_PolygonCentroid;

	Vector2D m_FirstEndPoint;
	Vector2D m_SecondEndPoint;

	Vector2D m_LineOrCapsuleNormal;
	Vector2D m_Normal;

	Vector2D m_LowerLimit;
	Vector2D m_UpperLimit;

	float m_CombinedRadius;
	size_t m_NumberOfPolygonVertices;
	bool m_Reversed;
};



struct ClipFace
{
	Vector2D m_FirstVertex;
	Vector2D m_SecondVertex;
	Vector2D m_Normal;

	Vector2D m_FirstNormal;
	Vector2D m_SecondNormal;

	float m_FirstOffset;
	float m_SecondOffset;

	size_t m_FirstIndex;
	size_t m_SecondIndex;
};



void PolygonCollision::GenerateContactCluster(LocalContactCluster* localContactCluster, const Vector2D* firstFixtureEndPoints, float firstFixtureRadius, const Transform2D& firstFixtureTransform, const PolygonShape* secondFixtureShape, const Transform2D& secondFixtureTransform)
{
	m_CombinedTransform = MultiplyTranspose(firstFixtureTransform, secondFixtureTransform);
	m_PolygonCentroid = Multiply(m_CombinedTransform, secondFixtureShape->GetPolygonCentroid());

	m_FirstEndPoint = firstFixtureEndPoints[0];
	m_SecondEndPoint = firstFixtureEndPoints[1];

	Vector2D lineVector = (m_SecondEndPoint - m_FirstEndPoint).GetNormalizedVector2D();
	m_LineOrCapsuleNormal = Vector2D(lineVector.Y, -lineVector.X);

	m_Reversed = (Vector2D::DotProduct(m_LineOrCapsuleNormal, m_PolygonCentroid - m_FirstEndPoint) >= 0.0f);
	if (m_Reversed)
	{
		m_Normal = m_LineOrCapsuleNormal;
		m_LowerLimit = m_LineOrCapsuleNormal.GetNegatedVector2D();
		m_UpperLimit = m_LineOrCapsuleNormal.GetNegatedVector2D();
	}
	else
	{
		m_Normal = m_LineOrCapsuleNormal.GetNegatedVector2D();
		m_LowerLimit = m_LineOrCapsuleNormal;
		m_UpperLimit = m_LineOrCapsuleNormal;
	}

	m_NumberOfPolygonVertices = secondFixtureShape->GetNumberOfVertices();
	for (size_t vertexIndex = 0; vertexIndex < m_NumberOfPolygonVertices; ++vertexIndex)
	{
		m_PolygonVertices[vertexIndex] = Multiply(m_CombinedTransform, secondFixtureShape->GetVertex(vertexIndex));
		m_EdgeNormals[vertexIndex] = Multiply(m_CombinedTransform.m_Rotation, secondFixtureShape->GetEdgeNormal(vertexIndex));
	}

	m_CombinedRadius = firstFixtureRadius + secondFixtureShape->GetBoundingRadius();
	localContactCluster->m_NumberOfContactPoints = 0U;

	uint8_t lineSeparationType;
	uint8_t lineIndex;
	float lineSeparation = GetLineOrCapsuleSeparationDistance(lineSeparationType, lineIndex);
	if (lineSeparationType == 0U)
	{
		return;
	}

	if (lineSeparation > m_CombinedRadius)
	{
		return;
	}

	uint8_t polygonSeparationType;
	uint8_t polygonIndex;
	float polygonSeparation = GetPolygonSeparationDistance(polygonSeparationType, polygonIndex);
	if (polygonSeparationType != 0U && polygonSeparation > m_CombinedRadius)
	{
		return;
	}

	uint8_t separationType;
	uint8_t desiredIndex;
	float separationDistance;
	if (polygonSeparationType == 0U)
	{
		separationType = lineSeparationType;
		desiredIndex = lineIndex;
		separationDistance = lineSeparation;
	}
	else if (polygonSeparation > (0.98f * lineSeparation) + 0.001f)
	{
		separationType = polygonSeparationType;
		desiredIndex = polygonIndex;
		separationDistance = polygonSeparation;
	}
	else
	{
		separationType = lineSeparationType;
		desiredIndex = lineIndex;
		separationDistance = lineSeparation;
	}

	ClipVertex incidentEdge[2];
	ClipFace clipFace;
	if (separationType == 1U)
	{
		localContactCluster->m_ClusterType = FIRST_EDGE_CLUSTER_TYPE;

		size_t idealIndex = 0;
		float squaredDistance = Vector2D::DotProduct(m_Normal, m_EdgeNormals[0]);
		for (size_t vertexIndex = 1; vertexIndex < m_NumberOfPolygonVertices; ++vertexIndex)
		{
			float currentSquaredDistance = Vector2D::DotProduct(m_Normal, m_EdgeNormals[vertexIndex]);
			if (currentSquaredDistance < squaredDistance)
			{
				squaredDistance = currentSquaredDistance;
				idealIndex = vertexIndex;
			}
		}

		size_t firstVertexIndex = idealIndex;
		size_t secondVertexIndex = firstVertexIndex < (m_NumberOfPolygonVertices - 1U) ? firstVertexIndex + 1U : 0U;

		incidentEdge[0].m_Vertex = m_PolygonVertices[firstVertexIndex];
		incidentEdge[0].m_TypeID.m_TypeInfo.m_FirstContactType = EDGE_CONTACT;
		incidentEdge[0].m_TypeID.m_TypeInfo.m_SecondContactType = VERTEX_CONTACT;
		incidentEdge[0].m_TypeID.m_TypeInfo.m_FirstContactTypeIndex = 0U;
		incidentEdge[0].m_TypeID.m_TypeInfo.m_SecondContactTypeIndex = (uint8_t)firstVertexIndex;

		incidentEdge[1].m_Vertex = m_PolygonVertices[secondVertexIndex];
		incidentEdge[1].m_TypeID.m_TypeInfo.m_FirstContactType = EDGE_CONTACT;
		incidentEdge[1].m_TypeID.m_TypeInfo.m_SecondContactType = VERTEX_CONTACT;
		incidentEdge[1].m_TypeID.m_TypeInfo.m_FirstContactTypeIndex = 0U;
		incidentEdge[1].m_TypeID.m_TypeInfo.m_SecondContactTypeIndex = (uint8_t)secondVertexIndex;

		if (m_Reversed)
		{
			clipFace.m_FirstIndex = 0U;
			clipFace.m_SecondIndex = 1U;
			clipFace.m_FirstVertex = m_FirstEndPoint;
			clipFace.m_SecondVertex = m_SecondEndPoint;
			clipFace.m_Normal = m_LineOrCapsuleNormal;
		}
		else
		{
			clipFace.m_FirstIndex = 1U;
			clipFace.m_SecondIndex = 0U;
			clipFace.m_FirstVertex = m_SecondEndPoint;
			clipFace.m_SecondVertex = m_FirstEndPoint;
			clipFace.m_Normal = m_LineOrCapsuleNormal.GetNegatedVector2D();
		}
	}
	else
	{
		localContactCluster->m_ClusterType = SECOND_EDGE_CLUSTER_TYPE;

		incidentEdge[0].m_Vertex = m_FirstEndPoint;
		incidentEdge[0].m_TypeID.m_TypeInfo.m_FirstContactType = VERTEX_CONTACT;
		incidentEdge[0].m_TypeID.m_TypeInfo.m_SecondContactType = EDGE_CONTACT;
		incidentEdge[0].m_TypeID.m_TypeInfo.m_FirstContactTypeIndex = 0U;
		incidentEdge[0].m_TypeID.m_TypeInfo.m_SecondContactTypeIndex = (uint8_t)desiredIndex;

		incidentEdge[1].m_Vertex = m_SecondEndPoint;
		incidentEdge[1].m_TypeID.m_TypeInfo.m_FirstContactType = VERTEX_CONTACT;
		incidentEdge[1].m_TypeID.m_TypeInfo.m_SecondContactType = EDGE_CONTACT;
		incidentEdge[1].m_TypeID.m_TypeInfo.m_FirstContactTypeIndex = 0U;
		incidentEdge[1].m_TypeID.m_TypeInfo.m_SecondContactTypeIndex = (uint8_t)desiredIndex;

		clipFace.m_FirstIndex = desiredIndex;
		clipFace.m_SecondIndex = clipFace.m_FirstIndex < (m_NumberOfPolygonVertices - 1U) ? clipFace.m_FirstIndex + 1U : 0U;
		clipFace.m_FirstVertex = m_PolygonVertices[clipFace.m_FirstIndex];
		clipFace.m_SecondVertex = m_PolygonVertices[clipFace.m_SecondIndex];
		clipFace.m_Normal = m_EdgeNormals[clipFace.m_FirstIndex];
	}

	clipFace.m_FirstNormal = Vector2D(clipFace.m_Normal.Y, -clipFace.m_Normal.X);
	clipFace.m_SecondNormal = clipFace.m_FirstNormal.GetNegatedVector2D();
	clipFace.m_FirstOffset = Vector2D::DotProduct(clipFace.m_FirstNormal, clipFace.m_FirstVertex);
	clipFace.m_SecondOffset = Vector2D::DotProduct(clipFace.m_SecondNormal, clipFace.m_SecondVertex);

	ClipVertex firstClipVertices[2U];
	ClipVertex secondClipVertices[2U];
	size_t numberOfClipPoints;

	numberOfClipPoints = PerformClipping(incidentEdge, firstClipVertices, clipFace.m_FirstNormal, clipFace.m_FirstOffset, clipFace.m_FirstIndex);
	if (numberOfClipPoints < MAXIMUM_NUMBER_OF_CONTACT_POINTS)
	{
		return;
	}

	numberOfClipPoints = PerformClipping(firstClipVertices, secondClipVertices, clipFace.m_SecondNormal, clipFace.m_SecondOffset, clipFace.m_SecondIndex);
	if (numberOfClipPoints < MAXIMUM_NUMBER_OF_CONTACT_POINTS)
	{
		return;
	}

	if (separationType == 1U)
	{
		localContactCluster->m_LocalReferenceNormal = clipFace.m_Normal;
		localContactCluster->m_LocalReferencePoint = clipFace.m_FirstVertex;
	}
	else
	{
		localContactCluster->m_LocalReferenceNormal = secondFixtureShape->GetEdgeNormal(clipFace.m_FirstIndex);
		localContactCluster->m_LocalReferencePoint = secondFixtureShape->GetVertex(clipFace.m_FirstIndex);
	}

	size_t numberOfContactPoints = 0U;
	for (size_t contactIndex = 0; contactIndex < MAXIMUM_NUMBER_OF_CONTACT_POINTS; ++contactIndex)
	{
		if (Vector2D::DotProduct(clipFace.m_Normal, secondClipVertices[contactIndex].m_Vertex - clipFace.m_FirstVertex) <= m_CombinedRadius)
		{
			ContactPoint* currentContactPoint = localContactCluster->m_AllContactPoints + numberOfContactPoints;

			if (separationType == 1U)
			{
				currentContactPoint->m_PointOfContact = MultiplyTranspose(m_CombinedTransform, secondClipVertices[contactIndex].m_Vertex);
				currentContactPoint->m_TypeID = secondClipVertices[contactIndex].m_TypeID;
			}
			else
			{
				currentContactPoint->m_PointOfContact = secondClipVertices[contactIndex].m_Vertex;
				currentContactPoint->m_TypeID.m_TypeInfo.m_FirstContactType = secondClipVertices[contactIndex].m_TypeID.m_TypeInfo.m_SecondContactType;
				currentContactPoint->m_TypeID.m_TypeInfo.m_SecondContactType = secondClipVertices[contactIndex].m_TypeID.m_TypeInfo.m_FirstContactType;
				currentContactPoint->m_TypeID.m_TypeInfo.m_FirstContactTypeIndex = secondClipVertices[contactIndex].m_TypeID.m_TypeInfo.m_SecondContactTypeIndex;
				currentContactPoint->m_TypeID.m_TypeInfo.m_SecondContactTypeIndex = secondClipVertices[contactIndex].m_TypeID.m_TypeInfo.m_FirstContactTypeIndex;
			}

			++numberOfContactPoints;
		}
	}

	localContactCluster->m_NumberOfContactPoints = numberOfContactPoints;
}



float PolygonCollision::GetLineOrCapsuleSeparationDistance(uint8_t& separationType, uint8_t& resultantIndex) const
{
	separationType = 1U;
	resultantIndex = (m_Reversed) ? 0U : 1U;

	float separationDistance = FLT_MAX;
	for (size_t vertexIndex = 0; vertexIndex < m_NumberOfPolygonVertices; ++vertexIndex)
	{
		float currentDistance = Vector2D::DotProduct(m_Normal, m_PolygonVertices[vertexIndex] - m_FirstEndPoint);
		if (currentDistance < separationDistance)
		{
			separationDistance = currentDistance;
		}
	}

	return separationDistance;
}



float PolygonCollision::GetPolygonSeparationDistance(uint8_t& separationType, uint8_t& resultantIndex) const
{
	separationType = 0U;
	resultantIndex = 255U;

	float separationDistance = -FLT_MAX;
	for (size_t vertexIndex = 0; vertexIndex < m_NumberOfPolygonVertices; ++vertexIndex)
	{
		Vector2D currentEdgeNormal = m_EdgeNormals[vertexIndex].GetNegatedVector2D();

		float firstSeparationDistance = Vector2D::DotProduct(currentEdgeNormal, m_PolygonVertices[vertexIndex] - m_FirstEndPoint);
		float secondSeparationDistance = Vector2D::DotProduct(currentEdgeNormal, m_PolygonVertices[vertexIndex] - m_SecondEndPoint);
		float currentSeparationDistance = GetMinimum(firstSeparationDistance, secondSeparationDistance);

		if (currentSeparationDistance > m_CombinedRadius)
		{
			separationType = 2U;
			resultantIndex = (uint8_t)vertexIndex;
			separationDistance = currentSeparationDistance;
			break;
		}

		if (Vector2D::DotProduct(currentEdgeNormal, Vector2D(-m_Normal.Y, m_Normal.X)) >= 0.0f)
		{
			if (Vector2D::DotProduct(currentEdgeNormal - m_UpperLimit, m_Normal) < -ANGULAR_DELTA)
			{
				continue;
			}
		}
		else
		{
			if (Vector2D::DotProduct(currentEdgeNormal - m_LowerLimit, m_Normal) < -ANGULAR_DELTA)
			{
				continue;
			}
		}

		if (currentSeparationDistance > separationDistance)
		{
			separationType = 2U;
			resultantIndex = (uint8_t)vertexIndex;
			separationDistance = currentSeparationDistance;
		}
	}

	return separationDistance;
}



void GenerateContactClusterForLineVersusCircle(LocalContactCluster* localContactCluster, const LineShape* firstFixtureShape, const Transform2D& firstFixtureTransform, const CircleShape* secondFixtureShape, const Transform2D& secondFixtureTransform)
{
	localContactCluster->m_NumberOfContactPoints = 0U;

	Vector2D circleCenter = Multiply(secondFixtureTransform, secondFixtureShape->GetLocalCenter());
	Vector2D foreignPoint = MultiplyTranspose(firstFixtureTransform, circleCenter);

	const Vector2D* lineEndPoints = firstFixtureShape->GetLineLocalPoints();
	Vector2D firstEndPoint = lineEndPoints[0];
	Vector2D secondEndPoint = lineEndPoints[1];
	Vector2D lineVector = secondEndPoint - firstEndPoint;

	float firstDistance = Vector2D::DotProduct(lineVector, secondEndPoint - foreignPoint);
	float secondDistance = Vector2D::DotProduct(lineVector, foreignPoint - firstEndPoint);
	float combinedRadii = firstFixtureShape->GetBoundingRadius() + secondFixtureShape->GetCircleRadius();

	ContactTypeInfo contactTypeInfo;
	contactTypeInfo.m_SecondContactType = VERTEX_CONTACT;
	contactTypeInfo.m_SecondContactTypeIndex = 0U;

	if (secondDistance <= 0.0f)
	{
		Vector2D displacement = foreignPoint - firstEndPoint;
		float squaredDistance = Vector2D::DotProduct(displacement, displacement);
		if (squaredDistance > combinedRadii * combinedRadii)
		{
			return;
		}

		contactTypeInfo.m_FirstContactType = VERTEX_CONTACT;
		contactTypeInfo.m_FirstContactTypeIndex = 0U;

		localContactCluster->m_ClusterType = CIRCLE_CLUSTER_TYPE;
		localContactCluster->m_LocalReferenceNormal = Vector2D::ZERO;
		localContactCluster->m_LocalReferencePoint = firstEndPoint;
		localContactCluster->m_NumberOfContactPoints = 1U;
		
		localContactCluster->m_AllContactPoints[0].m_PointOfContact = secondFixtureShape->GetLocalCenter();
		localContactCluster->m_AllContactPoints[0].m_TypeID.m_ID = 0U;
		localContactCluster->m_AllContactPoints[0].m_TypeID.m_TypeInfo = contactTypeInfo;

		return;
	}

	if (firstDistance <= 0.0f)
	{
		Vector2D displacement = foreignPoint - secondEndPoint;
		float squaredDistance = Vector2D::DotProduct(displacement, displacement);
		if (squaredDistance > combinedRadii * combinedRadii)
		{
			return;
		}

		contactTypeInfo.m_FirstContactType = VERTEX_CONTACT;
		contactTypeInfo.m_FirstContactTypeIndex = 1U;

		localContactCluster->m_ClusterType = CIRCLE_CLUSTER_TYPE;
		localContactCluster->m_LocalReferenceNormal = Vector2D::ZERO;
		localContactCluster->m_LocalReferencePoint = secondEndPoint;
		localContactCluster->m_NumberOfContactPoints = 1U;

		localContactCluster->m_AllContactPoints[0].m_PointOfContact = secondFixtureShape->GetLocalCenter();
		localContactCluster->m_AllContactPoints[0].m_TypeID.m_ID = 0U;
		localContactCluster->m_AllContactPoints[0].m_TypeID.m_TypeInfo = contactTypeInfo;

		return;
	}

	float squaredLineLength = Vector2D::DotProduct(lineVector, lineVector);
	ASSERT_OR_DIE(squaredLineLength > 0.0f, "Line has zero length.");

	Vector2D pointOnLine = ((firstEndPoint * firstDistance) + (secondEndPoint * secondDistance)) * (1.0f / squaredLineLength);
	Vector2D displacement = foreignPoint - pointOnLine;
	float squaredDistance = Vector2D::DotProduct(displacement, displacement);
	if (squaredDistance > combinedRadii * combinedRadii)
	{
		return;
	}

	Vector2D lineNormal = Vector2D(-lineVector.Y, lineVector.X);
	if (Vector2D::DotProduct(lineNormal, foreignPoint - firstEndPoint) < 0.0f)
	{
		lineNormal = lineNormal.GetNegatedVector2D();
	}

	lineNormal = lineNormal.GetNormalizedVector2D();

	contactTypeInfo.m_FirstContactType = EDGE_CONTACT;
	contactTypeInfo.m_FirstContactTypeIndex = 0U;

	localContactCluster->m_ClusterType = FIRST_EDGE_CLUSTER_TYPE;
	localContactCluster->m_LocalReferenceNormal = lineNormal;
	localContactCluster->m_LocalReferencePoint = firstEndPoint;
	localContactCluster->m_NumberOfContactPoints = 1U;

	localContactCluster->m_AllContactPoints[0].m_PointOfContact = secondFixtureShape->GetLocalCenter();
	localContactCluster->m_AllContactPoints[0].m_TypeID.m_ID = 0U;
	localContactCluster->m_AllContactPoints[0].m_TypeID.m_TypeInfo = contactTypeInfo;
}



void GenerateContactClusterForLineVersusCapsule(LocalContactCluster* localContactCluster, const LineShape* firstFixtureShape, const Transform2D& firstFixtureTransform, const CapsuleShape* secondFixtureShape, const Transform2D& secondFixtureTransform)
{
	localContactCluster->m_NumberOfContactPoints = 0U;

	Vector2D firstExternalPoint = MultiplyTranspose(secondFixtureTransform, Multiply(firstFixtureTransform, firstFixtureShape->GetLineLocalPoints()[0]));
	Vector2D secondExternalPoint = MultiplyTranspose(secondFixtureTransform, Multiply(firstFixtureTransform, firstFixtureShape->GetLineLocalPoints()[1]));

	Vector2D closestPointOnCapsule = Vector2D::ZERO;
	Vector2D closestPointOnLine = Vector2D::ZERO;
	secondFixtureShape->GetClosestPointsOnCapsuleAndExternalLine(firstExternalPoint, secondExternalPoint, closestPointOnCapsule, closestPointOnLine);

	Vector2D displacement = closestPointOnCapsule - closestPointOnLine;
	float squaredDistance = Vector2D::DotProduct(displacement, displacement);
	float combinedRadii = firstFixtureShape->GetBoundingRadius() + secondFixtureShape->GetCapsuleRadius();

	if (squaredDistance > combinedRadii * combinedRadii)
	{
		return;
	}

	localContactCluster->m_ClusterType = CIRCLE_CLUSTER_TYPE;
	localContactCluster->m_LocalReferencePoint = MultiplyTranspose(firstFixtureTransform, Multiply(secondFixtureTransform, closestPointOnLine));
	localContactCluster->m_LocalReferenceNormal = Vector2D::ZERO;
	localContactCluster->m_NumberOfContactPoints = 1U;

	localContactCluster->m_AllContactPoints[0].m_PointOfContact = closestPointOnCapsule;
	localContactCluster->m_AllContactPoints[0].m_TypeID.m_ID = 0U;
}



void GenerateContactClusterForLineVersusPolygon(LocalContactCluster* localContactCluster, const LineShape* firstFixtureShape, const Transform2D& firstFixtureTransform, const PolygonShape* secondFixtureShape, const Transform2D& secondFixtureTransform)
{
	PolygonCollision linePolygonCollision;
	linePolygonCollision.GenerateContactCluster(localContactCluster, firstFixtureShape->GetLineLocalPoints(), firstFixtureShape->GetBoundingRadius(), firstFixtureTransform, secondFixtureShape, secondFixtureTransform);
}



void GenerateContactClusterForCircleVersusCircle(LocalContactCluster* localContactCluster, const CircleShape* firstFixtureShape, const Transform2D& firstFixtureTransform, const CircleShape* secondFixtureShape, const Transform2D& secondFixtureTransform)
{
	localContactCluster->m_NumberOfContactPoints = 0U;

	Vector2D firstCircleCenter = Multiply(firstFixtureTransform, firstFixtureShape->GetLocalCenter());
	Vector2D secondCircleCenter = Multiply(secondFixtureTransform, secondFixtureShape->GetLocalCenter());

	Vector2D displacement = secondCircleCenter - firstCircleCenter;
	float squaredDistance = Vector2D::DotProduct(displacement, displacement);
	float combinedRadii = firstFixtureShape->GetCircleRadius() + secondFixtureShape->GetCircleRadius();

	if (squaredDistance > combinedRadii * combinedRadii)
	{
		return;
	}

	localContactCluster->m_ClusterType = CIRCLE_CLUSTER_TYPE;
	localContactCluster->m_LocalReferencePoint = firstFixtureShape->GetLocalCenter();
	localContactCluster->m_LocalReferenceNormal = Vector2D::ZERO;
	localContactCluster->m_NumberOfContactPoints = 1U;

	localContactCluster->m_AllContactPoints[0].m_PointOfContact = secondFixtureShape->GetLocalCenter();
	localContactCluster->m_AllContactPoints[0].m_TypeID.m_ID = 0U;
}



void GenerateContactClusterForCircleVersusCapsule(LocalContactCluster* localContactCluster, const CircleShape* firstFixtureShape, const Transform2D& firstFixtureTransform, const CapsuleShape* secondFixtureShape, const Transform2D& secondFixtureTransform)
{
	localContactCluster->m_NumberOfContactPoints = 0U;

	Vector2D circleCenter = Multiply(firstFixtureTransform, firstFixtureShape->GetLocalCenter());
	Vector2D foreignPoint = MultiplyTranspose(secondFixtureTransform, circleCenter);

	const Vector2D* capsuleExtents = secondFixtureShape->GetCapsuleLocalExtents();
	Vector2D capsuleFirstExtent = capsuleExtents[0];
	Vector2D capsuleSecondExtent = capsuleExtents[1];
	Vector2D capsuleVector = capsuleSecondExtent - capsuleFirstExtent;

	float firstDistance = Vector2D::DotProduct(capsuleVector, capsuleSecondExtent - foreignPoint);
	float secondDistance = Vector2D::DotProduct(capsuleVector, foreignPoint - capsuleFirstExtent);
	float combinedRadii = firstFixtureShape->GetCircleRadius() + secondFixtureShape->GetCapsuleRadius();

	ContactTypeInfo contactTypeInfo;
	contactTypeInfo.m_SecondContactType = VERTEX_CONTACT;
	contactTypeInfo.m_SecondContactTypeIndex = 0U;

	if (secondDistance <= 0.0f)
	{
		Vector2D displacement = foreignPoint - capsuleFirstExtent;
		float squaredDistance = Vector2D::DotProduct(displacement, displacement);
		if (squaredDistance > combinedRadii * combinedRadii)
		{
			return;
		}

		contactTypeInfo.m_FirstContactType = VERTEX_CONTACT;
		contactTypeInfo.m_FirstContactTypeIndex = 0U;

		localContactCluster->m_ClusterType = CIRCLE_CLUSTER_TYPE;
		localContactCluster->m_LocalReferenceNormal = Vector2D::ZERO;
		localContactCluster->m_LocalReferencePoint = firstFixtureShape->GetLocalCenter();
		localContactCluster->m_NumberOfContactPoints = 1U;

		localContactCluster->m_AllContactPoints[0].m_PointOfContact = capsuleFirstExtent;
		localContactCluster->m_AllContactPoints[0].m_TypeID.m_ID = 0U;
		localContactCluster->m_AllContactPoints[0].m_TypeID.m_TypeInfo = contactTypeInfo;

		return;
	}

	if (firstDistance <= 0.0f)
	{
		Vector2D displacement = foreignPoint - capsuleSecondExtent;
		float squaredDistance = Vector2D::DotProduct(displacement, displacement);
		if (squaredDistance > combinedRadii * combinedRadii)
		{
			return;
		}

		contactTypeInfo.m_FirstContactType = VERTEX_CONTACT;
		contactTypeInfo.m_FirstContactTypeIndex = 0U;

		localContactCluster->m_ClusterType = CIRCLE_CLUSTER_TYPE;
		localContactCluster->m_LocalReferenceNormal = Vector2D::ZERO;
		localContactCluster->m_LocalReferencePoint = firstFixtureShape->GetLocalCenter();
		localContactCluster->m_NumberOfContactPoints = 1U;

		localContactCluster->m_AllContactPoints[0].m_PointOfContact = capsuleSecondExtent;
		localContactCluster->m_AllContactPoints[0].m_TypeID.m_ID = 0U;
		localContactCluster->m_AllContactPoints[0].m_TypeID.m_TypeInfo = contactTypeInfo;

		return;
	}

	float squaredCapsuleLength = Vector2D::DotProduct(capsuleVector, capsuleVector);
	ASSERT_OR_DIE(squaredCapsuleLength > 0.0f, "Capsule has zero length.");

	Vector2D pointOnCapsule = ((capsuleFirstExtent * firstDistance) + (capsuleSecondExtent * secondDistance)) * (1.0f / squaredCapsuleLength);
	Vector2D displacement = foreignPoint - pointOnCapsule;
	float squaredDistance = Vector2D::DotProduct(displacement, displacement);
	if (squaredDistance > combinedRadii * combinedRadii)
	{
		return;
	}

	Vector2D capsuleNormal = Vector2D(-capsuleVector.Y, capsuleVector.X);
	if (Vector2D::DotProduct(capsuleNormal, foreignPoint - capsuleFirstExtent) < 0.0f)
	{
		capsuleNormal = capsuleNormal.GetNegatedVector2D();
	}

	capsuleNormal = capsuleNormal.GetNormalizedVector2D();

	contactTypeInfo.m_FirstContactType = EDGE_CONTACT;
	contactTypeInfo.m_FirstContactTypeIndex = 0U;

	localContactCluster->m_ClusterType = SECOND_EDGE_CLUSTER_TYPE;
	localContactCluster->m_LocalReferenceNormal = capsuleNormal;
	localContactCluster->m_LocalReferencePoint = capsuleFirstExtent;
	localContactCluster->m_NumberOfContactPoints = 1U;

	localContactCluster->m_AllContactPoints[0].m_PointOfContact = firstFixtureShape->GetLocalCenter();
	localContactCluster->m_AllContactPoints[0].m_TypeID.m_ID = 0U;
	localContactCluster->m_AllContactPoints[0].m_TypeID.m_TypeInfo = contactTypeInfo;
}



void GenerateContactClusterForCircleVersusPolygon(LocalContactCluster* localContactCluster, const CircleShape* firstFixtureShape, const Transform2D& firstFixtureTransform, const PolygonShape* secondFixtureShape, const Transform2D& secondFixtureTransform)
{
	localContactCluster->m_NumberOfContactPoints = 0U;

	Vector2D circleCenter = Multiply(firstFixtureTransform, firstFixtureShape->GetLocalCenter());
	Vector2D foreignPoint = MultiplyTranspose(secondFixtureTransform, circleCenter);

	size_t normalIndex = 0U;
	float separationDistance = -FLT_MAX;
	float combinedRadii = firstFixtureShape->GetCircleRadius() + secondFixtureShape->GetBoundingRadius();
	size_t numberOfVertices = secondFixtureShape->GetNumberOfVertices();

	const Vector2D* polygonVertices = secondFixtureShape->GetVertices();
	const Vector2D* edgeNormals = secondFixtureShape->GetEdgeNormals();

	for (size_t vertexIndex = 0; vertexIndex < numberOfVertices; ++vertexIndex)
	{
		float currentDistance = Vector2D::DotProduct(edgeNormals[vertexIndex], foreignPoint - polygonVertices[vertexIndex]);
		if (currentDistance > combinedRadii)
		{
			return;
		}

		if (currentDistance > separationDistance)
		{
			separationDistance = currentDistance;
			normalIndex = vertexIndex;
		}
	}

	size_t firstVertexIndex = normalIndex;
	size_t secondVertexIndex = (firstVertexIndex < numberOfVertices - 1U) ? firstVertexIndex + 1U : 0U;

	Vector2D firstVertex = polygonVertices[firstVertexIndex];
	Vector2D secondVertex = polygonVertices[secondVertexIndex];

	if (separationDistance < FLT_EPSILON)
	{
		localContactCluster->m_NumberOfContactPoints = 1U;
		localContactCluster->m_ClusterType = SECOND_EDGE_CLUSTER_TYPE;
		localContactCluster->m_LocalReferenceNormal = edgeNormals[normalIndex];
		localContactCluster->m_LocalReferencePoint = (firstVertex + secondVertex) * 0.5f;
		localContactCluster->m_AllContactPoints[0].m_PointOfContact = firstFixtureShape->GetLocalCenter();
		localContactCluster->m_AllContactPoints[0].m_TypeID.m_ID = 0U;

		return;
	}

	float firstDistance = Vector2D::DotProduct(foreignPoint - firstVertex, secondVertex - firstVertex);
	float secondDistance = Vector2D::DotProduct(foreignPoint - secondVertex, firstVertex - secondVertex);
	if (firstDistance <= 0.0f)
	{
		Vector2D displacement = foreignPoint - firstVertex;
		if (Vector2D::DotProduct(displacement, displacement) > combinedRadii * combinedRadii)
		{
			return;
		}

		localContactCluster->m_NumberOfContactPoints = 1U;
		localContactCluster->m_ClusterType = SECOND_EDGE_CLUSTER_TYPE;
		localContactCluster->m_LocalReferenceNormal = displacement.GetNormalizedVector2D();
		localContactCluster->m_LocalReferencePoint = firstVertex;
		localContactCluster->m_AllContactPoints[0].m_PointOfContact = firstFixtureShape->GetLocalCenter();
		localContactCluster->m_AllContactPoints[0].m_TypeID.m_ID = 0U;
	}
	else if (secondDistance <= 0.0f)
	{
		Vector2D displacement = foreignPoint - secondVertex;
		if (Vector2D::DotProduct(displacement, displacement) > combinedRadii * combinedRadii)
		{
			return;
		}

		localContactCluster->m_NumberOfContactPoints = 1U;
		localContactCluster->m_ClusterType = SECOND_EDGE_CLUSTER_TYPE;
		localContactCluster->m_LocalReferenceNormal = displacement.GetNormalizedVector2D();
		localContactCluster->m_LocalReferencePoint = secondVertex;
		localContactCluster->m_AllContactPoints[0].m_PointOfContact = firstFixtureShape->GetLocalCenter();
		localContactCluster->m_AllContactPoints[0].m_TypeID.m_ID = 0U;
	}
	else
	{
		Vector2D midPoint = (firstVertex + secondVertex) * 0.5f;
		float currentDistance = Vector2D::DotProduct(foreignPoint - midPoint, edgeNormals[normalIndex]);
		if (currentDistance > combinedRadii)
		{
			return;
		}

		localContactCluster->m_NumberOfContactPoints = 1U;
		localContactCluster->m_ClusterType = SECOND_EDGE_CLUSTER_TYPE;
		localContactCluster->m_LocalReferenceNormal = edgeNormals[normalIndex];
		localContactCluster->m_LocalReferencePoint = midPoint;
		localContactCluster->m_AllContactPoints[0].m_PointOfContact = firstFixtureShape->GetLocalCenter();
		localContactCluster->m_AllContactPoints[0].m_TypeID.m_ID = 0U;
	}
}



void GenerateContactClusterForCapsuleVersusCapsule(LocalContactCluster* localContactCluster, const CapsuleShape* firstFixtureShape, const Transform2D& firstFixtureTransform, const CapsuleShape* secondFixtureShape, const Transform2D& secondFixtureTransform)
{
	localContactCluster->m_NumberOfContactPoints = 0U;

	Vector2D firstExternalPoint = MultiplyTranspose(firstFixtureTransform, Multiply(secondFixtureTransform, secondFixtureShape->GetCapsuleLocalExtents()[0]));
	Vector2D secondExternalPoint = MultiplyTranspose(firstFixtureTransform, Multiply(secondFixtureTransform, secondFixtureShape->GetCapsuleLocalExtents()[1]));

	Vector2D closestPointOnFirstCapsule = Vector2D::ZERO;
	Vector2D closestPointOnSecondCapsule = Vector2D::ZERO;
	firstFixtureShape->GetClosestPointsOnCapsuleAndExternalLine(firstExternalPoint, secondExternalPoint, closestPointOnFirstCapsule, closestPointOnSecondCapsule);

	Vector2D displacement = closestPointOnSecondCapsule - closestPointOnFirstCapsule;
	float squaredDistance = Vector2D::DotProduct(displacement, displacement);
	float combinedRadii = firstFixtureShape->GetCapsuleRadius() + secondFixtureShape->GetCapsuleRadius();

	if (squaredDistance > combinedRadii * combinedRadii)
	{
		return;
	}

	localContactCluster->m_ClusterType = CIRCLE_CLUSTER_TYPE;
	localContactCluster->m_LocalReferencePoint = closestPointOnFirstCapsule;
	localContactCluster->m_LocalReferenceNormal = Vector2D::ZERO;
	localContactCluster->m_NumberOfContactPoints = 1U;

	localContactCluster->m_AllContactPoints[0].m_PointOfContact = MultiplyTranspose(secondFixtureTransform, Multiply(firstFixtureTransform, closestPointOnSecondCapsule));
	localContactCluster->m_AllContactPoints[0].m_TypeID.m_ID = 0U;
}



void GenerateContactClusterForCapsuleVersusPolygon(LocalContactCluster* localContactCluster, const CapsuleShape* firstFixtureShape, const Transform2D& firstFixtureTransform, const PolygonShape* secondFixtureShape, const Transform2D& secondFixtureTransform)
{
	PolygonCollision capsulePolygonCollision;
	capsulePolygonCollision.GenerateContactCluster(localContactCluster, firstFixtureShape->GetCapsuleLocalExtents(), firstFixtureShape->GetCapsuleRadius(), firstFixtureTransform, secondFixtureShape, secondFixtureTransform);
}



static float GetMaximumSeparationDistance(size_t& edgeIndex, const PolygonShape* firstPolygonShape, const Transform2D& firstPolygonTransform, const PolygonShape* secondPolygonShape, const Transform2D& secondPolygonTransform)
{
	Transform2D combinedTransform2D = MultiplyTranspose(secondPolygonTransform, firstPolygonTransform);

	size_t idealIndex = 0U;
	float maximumSeparationDistance = -FLT_MAX;
	for (size_t firstVertexIndex = 0U; firstVertexIndex < firstPolygonShape->GetNumberOfVertices(); ++firstVertexIndex)
	{
		Vector2D currentNormal = Multiply(combinedTransform2D.m_Rotation, firstPolygonShape->GetEdgeNormal(firstVertexIndex));
		Vector2D currentVertex = Multiply(combinedTransform2D, firstPolygonShape->GetVertex(firstVertexIndex));

		float separationDistance = FLT_MAX;
		for (size_t secondVertexIndex = 0; secondVertexIndex < secondPolygonShape->GetNumberOfVertices(); ++secondVertexIndex)
		{
			float currentDistance = Vector2D::DotProduct(currentNormal, secondPolygonShape->GetVertex(secondVertexIndex) - currentVertex);
			if (currentDistance < separationDistance)
			{
				separationDistance = currentDistance;
			}
		}

		if (separationDistance > maximumSeparationDistance)
		{
			maximumSeparationDistance = separationDistance;
			idealIndex = firstVertexIndex;
		}
	}

	edgeIndex = idealIndex;

	return maximumSeparationDistance;
}



static void GetIncidentEdge(ClipVertex* incidentEdge, size_t& edgeIndex, const PolygonShape* firstPolygonShape, const Transform2D& firstPolygonTransform, const PolygonShape* secondPolygonShape, const Transform2D& secondPolygonTransform)
{
	ASSERT_OR_DIE(edgeIndex >= 0U && edgeIndex < firstPolygonShape->GetNumberOfVertices(), "Edge index out of range.");

	Vector2D firstNormal = MultiplyTranspose(secondPolygonTransform.m_Rotation, Multiply(firstPolygonTransform.m_Rotation, firstPolygonShape->GetEdgeNormal(edgeIndex)));

	size_t desiredIndex = 0;
	float minimumDistance = FLT_MAX;
	for (size_t vertexIndex = 0; vertexIndex < secondPolygonShape->GetNumberOfVertices(); ++vertexIndex)
	{
		float currentDistance = Vector2D::DotProduct(firstNormal, secondPolygonShape->GetEdgeNormal(vertexIndex));
		if (currentDistance < minimumDistance)
		{
			minimumDistance = currentDistance;
			desiredIndex = vertexIndex;
		}
	}

	size_t firstIndex = desiredIndex;
	size_t secondIndex = firstIndex < (secondPolygonShape->GetNumberOfVertices() - 1U) ? firstIndex + 1U : 0U;

	incidentEdge[0].m_Vertex = Multiply(secondPolygonTransform, secondPolygonShape->GetVertex(firstIndex));
	incidentEdge[0].m_TypeID.m_TypeInfo.m_FirstContactType = EDGE_CONTACT;
	incidentEdge[0].m_TypeID.m_TypeInfo.m_SecondContactType = VERTEX_CONTACT;
	incidentEdge[0].m_TypeID.m_TypeInfo.m_FirstContactTypeIndex = (uint8_t)edgeIndex;
	incidentEdge[0].m_TypeID.m_TypeInfo.m_SecondContactTypeIndex = (uint8_t)firstIndex;

	incidentEdge[1].m_Vertex = Multiply(secondPolygonTransform, secondPolygonShape->GetVertex(secondIndex));
	incidentEdge[1].m_TypeID.m_TypeInfo.m_FirstContactType = EDGE_CONTACT;
	incidentEdge[1].m_TypeID.m_TypeInfo.m_SecondContactType = VERTEX_CONTACT;
	incidentEdge[1].m_TypeID.m_TypeInfo.m_FirstContactTypeIndex = (uint8_t)edgeIndex;
	incidentEdge[1].m_TypeID.m_TypeInfo.m_SecondContactTypeIndex = (uint8_t)secondIndex;
}



void GenerateContactClusterForPolygonVersusPolygon(LocalContactCluster* localContactCluster, const PolygonShape* firstFixtureShape, const Transform2D& firstFixtureTransform, const PolygonShape* secondFixtureShape, const Transform2D& secondFixtureTransform)
{
	localContactCluster->m_NumberOfContactPoints = 0U;

	float combinedRadii = firstFixtureShape->GetBoundingRadius() + secondFixtureShape->GetBoundingRadius();

	size_t firstEdgeIndex = 0U;
	float firstSeparation = GetMaximumSeparationDistance(firstEdgeIndex, firstFixtureShape, firstFixtureTransform, secondFixtureShape, secondFixtureTransform);
	if (firstSeparation > combinedRadii)
	{
		return;
	}

	size_t secondEdgeIndex = 0U;
	float secondSeparation = GetMaximumSeparationDistance(secondEdgeIndex, secondFixtureShape, secondFixtureTransform, firstFixtureShape, firstFixtureTransform);
	if (secondSeparation > combinedRadii)
	{
		return;
	}

	const PolygonShape* firstPolygonShape;
	const PolygonShape* secondPolygonShape;

	Transform2D firstPolygonTransform;
	Transform2D secondPolygonTransform;

	size_t edgeIndex;
	bool reversed;

	if (secondSeparation > firstSeparation + 0.1f * LINEAR_DELTA)
	{
		firstPolygonShape = secondFixtureShape;
		secondPolygonShape = firstFixtureShape;
		firstPolygonTransform = secondFixtureTransform;
		secondPolygonTransform = firstFixtureTransform;
		edgeIndex = secondEdgeIndex;
		localContactCluster->m_ClusterType = SECOND_EDGE_CLUSTER_TYPE;
		reversed = true;
	}
	else
	{
		firstPolygonShape = firstFixtureShape;
		secondPolygonShape = secondFixtureShape;
		firstPolygonTransform = firstFixtureTransform;
		secondPolygonTransform = secondFixtureTransform;
		edgeIndex = firstEdgeIndex;
		localContactCluster->m_ClusterType = FIRST_EDGE_CLUSTER_TYPE;
		reversed = false;
	}

	ClipVertex incidentEdge[2];
	GetIncidentEdge(incidentEdge, edgeIndex, firstPolygonShape, firstPolygonTransform, secondPolygonShape, secondPolygonTransform);

	size_t firstVertexIndex = edgeIndex;
	size_t secondVertexIndex = edgeIndex < (firstPolygonShape->GetNumberOfVertices() - 1U) ? edgeIndex + 1U : 0U;

	Vector2D firstVertex = firstPolygonShape->GetVertex(firstVertexIndex);
	Vector2D secondVertex = firstPolygonShape->GetVertex(secondVertexIndex);

	Vector2D localTangent = (secondVertex - firstVertex).GetNormalizedVector2D();
	Vector2D localNormal = Vector2D::CrossProduct(localTangent, 1.0f);
	Vector2D midPoint = (firstVertex + secondVertex) * 0.5f;

	Vector2D globalTangent = Multiply(firstPolygonTransform.m_Rotation, localTangent);
	Vector2D globalNormal = Vector2D::CrossProduct(globalTangent, 1.0f);

	firstVertex = Multiply(firstPolygonTransform, firstVertex);
	secondVertex = Multiply(firstPolygonTransform, secondVertex);

	float frontOffset = Vector2D::DotProduct(globalNormal, firstVertex);

	float firstSideOffset = -Vector2D::DotProduct(globalTangent, firstVertex) + combinedRadii;
	float secondSideOffset = Vector2D::DotProduct(globalTangent, secondVertex) + combinedRadii;

	ClipVertex firstClipVertices[2U];
	ClipVertex secondClipVertices[2U];
	size_t numberOfClipPoints;

	numberOfClipPoints = PerformClipping(incidentEdge, firstClipVertices, globalTangent.GetNegatedVector2D(), firstSideOffset, firstVertexIndex);
	if (numberOfClipPoints < 2U)
	{
		return;
	}

	numberOfClipPoints = PerformClipping(firstClipVertices, secondClipVertices, globalTangent, secondSideOffset, secondVertexIndex);
	if (numberOfClipPoints < 2U)
	{
		return;
	}

	localContactCluster->m_LocalReferenceNormal = localNormal;
	localContactCluster->m_LocalReferencePoint = midPoint;

	size_t numberOfContactPoints = 0U;
	for (size_t vertexIndex = 0; vertexIndex < MAXIMUM_NUMBER_OF_CONTACT_POINTS; ++vertexIndex)
	{
		float separationDistance = Vector2D::DotProduct(globalNormal, secondClipVertices[vertexIndex].m_Vertex) - frontOffset;
		if (separationDistance <= combinedRadii)
		{
			ContactPoint* currentContactPoint = localContactCluster->m_AllContactPoints + numberOfContactPoints;
			currentContactPoint->m_PointOfContact = MultiplyTranspose(secondPolygonTransform, secondClipVertices[vertexIndex].m_Vertex);
			currentContactPoint->m_TypeID = secondClipVertices[vertexIndex].m_TypeID;
			if (reversed)
			{
				ContactTypeInfo typeInfo = currentContactPoint->m_TypeID.m_TypeInfo;
				currentContactPoint->m_TypeID.m_TypeInfo.m_FirstContactType = typeInfo.m_SecondContactType;
				currentContactPoint->m_TypeID.m_TypeInfo.m_SecondContactType = typeInfo.m_FirstContactType;
				currentContactPoint->m_TypeID.m_TypeInfo.m_FirstContactTypeIndex = typeInfo.m_SecondContactTypeIndex;
				currentContactPoint->m_TypeID.m_TypeInfo.m_SecondContactTypeIndex = typeInfo.m_FirstContactTypeIndex;
			}

			++numberOfContactPoints;
		}
	}

	localContactCluster->m_NumberOfContactPoints = numberOfContactPoints;
}



ShapeReference::ShapeReference() :
	m_ShapeVertices(nullptr),
	m_ShapeBoundingRadius(0.0f),
	m_NumberOfVertices(0U)
{

}



ShapeReference::ShapeReference(const CollisionShape* currentShape) :
	m_ShapeVertices(nullptr),
	m_ShapeBoundingRadius(0.0f),
	m_NumberOfVertices(0U)
{
	switch (currentShape->GetShapeType())
	{
	case LINE_SHAPE:
	{
		const LineShape* lineShape = static_cast<const LineShape*>(currentShape);
		m_ShapeVertices = lineShape->GetLineLocalPoints();
		m_ShapeBoundingRadius = lineShape->GetBoundingRadius();
		m_NumberOfVertices = 2U;
	}
		break;

	case CIRCLE_SHAPE:
	{
		const CircleShape* circleShape = static_cast<const CircleShape*>(currentShape);
		m_ShapeVertices = circleShape->GetCircleLocalPoint();
		m_ShapeBoundingRadius = circleShape->GetBoundingRadius();
		m_NumberOfVertices = 1U;
	}
		break;

	case CAPSULE_SHAPE:
	{
		const CapsuleShape* capsuleShape = static_cast<const CapsuleShape*>(currentShape);
		m_ShapeVertices = capsuleShape->GetCapsuleLocalExtents();
		m_ShapeBoundingRadius = capsuleShape->GetBoundingRadius();
		m_NumberOfVertices = 2U;
	}
		break;

	case POLYGON_SHAPE:
	{
		const PolygonShape* polygonShape = static_cast<const PolygonShape*>(currentShape);
		m_ShapeVertices = polygonShape->GetVertices();
		m_ShapeBoundingRadius = polygonShape->GetBoundingRadius();
		m_NumberOfVertices = polygonShape->GetNumberOfVertices();
	}
		break;
	}
}



ShapeReference::~ShapeReference()
{

}



Vector2D ShapeReference::GetVertex(size_t vertexIndex) const
{
	ASSERT_OR_DIE(vertexIndex >= 0U && vertexIndex < m_NumberOfVertices, "Vertex index out of range.");

	return m_ShapeVertices[vertexIndex];
}



size_t ShapeReference::GetSupportingVertexIndexToPoint(const Vector2D& externalPoint) const
{
	size_t supportingIndex = 0U;
	float supportingSquaredDistance = Vector2D::DotProduct(m_ShapeVertices[0], externalPoint);
	for (size_t vertexIndex = 1; vertexIndex < m_NumberOfVertices; ++vertexIndex)
	{
		float squaredDistance = Vector2D::DotProduct(m_ShapeVertices[vertexIndex], externalPoint);
		if (squaredDistance > supportingSquaredDistance)
		{
			supportingIndex = vertexIndex;
			supportingSquaredDistance = squaredDistance;
		}
	}

	return supportingIndex;
}



SimplexShape::SimplexShape(const SimplexData& simplexData, const ShapeReference* firstShapeReference, const Transform2D& firstShapeTransform, const ShapeReference* secondShapeReference, const Transform2D& secondShapeTransform)
{
	ASSERT_OR_DIE(simplexData.m_NumberOfVertices <= 3U, "Number of vertices are out of range.");

	m_NumberOfVertices = simplexData.m_NumberOfVertices;
	SimplexShapeVertex* simplexShapeVertices = &m_FirstVertex;
	for (size_t vertexIndex = 0; vertexIndex < m_NumberOfVertices; ++vertexIndex)
	{
		SimplexShapeVertex* currentVertex = simplexShapeVertices + vertexIndex;

		currentVertex->m_FirstShapeIndex = (size_t)simplexData.m_FirstShapeIndices[vertexIndex];
		currentVertex->m_SecondShapeIndex = (size_t)simplexData.m_SecondShapeIndices[vertexIndex];

		currentVertex->m_FirstShapePoint = Multiply(firstShapeTransform, firstShapeReference->GetVertex(currentVertex->m_FirstShapeIndex));
		currentVertex->m_SecondShapePoint = Multiply(secondShapeTransform, secondShapeReference->GetVertex(currentVertex->m_SecondShapeIndex));
		currentVertex->m_Displacement = currentVertex->m_SecondShapePoint - currentVertex->m_FirstShapePoint;
		
		currentVertex->m_BarycentricPoint = 0.0f;
	}

	if (m_NumberOfVertices > 1U)
	{
		float firstShapeArea = simplexData.m_SimplexArea;
		float secondShapeArea = GetShapeArea();
		if (secondShapeArea < (0.5f * firstShapeArea) || secondShapeArea > (2.0f * firstShapeArea) || secondShapeArea < FLT_EPSILON)
		{
			m_NumberOfVertices = 0U;
		}
	}

	if (m_NumberOfVertices == 0U)
	{
		SimplexShapeVertex* currentVertex = simplexShapeVertices + 0U;
		
		currentVertex->m_FirstShapeIndex = 0U;
		currentVertex->m_SecondShapeIndex = 0U;

		currentVertex->m_FirstShapePoint = Multiply(firstShapeTransform, firstShapeReference->GetVertex(0U));
		currentVertex->m_SecondShapePoint = Multiply(secondShapeTransform, secondShapeReference->GetVertex(0U));
		currentVertex->m_Displacement = currentVertex->m_SecondShapePoint - currentVertex->m_FirstShapePoint;

		currentVertex->m_BarycentricPoint = 1.0f;

		m_NumberOfVertices = 1U;
	}
}



SimplexShape SimplexShape::CreateSimplexShape(const SimplexData& simplexData, const ShapeReference* firstShapeReference, const Transform2D& firstShapeTransform, const ShapeReference* secondShapeReference, const Transform2D& secondShapeTransform)
{
	return SimplexShape(simplexData, firstShapeReference, firstShapeTransform, secondShapeReference, secondShapeTransform);
}



void SimplexShape::DestroySimplexShape(const SimplexShape& simplexShape, SimplexData& simplexData)
{
	simplexData.m_SimplexArea = simplexShape.GetShapeArea();
	simplexData.m_NumberOfVertices = (uint16_t)simplexShape.m_NumberOfVertices;

	const SimplexShapeVertex* simplexShapeVertices = &simplexShape.m_FirstVertex;
	for (size_t vertexIndex = 0; vertexIndex < simplexShape.m_NumberOfVertices; ++vertexIndex)
	{
		simplexData.m_FirstShapeIndices[vertexIndex] = (uint8_t)simplexShapeVertices[vertexIndex].m_FirstShapeIndex;
		simplexData.m_SecondShapeIndices[vertexIndex] = (uint8_t)simplexShapeVertices[vertexIndex].m_SecondShapeIndex;
	}
}



float SimplexShape::GetShapeArea() const
{
	switch (m_NumberOfVertices)
	{
	case 0U:
		return 0.0f;

	case 1U:
		return 0.0f;

	case 2U:
		return (m_FirstVertex.m_Displacement - m_SecondVertex.m_Displacement).GetVector2DMagnitude();

	case 3U:
		return Vector2D::CrossProduct(m_SecondVertex.m_Displacement - m_FirstVertex.m_Displacement, m_ThirdVertex.m_Displacement - m_FirstVertex.m_Displacement);

	default:
		return 0.0f;
	}
}



Vector2D SimplexShape::GetSimplexDirection() const
{
	switch (m_NumberOfVertices)
	{
	case 1U:
		return m_FirstVertex.m_Displacement.GetNegatedVector2D();

	case 2U:
	{
		Vector2D difference = m_SecondVertex.m_Displacement - m_FirstVertex.m_Displacement;
		float crossProductResult = Vector2D::CrossProduct(difference, m_FirstVertex.m_Displacement.GetNegatedVector2D());
		if (crossProductResult > 0.0f)
		{
			return Vector2D::CrossProduct(1.0f, difference);
		}
		else
		{
			return Vector2D::CrossProduct(difference, 1.0f);
		}
	}

	default:
		return Vector2D::ZERO;
	}
}



void SimplexShape::GetSimplexPoints(Vector2D& firstPoint, Vector2D& secondPoint) const
{
	switch (m_NumberOfVertices)
	{
	case 0U:
		break;

	case 1U:
		firstPoint = m_FirstVertex.m_FirstShapePoint;
		secondPoint = m_FirstVertex.m_SecondShapePoint;

		break;

	case 2U:
		firstPoint = (m_FirstVertex.m_FirstShapePoint * m_FirstVertex.m_BarycentricPoint) + (m_SecondVertex.m_FirstShapePoint * m_SecondVertex.m_BarycentricPoint);
		secondPoint = (m_FirstVertex.m_SecondShapePoint * m_FirstVertex.m_BarycentricPoint) + (m_SecondVertex.m_SecondShapePoint * m_SecondVertex.m_BarycentricPoint);

		break;

	case 3U:
		firstPoint = (m_FirstVertex.m_FirstShapePoint * m_FirstVertex.m_BarycentricPoint) + (m_SecondVertex.m_FirstShapePoint * m_SecondVertex.m_BarycentricPoint) + (m_ThirdVertex.m_FirstShapePoint * m_ThirdVertex.m_BarycentricPoint);
		secondPoint = firstPoint;

		break;

	default:
		break;
	}
}



void SimplexShape::ResolveForTwoPoints()
{
	Vector2D firstDisplacement = m_FirstVertex.m_Displacement;
	Vector2D secondDisplacement = m_SecondVertex.m_Displacement;
	Vector2D difference = secondDisplacement - firstDisplacement;

	float dpResult_1 = Vector2D::DotProduct(firstDisplacement, difference) * -1.0f;
	if (dpResult_1 <= 0.0f)
	{
		m_FirstVertex.m_BarycentricPoint = 1.0f;
		m_NumberOfVertices = 1U;

		return;
	}

	float dpResult_2 = Vector2D::DotProduct(secondDisplacement, difference);
	if (dpResult_2 <= 0.0f)
	{
		m_SecondVertex.m_BarycentricPoint = 1.0f;
		m_NumberOfVertices = 1U;
		m_FirstVertex = m_SecondVertex;

		return;
	}

	float denominator = 1.0f / (dpResult_1 + dpResult_2);
	m_FirstVertex.m_BarycentricPoint = dpResult_2 * denominator;
	m_SecondVertex.m_BarycentricPoint = dpResult_1 * denominator;

	m_NumberOfVertices = 2U;
}



void SimplexShape::ResolveForThreePoints()
{
	Vector2D firstDisplacement = m_FirstVertex.m_Displacement;
	Vector2D secondDisplacement = m_SecondVertex.m_Displacement;
	Vector2D thirdDisplacement = m_ThirdVertex.m_Displacement;

	Vector2D difference_12 = secondDisplacement - firstDisplacement;
	float firstDPResult_1 = Vector2D::DotProduct(secondDisplacement, difference_12);
	float firstDPResult_2 = Vector2D::DotProduct(firstDisplacement, difference_12) * -1.0f;

	Vector2D difference_13 = thirdDisplacement - firstDisplacement;
	float secondDPResult_1 = Vector2D::DotProduct(thirdDisplacement, difference_13);
	float secondDPResult_2 = Vector2D::DotProduct(firstDisplacement, difference_13) * -1.0f;

	Vector2D difference_23 = thirdDisplacement - secondDisplacement;
	float thirdDPResult_1 = Vector2D::DotProduct(thirdDisplacement, difference_23);
	float thirdDPResult_2 = Vector2D::DotProduct(secondDisplacement, difference_23) * -1.0f;

	float crossProductResult = Vector2D::CrossProduct(difference_12, difference_13);
	float result_1 = crossProductResult * Vector2D::CrossProduct(secondDisplacement, thirdDisplacement);
	float result_2 = crossProductResult * Vector2D::CrossProduct(thirdDisplacement, firstDisplacement);
	float result_3 = crossProductResult * Vector2D::CrossProduct(firstDisplacement, secondDisplacement);

	if (firstDPResult_2 <= 0.0f && secondDPResult_2 <= 0.0f)
	{
		m_FirstVertex.m_BarycentricPoint = 1.0f;
		m_NumberOfVertices = 1U;

		return;
	}

	if (firstDPResult_1 > 0.0f && firstDPResult_2 > 0.0f && result_3 <= 0.0f)
	{
		float denominator = 1.0f / (firstDPResult_1 + firstDPResult_2);
		m_FirstVertex.m_BarycentricPoint = firstDPResult_1 * denominator;
		m_SecondVertex.m_BarycentricPoint = firstDPResult_2 * denominator;
		m_NumberOfVertices = 2U;

		return;
	}

	if (secondDPResult_1 > 0.0f && secondDPResult_2 > 0.0f && result_2 <= 0.0f)
	{
		float denominator = 1.0f / (secondDPResult_1 + secondDPResult_2);
		m_FirstVertex.m_BarycentricPoint = secondDPResult_1 * denominator;
		m_ThirdVertex.m_BarycentricPoint = secondDPResult_2 * denominator;
		m_NumberOfVertices = 2U;
		m_SecondVertex = m_ThirdVertex;

		return;
	}

	if (firstDPResult_1 <= 0.0f && thirdDPResult_2 <= 0.0f)
	{
		m_SecondVertex.m_BarycentricPoint = 1.0f;
		m_NumberOfVertices = 1U;
		m_FirstVertex = m_SecondVertex;

		return;
	}

	if (secondDPResult_1 <= 0.0f && thirdDPResult_1 <= 0.0f)
	{
		m_ThirdVertex.m_BarycentricPoint = 1.0f;
		m_NumberOfVertices = 1U;
		m_FirstVertex = m_ThirdVertex;

		return;
	}

	if (thirdDPResult_1 > 0.0f && thirdDPResult_2 > 0.0f && result_1 <= 0.0f)
	{
		float denominator = 1.0f / (thirdDPResult_1 + thirdDPResult_2);
		m_SecondVertex.m_BarycentricPoint = thirdDPResult_1 * denominator;
		m_ThirdVertex.m_BarycentricPoint = firstDPResult_2 * denominator;
		m_NumberOfVertices = 2U;
		m_FirstVertex = m_ThirdVertex;

		return;
	}

	float denominator = 1.0f / (result_1 + result_2 + result_3);
	m_FirstVertex.m_BarycentricPoint = result_1 * denominator;
	m_SecondVertex.m_BarycentricPoint = result_2 * denominator;
	m_ThirdVertex.m_BarycentricPoint = result_3 * denominator;
	m_NumberOfVertices = 3U;
}



float ComputeDistanceBetweenShapes(SimplexData& simplexData, bool usingRadii, const ShapeReference* firstShapeReference, const Transform2D& firstShapeTransform, const ShapeReference* secondShapeReference, const Transform2D& secondShapeTransform)
{
	const size_t MAXIMUM_NUMBER_OF_ITERATIONS = 20U;
	
	SimplexShape simplexShape = SimplexShape::CreateSimplexShape(simplexData, firstShapeReference, firstShapeTransform, secondShapeReference, secondShapeTransform);
	SimplexShapeVertex* simplexShapeVertices = &simplexShape.m_FirstVertex;

	size_t previousFirstShapeIndices[3U];
	size_t previousSecondShapeIndices[3U];
	size_t numberOfIndices = 0U;

	size_t currentIteration = 0U;
	while (currentIteration < MAXIMUM_NUMBER_OF_ITERATIONS)
	{
		numberOfIndices = simplexShape.m_NumberOfVertices;
		for (size_t vertexIndex = 0; vertexIndex < numberOfIndices; ++vertexIndex)
		{
			previousFirstShapeIndices[vertexIndex] = simplexShapeVertices[vertexIndex].m_FirstShapeIndex;
			previousSecondShapeIndices[vertexIndex] = simplexShapeVertices[vertexIndex].m_SecondShapeIndex;
		}

		switch (simplexShape.m_NumberOfVertices)
		{
		case 1U:
			break;

		case 2U:
			simplexShape.ResolveForTwoPoints();
			break;

		case 3U:
			simplexShape.ResolveForThreePoints();
			break;

		default:
			break;
		}

		if (simplexShape.m_NumberOfVertices == 3U)
		{
			break;
		}

		Vector2D simplexDirection = simplexShape.GetSimplexDirection();
		if (simplexDirection.GetSquaredVector2DMagnitude() < FLT_EPSILON * FLT_EPSILON)
		{
			break;
		}

		SimplexShapeVertex* currentVertex = simplexShapeVertices + simplexShape.m_NumberOfVertices;
		currentVertex->m_FirstShapeIndex = firstShapeReference->GetSupportingVertexIndexToPoint(MultiplyTranspose(firstShapeTransform.m_Rotation, simplexDirection.GetNegatedVector2D()));
		currentVertex->m_FirstShapePoint = Multiply(firstShapeTransform, firstShapeReference->GetVertex(currentVertex->m_FirstShapeIndex));
		currentVertex->m_SecondShapeIndex = secondShapeReference->GetSupportingVertexIndexToPoint(MultiplyTranspose(secondShapeTransform.m_Rotation, simplexDirection));
		currentVertex->m_SecondShapePoint = Multiply(secondShapeTransform, secondShapeReference->GetVertex(currentVertex->m_SecondShapeIndex));
		currentVertex->m_Displacement = currentVertex->m_SecondShapePoint - currentVertex->m_FirstShapePoint;

		++currentIteration;

		bool pointAlreadyExists = false;
		for (size_t vertexIndex = 0; vertexIndex < numberOfIndices; ++vertexIndex)
		{
			if (currentVertex->m_FirstShapeIndex == previousFirstShapeIndices[vertexIndex] && currentVertex->m_SecondShapeIndex == previousSecondShapeIndices[vertexIndex])
			{
				pointAlreadyExists = true;
				break;
			}
		}

		if (pointAlreadyExists)
		{
			break;
		}

		++simplexShape.m_NumberOfVertices;
	}

	Vector2D firstPoint;
	Vector2D secondPoint;
	simplexShape.GetSimplexPoints(firstPoint, secondPoint);

	float distanceBetweenShapes = (firstPoint - secondPoint).GetVector2DMagnitude();
	SimplexShape::DestroySimplexShape(simplexShape, simplexData);

	if (usingRadii)
	{
		float firstShapeRadius = firstShapeReference->m_ShapeBoundingRadius;
		float secondShapeRadius = secondShapeReference->m_ShapeBoundingRadius;

		if (distanceBetweenShapes > (firstShapeRadius + secondShapeRadius) && distanceBetweenShapes > FLT_EPSILON)
		{
			distanceBetweenShapes -= (firstShapeRadius + secondShapeRadius);
		}
		else
		{
			distanceBetweenShapes = 0.0f;
		}
	}

	return distanceBetweenShapes;
}



ShapeSeparation::ShapeSeparation(const SimplexData& simplexData, float delta, const ShapeReference* firstShapeReference, const BodySweptShape* firstBodySweptShape, const ShapeReference* secondShapeReference, const BodySweptShape* secondBodySweptShape) :
	m_FirstShapeReference(firstShapeReference),
	m_SecondShapeReference(secondShapeReference),
	m_FirstBodySweptShape(firstBodySweptShape),
	m_SecondBodySweptShape(secondBodySweptShape)
{
	size_t numberOfVertices = simplexData.m_NumberOfVertices;
	ASSERT_OR_DIE(numberOfVertices > 0U && numberOfVertices < 3U, "Number of vertices out of range.");

	Transform2D firstBodyTransform = m_FirstBodySweptShape->GetLocalTransformAtDelta(delta);
	Transform2D secondBodyTransform = m_SecondBodySweptShape->GetLocalTransformAtDelta(delta);

	if (numberOfVertices == 1U)
	{
		Vector2D firstPoint = Multiply(firstBodyTransform, firstShapeReference->GetVertex(simplexData.m_FirstShapeIndices[0]));
		Vector2D secondPoint = Multiply(secondBodyTransform, secondShapeReference->GetVertex(simplexData.m_SecondShapeIndices[0]));
		m_SeparatingAxis = (secondPoint - firstPoint).GetNormalizedVector2D();
		m_SeparationType = POINTS_TYPE;

		return;
	}
	else if (simplexData.m_FirstShapeIndices[0] == simplexData.m_FirstShapeIndices[1])
	{
		Vector2D firstForeignPoint = secondShapeReference->GetVertex(simplexData.m_SecondShapeIndices[0]);
		Vector2D secondForeignPoint = secondShapeReference->GetVertex(simplexData.m_SecondShapeIndices[1]);

		m_SeparatingAxis = Vector2D::CrossProduct(secondForeignPoint - firstForeignPoint, 1.0f).GetNormalizedVector2D();
		Vector2D normalVector = Multiply(secondBodyTransform.m_Rotation, m_SeparatingAxis);

		m_LocalReferencePoint = (firstForeignPoint + secondForeignPoint) * 0.5f;
		Vector2D firstPoint = Multiply(secondBodyTransform, m_LocalReferencePoint);
		Vector2D secondPoint = Multiply(firstBodyTransform, firstShapeReference->GetVertex(simplexData.m_FirstShapeIndices[0]));

		if (Vector2D::DotProduct(secondPoint - firstPoint, normalVector) < 0.0f)
		{
			m_SeparatingAxis = m_SeparatingAxis.GetNegatedVector2D();
		}

		m_SeparationType = SECOND_EDGE_TYPE;

		return;
	}
	else
	{
		Vector2D firstForeignPoint = firstShapeReference->GetVertex(simplexData.m_FirstShapeIndices[0]);
		Vector2D secondForeignPoint = firstShapeReference->GetVertex(simplexData.m_FirstShapeIndices[1]);

		m_SeparatingAxis = Vector2D::CrossProduct(secondForeignPoint - firstForeignPoint, 1.0f).GetNormalizedVector2D();
		Vector2D normalVector = Multiply(firstBodyTransform.m_Rotation, m_SeparatingAxis);

		m_LocalReferencePoint = (firstForeignPoint + secondForeignPoint) * 0.5f;
		Vector2D firstPoint = Multiply(firstBodyTransform, m_LocalReferencePoint);
		Vector2D secondPoint = Multiply(secondBodyTransform, secondShapeReference->GetVertex(simplexData.m_SecondShapeIndices[0]));

		if (Vector2D::DotProduct(secondPoint - firstPoint, normalVector) < 0.0f)
		{
			m_SeparatingAxis = m_SeparatingAxis.GetNegatedVector2D();
		}

		m_SeparationType = FIRST_EDGE_TYPE;

		return;
	}
}



float ShapeSeparation::GetMinimumSeparationDistance(size_t& firstIndex, size_t& secondIndex, float delta) const
{
	Transform2D firstBodyTransform = m_FirstBodySweptShape->GetLocalTransformAtDelta(delta);
	Transform2D secondBodyTransform = m_SecondBodySweptShape->GetLocalTransformAtDelta(delta);

	switch (m_SeparationType)
	{
	case POINTS_TYPE:
	{
		Vector2D firstAxis = MultiplyTranspose(firstBodyTransform.m_Rotation, m_SeparatingAxis);
		Vector2D secondAxis = MultiplyTranspose(secondBodyTransform.m_Rotation, m_SeparatingAxis.GetNegatedVector2D());

		firstIndex = m_FirstShapeReference->GetSupportingVertexIndexToPoint(firstAxis);
		secondIndex = m_SecondShapeReference->GetSupportingVertexIndexToPoint(secondAxis);

		Vector2D firstPoint = Multiply(firstBodyTransform, m_FirstShapeReference->GetVertex(firstIndex));
		Vector2D secondPoint = Multiply(secondBodyTransform, m_SecondShapeReference->GetVertex(secondIndex));

		return Vector2D::DotProduct(secondPoint - firstPoint, m_SeparatingAxis);
	}

	case FIRST_EDGE_TYPE:
	{
		Vector2D normalVector = Multiply(firstBodyTransform.m_Rotation, m_SeparatingAxis);
		Vector2D firstPoint = Multiply(firstBodyTransform, m_LocalReferencePoint);

		Vector2D secondAxis = MultiplyTranspose(secondBodyTransform.m_Rotation, normalVector.GetNegatedVector2D());

		firstIndex = 255U;
		secondIndex = m_SecondShapeReference->GetSupportingVertexIndexToPoint(secondAxis);

		Vector2D secondPoint = Multiply(secondBodyTransform, m_SecondShapeReference->GetVertex(secondIndex));

		return Vector2D::DotProduct(secondPoint - firstPoint, normalVector);
	}

	case SECOND_EDGE_TYPE:
	{
		Vector2D normalVector = Multiply(secondBodyTransform.m_Rotation, m_SeparatingAxis);
		Vector2D firstPoint = Multiply(secondBodyTransform, m_LocalReferencePoint);

		Vector2D firstAxis = MultiplyTranspose(firstBodyTransform.m_Rotation, normalVector.GetNegatedVector2D());

		firstIndex = m_FirstShapeReference->GetSupportingVertexIndexToPoint(firstAxis);
		secondIndex = 255U;

		Vector2D secondPoint = Multiply(firstBodyTransform, m_FirstShapeReference->GetVertex(firstIndex));

		return Vector2D::DotProduct(secondPoint - firstPoint, normalVector);
	}

	default:
		firstIndex = 255U;
		secondIndex = 255U;
		
		return 0.0f;
	}
}



float ShapeSeparation::ComputeSeparationDistance(size_t firstIndex, size_t secondIndex, float delta) const
{
	Transform2D firstBodyTransform = m_FirstBodySweptShape->GetLocalTransformAtDelta(delta);
	Transform2D secondBodyTransform = m_SecondBodySweptShape->GetLocalTransformAtDelta(delta);

	switch (m_SeparationType)
	{
	case POINTS_TYPE:
	{
		Vector2D firstPoint = Multiply(firstBodyTransform, m_FirstShapeReference->GetVertex(firstIndex));
		Vector2D secondPoint = Multiply(secondBodyTransform, m_SecondShapeReference->GetVertex(secondIndex));

		return Vector2D::DotProduct(secondPoint - firstPoint, m_SeparatingAxis);
	}

	case FIRST_EDGE_TYPE:
	{
		Vector2D normalVector = Multiply(firstBodyTransform.m_Rotation, m_SeparatingAxis);
		Vector2D firstPoint = Multiply(firstBodyTransform, m_LocalReferencePoint);
		Vector2D secondPoint = Multiply(secondBodyTransform, m_SecondShapeReference->GetVertex(secondIndex));

		return Vector2D::DotProduct(secondPoint - firstPoint, normalVector);
	}

	case SECOND_EDGE_TYPE:
	{
		Vector2D normalVector = Multiply(secondBodyTransform.m_Rotation, m_SeparatingAxis);
		Vector2D firstPoint = Multiply(secondBodyTransform, m_LocalReferencePoint);
		Vector2D secondPoint = Multiply(firstBodyTransform, m_FirstShapeReference->GetVertex(firstIndex));

		return Vector2D::DotProduct(secondPoint - firstPoint, normalVector);
	}

	default:
		return 0.0f;
	}
}



float ComputeTimeOfImpact(float maximumTimeOfImpact, uint8_t& timeOfImpactState, const ShapeReference* firstShapeReference, const BodySweptShape* firstBodySweptShape, const ShapeReference* secondShapeReference, const BodySweptShape* secondBodySweptShape)
{
	const size_t MAXIMUM_NUMBER_OF_ITERATIONS = 20U;
	
	timeOfImpactState = UNKNOWN_STATE;
	float timeOfImpact = maximumTimeOfImpact;

	BodySweptShape firstSweptShape = *firstBodySweptShape;
	BodySweptShape secondSweptShape = *secondBodySweptShape;

	firstSweptShape.NormalizeAngles();
	secondSweptShape.NormalizeAngles();

	float combinedRadii = firstShapeReference->m_ShapeBoundingRadius + secondShapeReference->m_ShapeBoundingRadius;
	float desiredSeparation = GetMaximum(LINEAR_DELTA, combinedRadii - 3.0f * LINEAR_DELTA);
	float separationTolerance = 0.25f * LINEAR_DELTA;

	float currentTmeOfImpact = 0.0f;

	SimplexData simplexData;
	simplexData.m_NumberOfVertices = 0U;

	size_t currentIteration = 0U;
	while (true)
	{
		Transform2D firstBodyTransform = firstSweptShape.GetLocalTransformAtDelta(currentTmeOfImpact);
		Transform2D secondBodyTransform = secondSweptShape.GetLocalTransformAtDelta(currentTmeOfImpact);

		float distanceBetweenShapes = ComputeDistanceBetweenShapes(simplexData, false, firstShapeReference, firstBodyTransform, secondShapeReference, secondBodyTransform);
		if (distanceBetweenShapes <= 0.0f)
		{
			timeOfImpactState = OVERLAPPING_STATE;
			timeOfImpact = 0.0f;
			break;
		}

		if (distanceBetweenShapes < desiredSeparation + separationTolerance)
		{
			timeOfImpactState = TOUCHING_STATE;
			timeOfImpact = currentTmeOfImpact;
			break;
		}

		ShapeSeparation shapeSeparation(simplexData, currentTmeOfImpact, firstShapeReference, &firstSweptShape, secondShapeReference, &secondSweptShape);

		bool completed = false;
		float nextTimeOfImpact = maximumTimeOfImpact;
		size_t currentVertexIteration = 0U;
		while (true)
		{
			size_t firstIndex;
			size_t secondIndex;
			float minimumSeparationDistance = shapeSeparation.GetMinimumSeparationDistance(firstIndex, secondIndex, nextTimeOfImpact);

			if (minimumSeparationDistance > desiredSeparation + separationTolerance)
			{
				timeOfImpactState = SEPARATED_STATE;
				timeOfImpact = maximumTimeOfImpact;
				completed = true;
				break;
			}

			if (minimumSeparationDistance > desiredSeparation - separationTolerance)
			{
				currentTmeOfImpact = nextTimeOfImpact;
				break;
			}

			float separationDistance = shapeSeparation.ComputeSeparationDistance(firstIndex, secondIndex, currentTmeOfImpact);

			if (separationDistance < desiredSeparation - separationTolerance)
			{
				timeOfImpactState = FAILED_STATE;
				timeOfImpact = currentTmeOfImpact;
				completed = true;
				break;
			}

			if (separationDistance <= desiredSeparation + separationTolerance)
			{
				timeOfImpactState = TOUCHING_STATE;
				timeOfImpact = currentTmeOfImpact;
				completed = true;
				break;
			}

			size_t currentRootIteration = 0U;
			float time_1 = currentTmeOfImpact;
			float time_2 = nextTimeOfImpact;
			while (true)
			{
				float currentTime;
				if (currentRootIteration & 1U)
				{
					currentTime = time_1 + (desiredSeparation - separationDistance) * (time_2 - time_1) / (minimumSeparationDistance - separationDistance);
				}
				else
				{
					currentTime = 0.5f * (time_1 + time_2);
				}

				++currentRootIteration;

				float currentSeparationDistance = shapeSeparation.ComputeSeparationDistance(firstIndex, secondIndex, currentTime);
				if (Absolute(currentSeparationDistance - desiredSeparation) < separationTolerance)
				{
					nextTimeOfImpact = currentTime;
					break;
				}

				if (currentSeparationDistance > desiredSeparation)
				{
					time_1 = currentTime;
					separationDistance = currentSeparationDistance;
				}
				else
				{
					time_2 = currentTime;
					minimumSeparationDistance = currentSeparationDistance;
				}

				if (currentRootIteration >= 50U)
				{
					break;
				}
			}

			++currentVertexIteration;
			if (currentVertexIteration >= MAXIMUM_POLYGON_VERTICES)
			{
				break;
			}
		}

		++currentIteration;

		if (completed)
		{
			break;
		}

		if (currentIteration >= MAXIMUM_NUMBER_OF_ITERATIONS)
		{
			timeOfImpactState = FAILED_STATE;
			timeOfImpact = currentTmeOfImpact;
			break;
		}
	}

	return timeOfImpact;
}