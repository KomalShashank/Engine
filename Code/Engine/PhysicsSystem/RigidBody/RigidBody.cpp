#include "Engine/PhysicsSystem/RigidBody/RigidBody.hpp"
#include "Engine/PhysicsSystem/RigidBody/BodyFixture.hpp"
#include "Engine/PhysicsSystem/ContactSolver/Contacts/Contact.hpp"
#include "Engine/PhysicsSystem/PhysicsWorld/PhysicsWorld.hpp"
#include "Engine/DataStructures/BlockMemoryAllocator.hpp"



enum
{
	AWAKE_FLAG = 0x0001,
	ACTIVE_FLAG = 0x0002,
	NO_ROTATION_FLAG = 0x0004,
	USING_CCD_FLAG = 0x0008,
	PART_OF_SPACE_PARTITION_FLAG = 0x0010
};



RigidBodyData::RigidBodyData() :
	m_LinearVelocity(Vector2D::ZERO),
	m_AngularVelocity(0.0f),
	m_LinearDamping(0.0f),
	m_AngularDamping(0.0f),
	m_BodyType(INVALID_BODY_TYPE),
	m_BodyFlags(AWAKE_FLAG | ACTIVE_FLAG)
{

}



void RigidBodyData::SetAwake(bool awake)
{
	if (awake)
	{
		m_BodyFlags |= AWAKE_FLAG;
	}
	else
	{
		m_BodyFlags &= ~AWAKE_FLAG;
	}
}



void RigidBodyData::SetActive(bool active)
{
	if (active)
	{
		m_BodyFlags |= ACTIVE_FLAG;
	}
	else
	{
		m_BodyFlags &= ~ACTIVE_FLAG;
	}
}



void RigidBodyData::SetHasNoRotation(bool hasNoRotation)
{
	if (hasNoRotation)
	{
		m_BodyFlags |= NO_ROTATION_FLAG;
	}
	else
	{
		m_BodyFlags &= ~NO_ROTATION_FLAG;
	}
}



void RigidBodyData::SetUsingCCD(bool usingCCD)
{
	if (usingCCD)
	{
		m_BodyFlags |= USING_CCD_FLAG;
	}
	else
	{
		m_BodyFlags &= ~USING_CCD_FLAG;
	}
}



RigidBody::RigidBody() :
m_ParentWorld(nullptr),
m_PreviousBody(nullptr),
m_NextBody(nullptr),
m_BodyFixtures(nullptr),
m_NumberOfFixtures(0U),
m_AllContactNodes(nullptr),
m_AccumulatedNetForce(Vector2D::ZERO),
m_AccumulatedNetTorque(0.0f),
m_SleepDuration(0.0f),
m_SpacePartitionID(-1)
{

}



RigidBody::~RigidBody()
{

}



void RigidBody::RenderAllFixtures() const
{
	if (m_NumberOfFixtures == 0U)
	{
		return;
	}

	for (const BodyFixture* currentFixture = GetFixturesList(); currentFixture != nullptr; currentFixture = currentFixture->GetNextFixture())
	{
		currentFixture->RenderFixture(m_BodyData.m_WorldTransform);
	}
}



void RigidBody::RenderAllFixtureAABBs(const BroadPhaseSystem* broadPhaseSystem) const
{
	if (m_NumberOfFixtures == 0U)
	{
		return;
	}

	for (const BodyFixture* currentFixture = GetFixturesList(); currentFixture != nullptr; currentFixture = currentFixture->GetNextFixture())
	{
		currentFixture->RenderFixtureAABB(broadPhaseSystem);
	}
}



