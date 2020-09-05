#pragma once

#include "Engine/PhysicsSystem/General/PhysicsCommons.hpp"



class CollisionShape;
class LineShape;
class CircleShape;
class CapsuleShape;
class PolygonShape;
class BodySweptShape;



enum ContactType : uint8_t
{
	VERTEX_CONTACT,
	EDGE_CONTACT
};



enum ContactClusterType : uint8_t
{
	CIRCLE_CLUSTER_TYPE,
	FIRST_EDGE_CLUSTER_TYPE,
	SECOND_EDGE_CLUSTER_TYPE
};



enum TimeOfImpactState : uint8_t
{
	UNKNOWN_STATE,
	FAILED_STATE,
	OVERLAPPING_STATE,
	TOUCHING_STATE,
	SEPARATED_STATE
};



enum SeparationType : uint8_t
{
	POINTS_TYPE,
	FIRST_EDGE_TYPE,
	SECOND_EDGE_TYPE
};



struct ContactTypeInfo
{
	uint8_t m_FirstContactType;
	uint8_t m_FirstContactTypeIndex;

	uint8_t m_SecondContactType;
	uint8_t m_SecondContactTypeIndex;
};



union ContactTypeID
{
	ContactTypeInfo m_TypeInfo;
	uint32_t m_ID;
};



struct ContactPoint
{
	Vector2D m_PointOfContact;
	float m_PushBackImpulse;
	float m_FrictionImpulse;

	ContactTypeID m_TypeID;
};



struct LocalContactCluster
{
	ContactPoint m_AllContactPoints[MAXIMUM_NUMBER_OF_CONTACT_POINTS];

	Vector2D m_LocalReferencePoint;
	Vector2D m_LocalReferenceNormal;

	size_t m_NumberOfContactPoints;
	uint8_t m_ClusterType;
};



class GlobalContactCluster
{
public:
	GlobalContactCluster();
	~GlobalContactCluster();

	void GenerateGlobalCluster(const LocalContactCluster* localContactCluster,	const Transform2D& firstFixtureTransform, float firstFixtureBoundingRadius,
																				const Transform2D& secondFixtureTransform, float secondFixtureBoundingRadius);

public:
	Vector2D m_GlobalReferenceNormal;
	Vector2D m_AllGlobalContactPoints[MAXIMUM_NUMBER_OF_CONTACT_POINTS];
	float m_SeparationDistance[MAXIMUM_NUMBER_OF_CONTACT_POINTS];
};



struct ClipVertex
{
	Vector2D m_Vertex;
	ContactTypeID m_TypeID;
};



size_t PerformClipping(const ClipVertex* inputClipVertices, ClipVertex* outputClipVertices, const Vector2D& normalVector, float offsetAmount, size_t vertexIndex);



bool AreShapesOverlapping(const CollisionShape* firstShape, const Transform2D& firstTransform, const CollisionShape* secondShape, const Transform2D& secondTransform);



void GenerateContactClusterForLineVersusCircle(LocalContactCluster* localContactCluster,	const LineShape* firstFixtureShape, const Transform2D& firstFixtureTransform,
																							const CircleShape* secondFixtureShape, const Transform2D& secondFixtureTransform);

void GenerateContactClusterForLineVersusCapsule(LocalContactCluster* localContactCluster,	const LineShape* firstFixtureShape, const Transform2D& firstFixtureTransform,
																							const CapsuleShape* secondFixtureShape, const Transform2D& secondFixtureTransform);

void GenerateContactClusterForLineVersusPolygon(LocalContactCluster* localContactCluster,	const LineShape* firstFixtureShape, const Transform2D& firstFixtureTransform,
																							const PolygonShape* secondFixtureShape, const Transform2D& secondFixtureTransform);



void GenerateContactClusterForCircleVersusCircle(LocalContactCluster* localContactCluster,	const CircleShape* firstFixtureShape, const Transform2D& firstFixtureTransform,
																							const CircleShape* secondFixtureShape, const Transform2D& secondFixtureTransform);

void GenerateContactClusterForCircleVersusCapsule(LocalContactCluster* localContactCluster,	const CircleShape* firstFixtureShape, const Transform2D& firstFixtureTransform,
																							const CapsuleShape* secondFixtureShape, const Transform2D& secondFixtureTransform);

void GenerateContactClusterForCircleVersusPolygon(LocalContactCluster* localContactCluster,	const CircleShape* firstFixtureShape, const Transform2D& firstFixtureTransform,
																							const PolygonShape* secondFixtureShape, const Transform2D& secondFixtureTransform);



