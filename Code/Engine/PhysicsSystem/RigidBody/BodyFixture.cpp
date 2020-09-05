#include "Engine/PhysicsSystem/RigidBody/BodyFixture.hpp"
#include "Engine/PhysicsSystem/RigidBody/RigidBody.hpp"
#include "Engine/PhysicsSystem/CollisionShape/CollisionShape.hpp"
#include "Engine/PhysicsSystem/CollisionShape/LineShape.hpp"
#include "Engine/PhysicsSystem/CollisionShape/CircleShape.hpp"
#include "Engine/PhysicsSystem/CollisionShape/CapsuleShape.hpp"
#include "Engine/PhysicsSystem/CollisionShape/PolygonShape.hpp"
#include "Engine/PhysicsSystem/CollisionDetection/BroadPhaseCollision.hpp"
#include "Engine/DataStructures/BlockMemoryAllocator.hpp"
#include "Engine/Renderer/RenderUtilities/AdvancedRenderer.hpp"



const RGBA STATIC_BODY_AWAKE_COLOR = RGBA(0, 191, 255, 255);
const RGBA STATIC_BODY_SLEEP_COLOR = RGBA(135, 206, 250, 255);
const RGBA KINEMATIC_BODY_AWAKE_COLOR = RGBA(238, 221, 130, 255);
const RGBA KINEMATIC_BODY_SLEEP_COLOR = RGBA(218, 165, 32, 255);
const RGBA DYNAMIC_BODY_AWAKE_COLOR = RGBA(173, 255, 47, 255);
const RGBA DYNAMIC_BODY_SLEEP_COLOR = RGBA(34, 139, 34, 255);



BodyFixture::BodyFixture() :
	m_ParentBody(nullptr),
	m_NextFixture(nullptr)
{

}



BodyFixture::~BodyFixture()
{

}



void BodyFixture::RenderFixtureShape(const Transform2D& shapeTransform) const
{
	RGBA shapeColor;
	GetShapeColorFromParentBodyState(&shapeColor);
	
	const CollisionShape* fixtureShape = GetFixtureShape();
	fixtureShape->RenderShape(shapeTransform, &shapeColor);
}



void BodyFixture::GetShapeColorFromParentBodyState(RGBA* shapeColor) const
{
	if (m_ParentBody->IsOfType(STATIC_BODY))
	{
		*shapeColor = (m_ParentBody->IsBodyAwake()) ? STATIC_BODY_AWAKE_COLOR : STATIC_BODY_SLEEP_COLOR;
	}
	else if (m_ParentBody->IsOfType(KINEMATIC_BODY))
	{
		*shapeColor = (m_ParentBody->IsBodyAwake()) ? KINEMATIC_BODY_AWAKE_COLOR : KINEMATIC_BODY_SLEEP_COLOR;
	}
	else if (m_ParentBody->IsOfType(DYNAMIC_BODY))
	{
		*shapeColor = (m_ParentBody->IsBodyAwake()) ? DYNAMIC_BODY_AWAKE_COLOR : DYNAMIC_BODY_SLEEP_COLOR;
	}
}



BodyFixture* BodyFixture::CreateFixture(BlockMemoryAllocator* blockAllocator, const BodyFixtureData& fixtureData, RigidBody* parentBody)
{
	void* allocatedMemory = blockAllocator->AllocateBlockMemory(sizeof(BodyFixture));
	BodyFixture* newFixture = new(allocatedMemory) BodyFixture;
	
	newFixture->m_FixtureData = fixtureData;
	newFixture->m_FixtureData.m_Shape = fixtureData.m_Shape->CloneShape(blockAllocator);

	newFixture->m_ParentBody = parentBody;
	newFixture->m_NextFixture = nullptr;

	newFixture->m_FixtureReference.m_BodyFixture = nullptr;
	newFixture->m_FixtureReference.m_FixtureID = INVALID_ID;

	return newFixture;
}