RigidBody* RigidBody::CreateBody(BlockMemoryAllocator* blockAllocator, const RigidBodyData& bodyData, PhysicsWorld* parentWorld)
{
	void* allocatedMemory = blockAllocator->AllocateBlockMemory(sizeof(RigidBody));
	RigidBody* newBody = new(allocatedMemory) RigidBody;

	newBody->m_BodyData = bodyData;
	newBody->m_ParentWorld = parentWorld;

	newBody->m_SweptShape.m_PreviousWorldPosition = bodyData.m_WorldTransform.m_Position;
	newBody->m_SweptShape.m_PreviousWorldRotation = bodyData.m_WorldTransform.m_Rotation.GetAngle();
	newBody->m_SweptShape.m_NextWorldPosition = bodyData.m_WorldTransform.m_Position;
	newBody->m_SweptShape.m_NextWorldRotation = bodyData.m_WorldTransform.m_Rotation.GetAngle();

	return newBody;
}



void RigidBody::DestroyBody(BlockMemoryAllocator* blockAllocator, RigidBody* currentBody)
{
	currentBody->~RigidBody();
	blockAllocator->FreeBlockMemory(currentBody, sizeof(RigidBody));
}



BodyFixture* RigidBody::CreateBodyFixture(const BodyFixtureData* fixtureData)
{
	if (m_ParentWorld->IsWorldLocked())
	{
		return nullptr;
	}

	BodyFixture* newFixture = BodyFixture::CreateFixture(&m_ParentWorld->m_BlockAllocator, *fixtureData, this);

	if (IsBodyActive())
	{
		BroadPhaseSystem* broadPhaseSystem = &m_ParentWorld->m_ContactHandler.m_BroadPhaseSystem;
		newFixture->AddToBroadPhaseSystem(broadPhaseSystem, m_BodyData.m_WorldTransform);
	}

	newFixture->SetNextFixture(m_BodyFixtures);
	m_BodyFixtures = newFixture;
	++m_NumberOfFixtures;

	if (newFixture->GetDensity() > 0.0f)
	{
		RecalculateBodyMassInfo();
	}

	m_ParentWorld->SetNewFixturesCreated(true);

	return newFixture;
}



void RigidBody::DestroyBodyFixture(BodyFixture* currentFixture)
{
	if (currentFixture == nullptr)
	{
		return;
	}

	if (m_ParentWorld->IsWorldLocked())
	{
		return;
	}

	ASSERT_OR_DIE(m_NumberOfFixtures > 0, "No fixtures attached to this body.");
	ASSERT_OR_DIE(currentFixture->GetParentBody() == this, "Fixture does not belong to this body.");

	// TODO: Do something here.

	for (ContactNode* currentContactNode = GetContactNodeList(); currentContactNode != nullptr;)
	{
		Contact* destroyableContact = currentContactNode->m_Contact;
		currentContactNode = currentContactNode->m_NextNode;

		BodyFixture* firstFixture = destroyableContact->GetFirstFixture();
		BodyFixture* secondFixture = destroyableContact->GetSecondFixture();

		if (currentFixture == firstFixture || currentFixture == secondFixture)
		{
			m_ParentWorld->m_ContactHandler.DestroyExistingContact(destroyableContact);
		}
	}

	if (IsBodyActive())
	{
		BroadPhaseSystem* broadPhaseSystem = &m_ParentWorld->m_ContactHandler.m_BroadPhaseSystem;
		currentFixture->RemoveFromBroadPhaseSystem(broadPhaseSystem);
	}

	BodyFixture::DestroyFixture(&m_ParentWorld->m_BlockAllocator, currentFixture);
	--m_NumberOfFixtures;

	RecalculateBodyMassInfo();
}



void RigidBody::SynchronizeAllBodyFixtures()
{
	Transform2D sweptTransform;
	sweptTransform.m_Rotation.SetRotator2D(m_SweptShape.m_PreviousWorldRotation);
	sweptTransform.m_Position = m_SweptShape.m_PreviousWorldPosition - Multiply(sweptTransform.m_Rotation, m_SweptShape.m_LocalCenter);

	BroadPhaseSystem* broadPhaseSystem = &m_ParentWorld->m_ContactHandler.m_BroadPhaseSystem;
	for (BodyFixture* currentFixture = GetFixturesList(); currentFixture != nullptr; currentFixture = currentFixture->GetNextFixture())
	{
		currentFixture->SynchronizeFixture(broadPhaseSystem, sweptTransform, m_BodyData.m_WorldTransform);
	}
}