void GenerateContactClusterForCapsuleVersusCapsule(LocalContactCluster* localContactCluster,	const CapsuleShape* firstFixtureShape, const Transform2D& firstFixtureTransform,
																								const CapsuleShape* secondFixtureShape, const Transform2D& secondFixtureTransform);

void GenerateContactClusterForCapsuleVersusPolygon(LocalContactCluster* localContactCluster,	const CapsuleShape* firstFixtureShape, const Transform2D& firstFixtureTransform,
																								const PolygonShape* secondFixtureShape, const Transform2D& secondFixtureTransform);



void GenerateContactClusterForPolygonVersusPolygon(LocalContactCluster* localContactCluster,	const PolygonShape* firstFixtureShape, const Transform2D& firstFixtureTransform,
																								const PolygonShape* secondFixtureShape, const Transform2D& secondFixtureTransform);



class ShapeReference
{
public:
	ShapeReference();
	ShapeReference(const CollisionShape* currentShape);
	~ShapeReference();

	Vector2D GetVertex(size_t vertexIndex) const;
	size_t GetSupportingVertexIndexToPoint(const Vector2D& externalPoint) const;

public:
	const Vector2D* m_ShapeVertices;
	float m_ShapeBoundingRadius;
	size_t m_NumberOfVertices;
};



struct SimplexData
{
	float m_SimplexArea;
	uint16_t m_NumberOfVertices;
	uint8_t m_FirstShapeIndices[3U];
	uint8_t m_SecondShapeIndices[3U];
};



struct SimplexShapeVertex
{
	Vector2D m_FirstShapePoint;
	Vector2D m_SecondShapePoint;
	Vector2D m_Displacement;
	float m_BarycentricPoint;
	size_t m_FirstShapeIndex;
	size_t m_SecondShapeIndex;
};



class SimplexShape
{
private:
	SimplexShape(const SimplexData& simplexData,	const ShapeReference* firstShapeReference, const Transform2D& firstShapeTransform,
													const ShapeReference* secondShapeReference, const Transform2D& secondShapeTransform);

public:
	static SimplexShape CreateSimplexShape(const SimplexData& simplexData,	const ShapeReference* firstShapeReference, const Transform2D& firstShapeTransform,
																			const ShapeReference* secondShapeReference, const Transform2D& secondShapeTransform);
	static void DestroySimplexShape(const SimplexShape& simplexShape, SimplexData& simplexData);

	float GetShapeArea() const;
	Vector2D GetSimplexDirection() const;
	void GetSimplexPoints(Vector2D& firstPoint, Vector2D& secondPoint) const;

	void ResolveForTwoPoints();
	void ResolveForThreePoints();

public:
	SimplexShapeVertex m_FirstVertex;
	SimplexShapeVertex m_SecondVertex;
	SimplexShapeVertex m_ThirdVertex;

	size_t m_NumberOfVertices;
};



float ComputeDistanceBetweenShapes(SimplexData& simplexData, bool usingRadii,	const ShapeReference* firstShapeReference, const Transform2D& firstShapeTransform,
																				const ShapeReference* secondShapeReference, const Transform2D& secondShapeTransform);



class ShapeSeparation
{
public:
	ShapeSeparation(const SimplexData& simplexData, float delta,	const ShapeReference* firstShapeReference, const BodySweptShape* firstBodySweptShape,
																	const ShapeReference* secondShapeReference, const BodySweptShape* secondBodySweptShape);

	float GetMinimumSeparationDistance(size_t& firstIndex, size_t& secondIndex, float delta) const;
	float ComputeSeparationDistance(size_t firstIndex, size_t secondIndex, float delta) const;

public:
	Vector2D m_SeparatingAxis;
	Vector2D m_LocalReferencePoint;

	const ShapeReference* m_FirstShapeReference;
	const ShapeReference* m_SecondShapeReference;
	
	const BodySweptShape* m_FirstBodySweptShape;
	const BodySweptShape* m_SecondBodySweptShape;

	uint8_t m_SeparationType;
};



float ComputeTimeOfImpact(float maximumTimeOfImpact, uint8_t& timeOfImpactState,	const ShapeReference* firstShapeReference, const BodySweptShape* firstBodySweptShape,
																					const ShapeReference* secondShapeReference, const BodySweptShape* secondBodySweptShape);