void BodyFixture::DestroyFixture(BlockMemoryAllocator* blockAllocator, BodyFixture* currentFixture)
{
	uint8_t shapeType = currentFixture->m_FixtureData.m_Shape->GetShapeType();
	switch (shapeType)
	{
	case LINE_SHAPE:
	{
		LineShape* lineShape = static_cast<LineShape*>(currentFixture->m_FixtureData.m_Shape);
		lineShape->~LineShape();
		blockAllocator->FreeBlockMemory(lineShape, sizeof(LineShape));
	}
		break;

	case CIRCLE_SHAPE:
	{
		CircleShape* circleShape = static_cast<CircleShape*>(currentFixture->m_FixtureData.m_Shape);
		circleShape->~CircleShape();
		blockAllocator->FreeBlockMemory(circleShape, sizeof(CircleShape));
	}
		break;

	case CAPSULE_SHAPE:
	{
		CapsuleShape* capsuleShape = static_cast<CapsuleShape*>(currentFixture->m_FixtureData.m_Shape);
		capsuleShape->~CapsuleShape();
		blockAllocator->FreeBlockMemory(capsuleShape, sizeof(CapsuleShape));
	}
		break;

	case POLYGON_SHAPE:
	{
		PolygonShape* polygonShape = static_cast<PolygonShape*>(currentFixture->m_FixtureData.m_Shape);
		polygonShape->~PolygonShape();
		blockAllocator->FreeBlockMemory(polygonShape, sizeof(PolygonShape));
	}
		break;

	default:
		break;
	}

	currentFixture->~BodyFixture();
	blockAllocator->FreeBlockMemory(currentFixture, sizeof(BodyFixture));
}



void BodyFixture::AddToBroadPhaseSystem(BroadPhaseSystem* broadPhaseSystem, const Transform2D& fixtureTransform)
{
	m_FixtureReference.m_FixtureAABB = m_FixtureData.m_Shape->GetAABB(fixtureTransform);
	m_FixtureReference.m_FixtureID = broadPhaseSystem->AddFixture(m_FixtureReference.m_FixtureAABB, &m_FixtureReference);
	m_FixtureReference.m_BodyFixture = this;
}



void BodyFixture::RemoveFromBroadPhaseSystem(BroadPhaseSystem* broadPhaseSystem)
{
	broadPhaseSystem->RemoveFixture(m_FixtureReference.m_FixtureID);
	m_FixtureReference.m_FixtureID = INVALID_ID;
}



void BodyFixture::SynchronizeFixture(BroadPhaseSystem* broadPhaseSystem, const Transform2D& firstTransform, const Transform2D& secondTransform)
{
	AABB2D firstAABB = m_FixtureData.m_Shape->GetAABB(firstTransform);
	AABB2D secondAABB = m_FixtureData.m_Shape->GetAABB(secondTransform);

	m_FixtureReference.m_FixtureAABB = AABB2D::GetCombinedAABB(firstAABB, secondAABB);

	Vector2D fixtureDisplacement = secondTransform.m_Position - firstTransform.m_Position;
	broadPhaseSystem->MoveFixture(m_FixtureReference.m_FixtureID, m_FixtureReference.m_FixtureAABB, fixtureDisplacement);
}



CollisionShape* BodyFixture::GetFixtureShape()
{
	return m_FixtureData.m_Shape;
}



const CollisionShape* BodyFixture::GetFixtureShape() const
{
	return m_FixtureData.m_Shape;
}



bool BodyFixture::IsFixtureShapeOfType(uint8_t currentShapeType) const
{
	return m_FixtureData.m_Shape->IsOfType(currentShapeType);
}



void BodyFixture::SetDensity(float density)
{
	m_FixtureData.m_Density = density;
}



float BodyFixture::GetDensity() const
{
	return m_FixtureData.m_Density;
}



void BodyFixture::SetCoefficientOfFriction(float coefficientOfFriction)
{
	m_FixtureData.m_CoefficientOfFriction = coefficientOfFriction;
}



float BodyFixture::GetCoefficientOfFriction() const
{
	return m_FixtureData.m_CoefficientOfFriction;
}