void RigidBody::UpdateBodyTransform()
{
	m_BodyData.m_WorldTransform.m_Rotation.SetRotator2D(m_SweptShape.m_NextWorldRotation);
	m_BodyData.m_WorldTransform.m_Position = m_SweptShape.m_NextWorldPosition - Multiply(m_BodyData.m_WorldTransform.m_Rotation, m_SweptShape.m_LocalCenter);
}



void RigidBody::AdvanceBodyToDelta(float delta)
{
	m_SweptShape.AdvanceToDelta(delta);
	m_SweptShape.m_NextWorldPosition = m_SweptShape.m_PreviousWorldPosition;
	m_SweptShape.m_NextWorldRotation = m_SweptShape.m_PreviousWorldRotation;

	UpdateBodyTransform();
}



void RigidBody::ApplyForceToBody(const Vector2D& appliedForce)
{
	if (!IsOfType(DYNAMIC_BODY))
	{
		return;
	}

	if (!IsBodyAwake())
	{
		SetBodyAwake(true);
	}

	if (IsBodyAwake())
	{
		m_AccumulatedNetForce += appliedForce;
	}
}



void RigidBody::ApplyForceToBodyAtPosition(const Vector2D& appliedForce, const Vector2D& appliedPosition)
{
	if (!IsOfType(DYNAMIC_BODY))
	{
		return;
	}

	if (!IsBodyAwake())
	{
		SetBodyAwake(true);
	}

	if (IsBodyAwake())
	{
		m_AccumulatedNetForce += appliedForce;
		m_AccumulatedNetTorque += Vector2D::CrossProduct(appliedPosition - m_SweptShape.m_NextWorldPosition, appliedForce);
	}
}



void RigidBody::ApplyTorqueToBody(float appliedTorque)
{
	if (!IsOfType(DYNAMIC_BODY))
	{
		return;
	}

	if (!IsBodyAwake())
	{
		SetBodyAwake(true);
	}

	if (IsBodyAwake())
	{
		m_AccumulatedNetTorque += appliedTorque;
	}
}



void RigidBody::ResetAllForces()
{
	m_AccumulatedNetForce = Vector2D::ZERO;
	m_AccumulatedNetTorque = 0.0f;
}



void RigidBody::ApplyLinearImpulseToBody(const Vector2D& appliedLinearImpulse)
{
	if (!IsOfType(DYNAMIC_BODY))
	{
		return;
	}

	if (!IsBodyAwake())
	{
		SetBodyAwake(true);
	}

	if (IsBodyAwake())
	{
		m_BodyData.m_LinearVelocity += appliedLinearImpulse * m_BodyMassInfo.m_InverseMass;
	}
}



void RigidBody::ApplyLinearImpulseToBodyAtPosition(const Vector2D& appliedLinearImpulse, const Vector2D& appliedPosition)
{
	if (!IsOfType(DYNAMIC_BODY))
	{
		return;
	}

	if (!IsBodyAwake())
	{
		SetBodyAwake(true);
	}

	if (IsBodyAwake())
	{
		m_BodyData.m_LinearVelocity += appliedLinearImpulse * m_BodyMassInfo.m_InverseMass;
		m_BodyData.m_AngularVelocity += Vector2D::CrossProduct(appliedPosition - m_SweptShape.m_NextWorldPosition, appliedLinearImpulse) * m_BodyMassInfo.m_InverseMomentOfInertia;
	}
}



void RigidBody::ApplyAngularImpulse(float appliedAngularImpulse)
{
	if (!IsOfType(DYNAMIC_BODY))
	{
		return;
	}

	if (!IsBodyAwake())
	{
		SetBodyAwake(true);
	}

	if (IsBodyAwake())
	{
		m_BodyData.m_AngularVelocity += appliedAngularImpulse * m_BodyMassInfo.m_InverseMomentOfInertia;
	}
}