void BodyFixture::SetCoefficientOfRestitution(float coefficientOfRestitution)
{
	m_FixtureData.m_CoefficientOfRestitution = coefficientOfRestitution;
}



float BodyFixture::GetCoefficientOfRestitution() const
{
	return m_FixtureData.m_CoefficientOfRestitution;
}



void BodyFixture::SetOverlapOnly(bool overlapOnly)
{
	if (overlapOnly != m_FixtureData.m_OverlapOnly)
	{
		m_ParentBody->SetBodyAwake(true);
		m_FixtureData.m_OverlapOnly = overlapOnly;
	}
}



bool BodyFixture::IsOverlapOnly() const
{
	return m_FixtureData.m_OverlapOnly;
}



RigidBody* BodyFixture::GetParentBody()
{
	return m_ParentBody;
}



const RigidBody* BodyFixture::GetParentBody() const
{
	return m_ParentBody;
}



void BodyFixture::SetNextFixture(BodyFixture* nextFixture)
{
	m_NextFixture = nextFixture;
}



BodyFixture* BodyFixture::GetNextFixture()
{
	return m_NextFixture;
}



const BodyFixture* BodyFixture::GetNextFixture() const
{
	return m_NextFixture;
}



void BodyFixture::CalculateMassInfo(BodyMassInfo* bodyMassInfo) const
{
	m_FixtureData.m_Shape->CalculateMassInfo(bodyMassInfo, m_FixtureData.m_Density);
}



bool BodyFixture::IsPointInside(const Vector2D& testPoint) const
{
	return m_FixtureData.m_Shape->IsPointInside(m_ParentBody->GetWorldTransform2D(), testPoint);
}



bool BodyFixture::Raycast(const RaycastInput& raycastInput, RaycastResult& raycastResult) const
{
	return m_FixtureData.m_Shape->Raycast(m_ParentBody->GetWorldTransform2D(), raycastInput, raycastResult);
}



AABB2D BodyFixture::GetAABB() const
{
	return m_FixtureReference.m_FixtureAABB;
}



int32_t BodyFixture::GetFixtureID() const
{
	return m_FixtureReference.m_FixtureID;
}



void BodyFixture::RenderFixture(const Transform2D& fixtureTransform) const
{
	RenderFixtureShape(fixtureTransform); // TODO: Maybe more.
}



void BodyFixture::RenderFixtureAABB(const BroadPhaseSystem* broadPhaseSystem) const
{
	const size_t MAXIMUM_AABB_POINTS = 4U;

	Vertex3D aabbVertices[MAXIMUM_AABB_POINTS];
	uint32_t aabbIndices[MAXIMUM_AABB_POINTS] = { 0, 1, 2, 3 };

	Vertex3D aabbVertex;
	aabbVertex.m_Color = RGBA(128, 128, 128, 255);

	AABB2D fixtureAABB = broadPhaseSystem->GetExtendedAABB(GetFixtureID());
	Vector2D aabbMinimums = fixtureAABB.minimums;
	Vector2D aabbMaximums = fixtureAABB.maximums;

	aabbVertex.m_Position = Vector3(aabbMinimums.X, aabbMinimums.Y, 0.0f);
	aabbVertices[0] = aabbVertex;

	aabbVertex.m_Position = Vector3(aabbMaximums.X, aabbMinimums.Y, 0.0f);
	aabbVertices[1] = aabbVertex;

	aabbVertex.m_Position = Vector3(aabbMaximums.X, aabbMaximums.Y, 0.0f);
	aabbVertices[2] = aabbVertex;

	aabbVertex.m_Position = Vector3(aabbMinimums.X, aabbMaximums.Y, 0.0f);
	aabbVertices[3] = aabbVertex;

	Mesh aabbMesh(&aabbVertices[0], &aabbIndices[0], MAXIMUM_AABB_POINTS, MAXIMUM_AABB_POINTS);
	AdvancedRenderer::SingletonInstance()->DrawLineLoopMesh(&aabbMesh, MAXIMUM_AABB_POINTS, MAXIMUM_AABB_POINTS, 0.5f);
}