Vector2D RigidBody::GetWorldCenter() const
{
	return m_SweptShape.m_NextWorldPosition;
}



Vector2D RigidBody::GetLocalCenter() const
{
	return m_SweptShape.m_LocalCenter;
}



void RigidBody::SetWorldTransform2D(const Vector2D& transformPosition, float transformRotation)
{
	if (m_ParentWorld->IsWorldLocked())
	{
		return;
	}

	m_BodyData.m_WorldTransform.m_Position = transformPosition;
	m_BodyData.m_WorldTransform.m_Rotation.SetRotator2D(transformRotation);

	m_SweptShape.m_NextWorldPosition = Multiply(m_BodyData.m_WorldTransform, m_SweptShape.m_LocalCenter);
	m_SweptShape.m_NextWorldRotation = transformRotation;

	m_SweptShape.m_PreviousWorldPosition = m_SweptShape.m_NextWorldPosition;
	m_SweptShape.m_PreviousWorldRotation = m_SweptShape.m_NextWorldRotation;

	BroadPhaseSystem* broadPhaseSystem = &m_ParentWorld->m_ContactHandler.m_BroadPhaseSystem;
	for (BodyFixture* currentFixture = GetFixturesList(); currentFixture != nullptr; currentFixture = currentFixture->GetNextFixture())
	{
		currentFixture->SynchronizeFixture(broadPhaseSystem, m_BodyData.m_WorldTransform, m_BodyData.m_WorldTransform);
	}
}



Transform2D RigidBody::GetWorldTransform2D() const
{
	return m_BodyData.m_WorldTransform;
}



Vector2D RigidBody::GetWorldPosition() const
{
	return m_BodyData.m_WorldTransform.m_Position;
}



float RigidBody::GetWorldRotation() const
{
	return m_BodyData.m_WorldTransform.m_Rotation.GetAngle();
}



void RigidBody::SetLinearVelocity(const Vector2D& linearVelocity)
{
	if (IsOfType(STATIC_BODY))
	{
		return;
	}

	if (Vector2D::DotProduct(linearVelocity, linearVelocity) > 0.0f)
	{
		SetBodyAwake(true);
	}

	m_BodyData.m_LinearVelocity = linearVelocity;
}



Vector2D RigidBody::GetLinearVelocity() const
{
	return m_BodyData.m_LinearVelocity;
}



void RigidBody::SetAngularVelocity(float angularVelocity)
{
	if (IsOfType(STATIC_BODY))
	{
		return;
	}

	if (angularVelocity * angularVelocity > 0.0f)
	{
		SetBodyAwake(true);
	}

	m_BodyData.m_AngularVelocity = angularVelocity;
}



float RigidBody::GetAngularVelocity() const
{
	return m_BodyData.m_AngularVelocity;
}



void RigidBody::SetLinearDamping(float linearDamping)
{
	m_BodyData.m_LinearDamping = linearDamping;
}



float RigidBody::GetLinearDamping() const
{
	return m_BodyData.m_LinearDamping;
}



void RigidBody::SetAngularDamping(float angularDamping)
{
	m_BodyData.m_AngularDamping = angularDamping;
}



float RigidBody::GetAngularDamping() const
{
	return m_BodyData.m_AngularDamping;
}



void RigidBody::SetBodyAwake(bool awake)
{
	if (awake)
	{
		if (!IsBodyAwake())
		{
			m_BodyData.m_BodyFlags |= AWAKE_FLAG;
			m_SleepDuration = 0.0f;
		}
	}
	else
	{
		m_BodyData.m_BodyFlags &= ~AWAKE_FLAG;
		m_BodyData.m_AngularVelocity = 0.0f;
		m_BodyData.m_LinearVelocity = Vector2D::ZERO;
		m_AccumulatedNetForce = Vector2D::ZERO;
		m_AccumulatedNetTorque = 0.0f;
		m_SleepDuration = 0.0f;
	}
}



bool RigidBody::IsBodyAwake() const
{
	return ((m_BodyData.m_BodyFlags & AWAKE_FLAG) == AWAKE_FLAG);
}



void RigidBody::SetBodyActive(bool active)
{
	ASSERT_OR_DIE(!m_ParentWorld->IsWorldLocked(), "World is locked.");

	if (IsBodyActive() == active)
	{
		return;
	}

	if (active)
	{
		m_BodyData.m_BodyFlags |= ACTIVE_FLAG;
	}
	else
	{
		m_BodyData.m_BodyFlags &= ~ACTIVE_FLAG;
	}

	BroadPhaseSystem* broadPhaseSystem = &m_ParentWorld->m_ContactHandler.m_BroadPhaseSystem;

	if (active)
	{
		for (BodyFixture* currentFixture = GetFixturesList(); currentFixture != nullptr; currentFixture = currentFixture->GetNextFixture())
		{
			currentFixture->AddToBroadPhaseSystem(broadPhaseSystem, m_BodyData.m_WorldTransform);
		}
	}
	else
	{
		for (BodyFixture* currentFixture = GetFixturesList(); currentFixture != nullptr; currentFixture = currentFixture->GetNextFixture())
		{
			currentFixture->RemoveFromBroadPhaseSystem(broadPhaseSystem);
		}

		for (ContactNode* currentContactNode = GetContactNodeList(); currentContactNode != nullptr;)
		{
			ContactNode* destroyableContactNode = currentContactNode;
			currentContactNode = currentContactNode->m_NextNode;
			m_ParentWorld->m_ContactHandler.DestroyExistingContact(destroyableContactNode->m_Contact);
		}

		SetContactNodeList(nullptr);
	}
}



bool RigidBody::IsBodyActive() const
{
	return ((m_BodyData.m_BodyFlags & ACTIVE_FLAG) == ACTIVE_FLAG);
}



void RigidBody::SetNoRotation(bool noRotation)
{
	bool currentRotationState = HasNoRotation();
	if (currentRotationState == noRotation)
	{
		return;
	}

	if (noRotation)
	{
		m_BodyData.m_BodyFlags |= NO_ROTATION_FLAG;
	}
	else
	{
		m_BodyData.m_BodyFlags &= ~NO_ROTATION_FLAG;
	}

	m_BodyData.m_AngularVelocity = 0.0f;
	RecalculateBodyMassInfo();
}



bool RigidBody::HasNoRotation() const
{
	return ((m_BodyData.m_BodyFlags & NO_ROTATION_FLAG) == NO_ROTATION_FLAG);
}



void RigidBody::UseCCD(bool usingCCD)
{
	if (usingCCD)
	{
		m_BodyData.m_BodyFlags |= USING_CCD_FLAG;
	}
	else
	{
		m_BodyData.m_BodyFlags &= ~USING_CCD_FLAG;
	}
}



bool RigidBody::IsUsingCCD() const
{
	return ((m_BodyData.m_BodyFlags & USING_CCD_FLAG) == USING_CCD_FLAG);
}



void RigidBody::SetBodyType(uint8_t currentBodyType)
{
	if (m_ParentWorld->IsWorldLocked())
	{
		return;
	}

	if (IsOfType(currentBodyType))
	{
		return;
	}

	m_BodyData.m_BodyType = currentBodyType;
	RecalculateBodyMassInfo();

	if (IsOfType(STATIC_BODY))
	{
		m_BodyData.m_LinearVelocity = Vector2D::ZERO;
		m_BodyData.m_AngularVelocity = 0.0f;
		m_SweptShape.m_PreviousWorldPosition = m_SweptShape.m_NextWorldPosition;
		m_SweptShape.m_PreviousWorldRotation = m_SweptShape.m_NextWorldRotation;
		SynchronizeAllBodyFixtures();
	}

	SetBodyAwake(true);
	ResetAllForces();

	for (ContactNode* currentContactNode = GetContactNodeList(); currentContactNode != nullptr;)
	{
		ContactNode* destroyableContactNode = currentContactNode;
		currentContactNode = currentContactNode->m_NextNode;
		m_ParentWorld->m_ContactHandler.DestroyExistingContact(destroyableContactNode->m_Contact);
	}

	SetContactNodeList(nullptr);

	BroadPhaseSystem* broadPhaseSystem = &m_ParentWorld->m_ContactHandler.m_BroadPhaseSystem;
	for (BodyFixture* currentFixture = GetFixturesList(); currentFixture != nullptr; currentFixture = currentFixture->GetNextFixture())
	{
		broadPhaseSystem->TriggerFixture(currentFixture->GetFixtureID());
	}
}



bool RigidBody::IsOfType(uint8_t currentBodyType)
{
	return (m_BodyData.m_BodyType == currentBodyType);
}



BodyMassInfo RigidBody::GetBodyMassInfo() const
{
	return m_BodyMassInfo;
}



void RigidBody::RecalculateBodyMassInfo()
{
	m_BodyMassInfo = BodyMassInfo();

	if (IsOfType(STATIC_BODY) || IsOfType(KINEMATIC_BODY))
	{
		m_SweptShape.m_PreviousWorldPosition = m_BodyData.m_WorldTransform.m_Position;
		m_SweptShape.m_NextWorldPosition = m_BodyData.m_WorldTransform.m_Position;
		m_SweptShape.m_PreviousWorldRotation = m_SweptShape.m_NextWorldRotation;

		return;
	}

	Vector2D localCenter = Vector2D::ZERO;
	for (BodyFixture* currentFixture = GetFixturesList(); currentFixture != nullptr; currentFixture = currentFixture->GetNextFixture())
	{
		if (currentFixture->GetDensity() == 0.0f)
		{
			continue;
		}

		BodyMassInfo bodyMassInfo;
		currentFixture->CalculateMassInfo(&bodyMassInfo);
		m_BodyMassInfo.m_Mass += bodyMassInfo.m_Mass;
		localCenter += bodyMassInfo.m_CenterOfMass * bodyMassInfo.m_Mass;
		m_BodyMassInfo.m_MomentOfInertia += bodyMassInfo.m_MomentOfInertia;
	}

	if (m_BodyMassInfo.m_Mass > 0.0f)
	{
		m_BodyMassInfo.m_InverseMass = 1.0f / m_BodyMassInfo.m_Mass;
		localCenter *= m_BodyMassInfo.m_InverseMass;
	}
	else
	{
		m_BodyMassInfo.m_Mass = 1.0f;
		m_BodyMassInfo.m_InverseMass = 1.0f;
	}

	if (m_BodyMassInfo.m_MomentOfInertia > 0.0f && !HasNoRotation())
	{
		m_BodyMassInfo.m_MomentOfInertia -= m_BodyMassInfo.m_Mass * Vector2D::DotProduct(localCenter, localCenter);
		ASSERT_OR_DIE(m_BodyMassInfo.m_MomentOfInertia > 0.0f, "Moment of Inertia is negative.");
		m_BodyMassInfo.m_InverseMomentOfInertia = 1.0f / m_BodyMassInfo.m_MomentOfInertia;
	}
	else
	{
		m_BodyMassInfo.m_MomentOfInertia = 0.0f;
		m_BodyMassInfo.m_InverseMomentOfInertia = 0.0f;
	}

	Vector2D previousCenter = m_SweptShape.m_NextWorldPosition;
	m_SweptShape.m_LocalCenter = localCenter;
	m_SweptShape.m_NextWorldPosition = Multiply(m_BodyData.m_WorldTransform, m_SweptShape.m_LocalCenter);
	m_SweptShape.m_PreviousWorldPosition = m_SweptShape.m_NextWorldPosition;

	m_BodyData.m_LinearVelocity += Vector2D::CrossProduct(m_BodyData.m_AngularVelocity, m_SweptShape.m_NextWorldPosition - previousCenter);
}



float RigidBody::GetMass() const
{
	return m_BodyMassInfo.m_Mass;
}



float RigidBody::GetInverseMass() const
{
	float mass = GetMass();
	if (mass > 0.0f)
	{
		return (1.0f / mass);
	}

	return 1.0f;
}



float RigidBody::GetLocalMomentOfInertia() const
{
	return (m_BodyMassInfo.m_MomentOfInertia + (m_BodyMassInfo.m_Mass * Vector2D::DotProduct(m_SweptShape.m_LocalCenter, m_SweptShape.m_LocalCenter)));
}



float RigidBody::GetInverseLocalMomentOfInertia() const
{
	float localMomentOfInertia = GetLocalMomentOfInertia();
	if (localMomentOfInertia > 0.0f)
	{
		return (1.0f / localMomentOfInertia);
	}

	return 0.0f;
}



PhysicsWorld* RigidBody::GetParentWorld()
{
	return m_ParentWorld;
}



const PhysicsWorld* RigidBody::GetParentWorld() const
{
	return m_ParentWorld;
}



void RigidBody::SetPreviousBody(RigidBody* previousBody)
{
	m_PreviousBody = previousBody;
}



RigidBody* RigidBody::GetPreviousBody()
{
	return m_PreviousBody;
}



const RigidBody* RigidBody::GetPreviousBody() const
{
	return m_PreviousBody;
}



void RigidBody::SetNextBody(RigidBody* nextBody)
{
	m_NextBody = nextBody;
}



RigidBody* RigidBody::GetNextBody()
{
	return m_NextBody;
}



const RigidBody* RigidBody::GetNextBody() const
{
	return m_NextBody;
}



BodyFixture* RigidBody::GetFixturesList()
{
	return m_BodyFixtures;
}



const BodyFixture* RigidBody::GetFixturesList() const
{
	return m_BodyFixtures;
}



size_t RigidBody::GetNumberOfFixtures() const
{
	return m_NumberOfFixtures;
}



void RigidBody::SetContactNodeList(ContactNode* contactNodes)
{
	m_AllContactNodes = contactNodes;
}



ContactNode* RigidBody::GetContactNodeList()
{
	return m_AllContactNodes;
}



const ContactNode* RigidBody::GetContactNodeList() const
{
	return m_AllContactNodes;
}



Vector2D RigidBody::GetAccumulatedNetForce() const
{
	return m_AccumulatedNetForce;
}



float RigidBody::GetAccumulatedNetTorque() const
{
	return m_AccumulatedNetTorque;
}



void RigidBody::SetSleepDuration(float sleepDuration)
{
	m_SleepDuration = sleepDuration;
}



float RigidBody::GetSleepDuration() const
{
	return m_SleepDuration;
}



void RigidBody::SetSpacePartitionID(int32_t spacePartitionID)
{
	m_SpacePartitionID = spacePartitionID;
}



int32_t RigidBody::GetSpacePartitionID() const
{
	return m_SpacePartitionID;
}



void RigidBody::SetBodyPartOfSpacePartition(bool partOfSpacePartition)
{
	if (partOfSpacePartition)
	{
		m_BodyData.m_BodyFlags |= PART_OF_SPACE_PARTITION_FLAG;
	}
	else
	{
		m_BodyData.m_BodyFlags &= ~PART_OF_SPACE_PARTITION_FLAG;
	}
}



bool RigidBody::IsBodyPartOfSpacePartition() const
{
	return ((m_BodyData.m_BodyFlags & PART_OF_SPACE_PARTITION_FLAG) == PART_OF_SPACE_PARTITION_FLAG);
}



bool RigidBody::CanBodiesCollide(RigidBody* firstBody, RigidBody* secondBody)
{
	if (firstBody->IsOfType(DYNAMIC_BODY) || secondBody->IsOfType(DYNAMIC_BODY))
	{
		return true;
	}

	return false;
}



void RigidBody::RenderBody() const
{
	RenderAllFixtures(); // TODO: Maybe more.
}



void RigidBody::RenderBodyAABBs(const BroadPhaseSystem* broadPhaseSystem) const
{
	if (!IsBodyActive())
	{
		return;
	}
	
	RenderAllFixtureAABBs(broadPhaseSystem);
}