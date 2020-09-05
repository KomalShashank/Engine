#include "Engine/PhysicsSystem/PhysicsWorld/PhysicsWorld.hpp"
#include "Engine/PhysicsSystem/PhysicsWorld/SpacePartition.hpp"
#include "Engine/PhysicsSystem/RigidBody/RigidBody.hpp"
#include "Engine/PhysicsSystem/RigidBody/BodyFixture.hpp"
#include "Engine/PhysicsSystem/ContactSolver/Contacts/Contact.hpp"
#include "Engine/DebugTools/ProfilerSystem/ProfilerSystem.hpp"
#include "Engine/Input/InputSystem.hpp"



PhysicsWorld* g_PhysicsWorld = nullptr;
const size_t MAXIMUM_NUMBER_OF_TIME_OF_IMPACT_CONTACTS = 32U;



PhysicsWorld::PhysicsWorld(float deltaTimeConstant, float worldGravity) :
	m_RigidBodies(nullptr),
	m_NumberOfBodies(0U),
	m_DeltaTimeConstant(deltaTimeConstant),
	m_WorldGravity(worldGravity),
	m_WorldLocked(false),
	m_NewFixturesCreated(false),
	m_ShowAABBs(false)
{
	m_ContactHandler.m_BlockAllocator = &m_BlockAllocator;
	m_InverseDeltaTimeConstant = (m_DeltaTimeConstant > 0.0f) ? 1.0f / m_DeltaTimeConstant : 0.0f;
}



PhysicsWorld::~PhysicsWorld()
{

}



void PhysicsWorld::ResolvePhysics()
{
	SpacePartition spacePartition = SpacePartition(&m_StackAllocator, m_ContactHandler.m_ContactCallbacks, m_NumberOfBodies, m_ContactHandler.m_NumberOfContacts);

	for (RigidBody* currentBody = GetBodiesList(); currentBody != nullptr; currentBody = currentBody->GetNextBody())
	{
		currentBody->SetBodyPartOfSpacePartition(false);
	}

	for (Contact* currentContact = m_ContactHandler.m_AllContacts; currentContact != nullptr; currentContact = currentContact->GetNextContact())
	{
		currentContact->SetContactPartOfSpacePartition(false);
	}

	size_t maximumNumberOfSpacePartitionBodies = m_NumberOfBodies;
	RigidBody** spacePartitionBodies = (RigidBody**)m_StackAllocator.AllocateStackMemory(maximumNumberOfSpacePartitionBodies * sizeof(RigidBody*));
	for (RigidBody* currentBody = GetBodiesList(); currentBody != nullptr; currentBody = currentBody->GetNextBody())
	{
		if (currentBody->IsBodyPartOfSpacePartition())
		{
			continue;
		}

		if (!currentBody->IsBodyActive() || !currentBody->IsBodyAwake())
		{
			continue;
		}

		if (currentBody->IsOfType(STATIC_BODY))
		{
			continue;
		}

		spacePartition.ResetPartition();

		size_t numberOfSpacePartitionBodies = 0U;
		spacePartitionBodies[numberOfSpacePartitionBodies] = currentBody;
		++numberOfSpacePartitionBodies;
		currentBody->SetBodyPartOfSpacePartition(true);

		while (numberOfSpacePartitionBodies > 0)
		{
			--numberOfSpacePartitionBodies;
			RigidBody* currentSpacePartitionBody = spacePartitionBodies[numberOfSpacePartitionBodies];

			ASSERT_OR_DIE(currentSpacePartitionBody->IsBodyActive(), "Body is inactive.");

			spacePartition.AddBody(currentSpacePartitionBody);
			currentSpacePartitionBody->SetBodyAwake(true);

			if (currentSpacePartitionBody->IsOfType(STATIC_BODY))
			{
				continue;
			}

			for (ContactNode* currentContactNode = currentSpacePartitionBody->GetContactNodeList(); currentContactNode != nullptr; currentContactNode = currentContactNode->m_NextNode)
			{
				Contact* currentContact = currentContactNode->m_Contact;

				if (currentContact->IsContactPartOfSpacePartition())
				{
					continue;
				}

				if (!currentContact->IsContactEnabled() || !currentContact->AreFixturesInContact())
				{
					continue;
				}

				if (currentContact->GetFirstFixture()->IsOverlapOnly() || currentContact->GetSecondFixture()->IsOverlapOnly())
				{
					continue;
				}

				spacePartition.AddContact(currentContact);
				currentContact->SetContactPartOfSpacePartition(true);

				RigidBody* otherBody = currentContactNode->m_OtherBody;
				if (otherBody->IsBodyPartOfSpacePartition())
				{
					continue;
				}

				ASSERT_OR_DIE(numberOfSpacePartitionBodies < maximumNumberOfSpacePartitionBodies, "Limit exceeded.");
				spacePartitionBodies[numberOfSpacePartitionBodies] = otherBody;
				++numberOfSpacePartitionBodies;
				otherBody->SetBodyPartOfSpacePartition(true);
			}
		}

		spacePartition.ResolvePhysics(m_DeltaTimeConstant, m_WorldGravity);

		for (size_t bodyIndex = 0; bodyIndex < spacePartition.m_NumberOfBodies; ++bodyIndex)
		{
			RigidBody* bodyAtIndex = spacePartition.m_AllBodies[bodyIndex];
			if (bodyAtIndex->IsOfType(STATIC_BODY))
			{
				bodyAtIndex->SetBodyPartOfSpacePartition(false);
			}
		}
	}

	m_StackAllocator.FreeStackMemory(spacePartitionBodies);

	for (RigidBody* currentBody = GetBodiesList(); currentBody != nullptr; currentBody = currentBody->GetNextBody())
	{
		if (!currentBody->IsBodyPartOfSpacePartition())
		{
			continue;
		}

		if (currentBody->IsOfType(STATIC_BODY))
		{
			continue;
		}

		currentBody->SynchronizeAllBodyFixtures();
	}

	ProfilerSystem::SingletonInstance()->PushProfileSample("ContactCreation");
	{
		m_ContactHandler.CreateNewContacts();
	}
	ProfilerSystem::SingletonInstance()->PopProfileSample();
}



void PhysicsWorld::ResolveTimeOfImpactPhysics()
{
	SpacePartition spacePartition = SpacePartition(&m_StackAllocator, m_ContactHandler.m_ContactCallbacks, 2U * MAXIMUM_NUMBER_OF_TIME_OF_IMPACT_CONTACTS, MAXIMUM_NUMBER_OF_TIME_OF_IMPACT_CONTACTS);

	for (RigidBody* currentBody = GetBodiesList(); currentBody != nullptr; currentBody = currentBody->GetNextBody())
	{
		currentBody->SetBodyPartOfSpacePartition(false);
		currentBody->m_SweptShape.m_PreviousDelta = 0.0f;
	}

	for (Contact* currentContact = m_ContactHandler.m_AllContacts; currentContact != nullptr; currentContact = currentContact->GetNextContact())
	{
		currentContact->SetContactPartOfSpacePartition(false);
		currentContact->SetValidTimeOfImpact(false);
		currentContact->SetNumberOfTimesOfImpact(0U);
		currentContact->SetTimeOfImpactDuration(1.0f);
	}

	while (true)
	{
		Contact* firstContact = nullptr;
		float firstDelta = 1.0f;

		for (Contact* currentContact = m_ContactHandler.m_AllContacts; currentContact != nullptr; currentContact = currentContact->GetNextContact())
		{
			if (!currentContact->IsContactEnabled())
			{
				continue;
			}

			if (currentContact->GetNumberOfTimesOfImpact() > 8U)
			{
				continue;
			}

			float delta = 1.0f;
			if (currentContact->DoesHaveAValidTimeOfImpact())
			{
				delta = currentContact->GetTimeOfImpactDuration();
			}
			else
			{
				BodyFixture* firstFixture = currentContact->GetFirstFixture();
				BodyFixture* secondFixture = currentContact->GetSecondFixture();

				if (firstFixture->IsOverlapOnly() || secondFixture->IsOverlapOnly())
				{
					continue;
				}

				RigidBody* firstBody = firstFixture->GetParentBody();
				RigidBody* secondBody = secondFixture->GetParentBody();

				ASSERT_OR_DIE(firstBody->IsOfType(DYNAMIC_BODY) || secondBody->IsOfType(DYNAMIC_BODY), "Neither body is dynamic.");

				bool firstBodyActive = firstBody->IsBodyAwake() && !firstBody->IsOfType(STATIC_BODY);
				bool secondBodyActive = secondBody->IsBodyAwake() && !secondBody->IsOfType(STATIC_BODY);

				if (!firstBodyActive && !secondBodyActive)
				{
					continue;
				}

				bool firstBodyNotDynamic = firstBody->IsUsingCCD() || !firstBody->IsOfType(DYNAMIC_BODY);
				bool secondBodyNotDynamic = secondBody->IsUsingCCD() || !secondBody->IsOfType(DYNAMIC_BODY);

				if (!firstBodyNotDynamic && !secondBodyNotDynamic)
				{
					continue;
				}

				float previousDelta = firstBody->m_SweptShape.m_PreviousDelta;
				if (firstBody->m_SweptShape.m_PreviousDelta < secondBody->m_SweptShape.m_PreviousDelta)
				{
					previousDelta = secondBody->m_SweptShape.m_PreviousDelta;
					firstBody->m_SweptShape.AdvanceToDelta(previousDelta);
				}
				else if (secondBody->m_SweptShape.m_PreviousDelta < firstBody->m_SweptShape.m_PreviousDelta)
				{
					previousDelta = firstBody->m_SweptShape.m_PreviousDelta;
					secondBody->m_SweptShape.AdvanceToDelta(previousDelta);
				}

				ASSERT_OR_DIE(previousDelta < 1.0f, "Out of bounds.");

				ShapeReference firstShapeReference = ShapeReference(firstFixture->GetFixtureShape());
				ShapeReference secondShapeReference = ShapeReference(secondFixture->GetFixtureShape());

				uint8_t timeOfImpactState;
				float timeOfImpact = ComputeTimeOfImpact(1.0f, timeOfImpactState, &firstShapeReference, &firstBody->m_SweptShape, &secondShapeReference, &secondBody->m_SweptShape);
				if (timeOfImpactState == TOUCHING_STATE)
				{
					delta = GetMinimum(previousDelta + ((1.0f - previousDelta) * timeOfImpact), 1.0f);
				}
				else
				{
					delta = 1.0f;
				}

				currentContact->SetTimeOfImpactDuration(delta);
				currentContact->SetValidTimeOfImpact(true);
			}

			if (delta < firstDelta)
			{
				firstContact = currentContact;
				firstDelta = delta;
			}
		}

		if (firstContact == nullptr || firstDelta > (1.0f - (10.0f * FLT_EPSILON)))
		{
			break;
		}

		RigidBody* firstBody = firstContact->GetFirstFixture()->GetParentBody();
		RigidBody* secondBody = firstContact->GetSecondFixture()->GetParentBody();

		BodySweptShape firstBodySweptShape = firstBody->m_SweptShape;
		BodySweptShape secondBodySweptShape = secondBody->m_SweptShape;

		firstBody->AdvanceBodyToDelta(firstDelta);
		secondBody->AdvanceBodyToDelta(firstDelta);

		firstContact->UpdateContact(m_ContactHandler.m_ContactCallbacks);
		firstContact->SetValidTimeOfImpact(false);

		size_t numberOfTimesOfImpact = firstContact->GetNumberOfTimesOfImpact() + 1U;
		firstContact->SetNumberOfTimesOfImpact(numberOfTimesOfImpact);

		if (!firstContact->IsContactEnabled() || !firstContact->AreFixturesInContact())
		{
			firstContact->SetContactEnabled(false);
			firstBody->m_SweptShape = firstBodySweptShape;
			secondBody->m_SweptShape = secondBodySweptShape;
			firstBody->UpdateBodyTransform();
			secondBody->UpdateBodyTransform();
			continue;
		}

		firstBody->SetBodyAwake(true);
		secondBody->SetBodyAwake(true);

		spacePartition.ResetPartition();
		spacePartition.AddBody(firstBody);
		spacePartition.AddBody(secondBody);
		spacePartition.AddContact(firstContact);

		firstBody->SetBodyPartOfSpacePartition(true);
		secondBody->SetBodyPartOfSpacePartition(true);
		firstContact->SetContactPartOfSpacePartition(true);

		RigidBody* bodyPair[2U] = { firstBody, secondBody };
		for (size_t bodyIndex = 0; bodyIndex < 2U; ++bodyIndex)
		{
			RigidBody* currentBody = bodyPair[bodyIndex];
			if (currentBody->IsOfType(DYNAMIC_BODY))
			{
				for (ContactNode* currentContactNode = currentBody->GetContactNodeList(); currentContactNode != nullptr; currentContactNode = currentContactNode->m_NextNode)
				{
					if (spacePartition.m_NumberOfBodies >= spacePartition.m_MaximumNumberOfBodies)
					{
						break;
					}

					if (spacePartition.m_NumberOfContacts >= spacePartition.m_MaximumNumberOfContacts)
					{
						break;
					}

					Contact* currentContact = currentContactNode->m_Contact;

					if (currentContact->IsContactPartOfSpacePartition())
					{
						continue;
					}

					RigidBody* otherBody = currentContactNode->m_OtherBody;
					if (otherBody->IsOfType(DYNAMIC_BODY) && !currentBody->IsUsingCCD() && !otherBody->IsUsingCCD())
					{
						continue;
					}

					if (currentContact->GetFirstFixture()->IsOverlapOnly() || currentContact->GetSecondFixture()->IsOverlapOnly())
					{
						continue;
					}

					BodySweptShape otherBodySweptShape = otherBody->m_SweptShape;
					if (!otherBody->IsBodyPartOfSpacePartition())
					{
						otherBody->AdvanceBodyToDelta(firstDelta);
					}

					currentContact->UpdateContact(m_ContactHandler.m_ContactCallbacks);

					if (!currentContact->IsContactEnabled())
					{
						otherBody->m_SweptShape = otherBodySweptShape;
						otherBody->UpdateBodyTransform();
						continue;
					}

					if (!currentContact->AreFixturesInContact())
					{
						otherBody->m_SweptShape = otherBodySweptShape;
						otherBody->UpdateBodyTransform();
						continue;
					}

					spacePartition.AddContact(currentContact);
					currentContact->SetContactPartOfSpacePartition(true);

					if (otherBody->IsBodyPartOfSpacePartition())
					{
						continue;
					}

					otherBody->SetBodyPartOfSpacePartition(true);

					if (!otherBody->IsOfType(STATIC_BODY))
					{
						otherBody->SetBodyAwake(true);
					}

					spacePartition.AddBody(otherBody);
				}
			}
		}

		float subDeltaTime = (1.0f - firstDelta) * m_DeltaTimeConstant;
		spacePartition.ResolveTimeOfImpactPhysics(subDeltaTime, firstBody->GetSpacePartitionID(), secondBody->GetSpacePartitionID());

		for (size_t bodyIndex = 0; bodyIndex < spacePartition.m_NumberOfBodies; ++bodyIndex)
		{
			RigidBody* currentBody = spacePartition.m_AllBodies[bodyIndex];
			currentBody->SetBodyPartOfSpacePartition(false);

			if (!currentBody->IsOfType(DYNAMIC_BODY))
			{
				continue;
			}

			currentBody->SynchronizeAllBodyFixtures();

			for (ContactNode* currentContactNode = currentBody->GetContactNodeList(); currentContactNode != nullptr; currentContactNode = currentContactNode->m_NextNode)
			{
				currentContactNode->m_Contact->SetContactPartOfSpacePartition(false);
				currentContactNode->m_Contact->SetValidTimeOfImpact(false);
			}
		}

		m_ContactHandler.CreateNewContacts();
	}
}



void PhysicsWorld::ResetForcesOnAllBodies()
{
	for (RigidBody* currentBody = GetBodiesList(); currentBody != nullptr; currentBody = currentBody->GetNextBody())
	{
		currentBody->ResetAllForces();
	}
}



void PhysicsWorld::ToggleAABBs()
{
	if (InputSystem::SingletonInstance()->KeyWasJustPressed('B'))
	{
		m_ShowAABBs = !m_ShowAABBs;
	}
}



void PhysicsWorld::RenderAllBodies() const
{
	if (m_NumberOfBodies == 0U)
	{
		return;
	}

	for (const RigidBody* currentBody = GetBodiesList(); currentBody != nullptr; currentBody = currentBody->GetNextBody())
	{
		currentBody->RenderBody();
	}
}



void PhysicsWorld::RenderAllBodyAABBs(const BroadPhaseSystem* broadPhaseSystem) const
{
	if (m_NumberOfBodies == 0U)
	{
		return;
	}

	for (const RigidBody* currentBody = GetBodiesList(); currentBody != nullptr; currentBody = currentBody->GetNextBody())
	{
		currentBody->RenderBodyAABBs(broadPhaseSystem);
	}
}



void PhysicsWorld::InitializePhysicsWorld()
{
	if (g_PhysicsWorld == nullptr)
	{
		g_PhysicsWorld = new PhysicsWorld(0.0f, 0.0f);
	}
}



void PhysicsWorld::InitializePhysicsWorld(float deltaTimeConstant, float worldGravity)
{
	if (g_PhysicsWorld == nullptr)
	{
		g_PhysicsWorld = new PhysicsWorld(deltaTimeConstant, worldGravity);
	}
}



void PhysicsWorld::UninitializePhysicsWorld()
{
	if (g_PhysicsWorld != nullptr)
	{
		delete g_PhysicsWorld;
		g_PhysicsWorld = nullptr;
	}
}



PhysicsWorld* PhysicsWorld::SingletonInstance()
{
	ASSERT_OR_DIE(g_PhysicsWorld != nullptr, "Physics World instance does not exist.");
	
	return g_PhysicsWorld;
}



void PhysicsWorld::SimulateWorld()
{
	ToggleAABBs();
	
	if (HaveNewFixturesBeenCreated())
	{
		m_ContactHandler.CreateNewContacts();
		SetNewFixturesCreated(false);
	}

	SetWorldLocked(true);
	
	ProfilerSystem::SingletonInstance()->PushProfileSample("ContactUpdate");
	{
		m_ContactHandler.HandleCollision();
	}
	ProfilerSystem::SingletonInstance()->PopProfileSample();

	if (m_DeltaTimeConstant > 0.0f)
	{
		ProfilerSystem::SingletonInstance()->PushProfileSample("ResolvePhysics");
		{
			ResolvePhysics();
		}
		ProfilerSystem::SingletonInstance()->PopProfileSample();

		ProfilerSystem::SingletonInstance()->PushProfileSample("ResolveTOIPhysics");
		{
			ResolveTimeOfImpactPhysics();
		}
		ProfilerSystem::SingletonInstance()->PopProfileSample();
	}

	ResetForcesOnAllBodies();
	SetWorldLocked(false);
}



void PhysicsWorld::RenderWorld() const
{
	RenderAllBodies(); // TODO: Maybe more.

	if (m_ShowAABBs)
	{
		const BroadPhaseSystem* broadPhaseSystem = &m_ContactHandler.m_BroadPhaseSystem;
		RenderAllBodyAABBs(broadPhaseSystem);
	}
}



void PhysicsWorld::SetContactCallbacks(ContactCallbacks* contactCallbacks)
{
	m_ContactHandler.m_ContactCallbacks = contactCallbacks;
}



RigidBody* PhysicsWorld::CreateRigidBody(const RigidBodyData* bodyData)
{
	if (IsWorldLocked())
	{
		return nullptr;
	}

	RigidBody* newBody = RigidBody::CreateBody(&m_BlockAllocator, *bodyData, this);

	newBody->SetNextBody(m_RigidBodies);
	if (m_RigidBodies != nullptr)
	{
		m_RigidBodies->SetPreviousBody(newBody);
	}

	m_RigidBodies = newBody;
	++m_NumberOfBodies;

	return newBody;
}



void PhysicsWorld::DestroyRigidBody(RigidBody* currentBody)
{
	if (currentBody == nullptr)
	{
		return;
	}
	
	if (IsWorldLocked())
	{
		return;
	}

	ASSERT_OR_DIE(m_NumberOfBodies > 0, "No rigid bodies exist in the world.");
	
	for (ContactNode* currentContactNode = currentBody->GetContactNodeList(); currentContactNode != nullptr;)
	{
		ContactNode* destroyableContactNode = currentContactNode;
		currentContactNode = currentContactNode->m_NextNode;
		m_ContactHandler.DestroyExistingContact(destroyableContactNode->m_Contact);
	}

	for (BodyFixture* currentFixture = currentBody->GetFixturesList(); currentFixture != nullptr;)
	{
		BodyFixture* destroyableFixture = currentFixture;
		currentFixture = currentFixture->GetNextFixture();

		destroyableFixture->RemoveFromBroadPhaseSystem(&m_ContactHandler.m_BroadPhaseSystem);
		BodyFixture::DestroyFixture(&m_BlockAllocator, destroyableFixture);
	}

	RigidBody* previousBody = currentBody->GetPreviousBody();
	RigidBody* nextBody = currentBody->GetNextBody();

	if (previousBody != nullptr)
	{
		previousBody->SetNextBody(nextBody);
	}

	if (nextBody != nullptr)
	{
		nextBody->SetPreviousBody(previousBody);
	}

	if (currentBody == m_RigidBodies)
	{
		m_RigidBodies = nextBody;
	}

	--m_NumberOfBodies;
	RigidBody::DestroyBody(&m_BlockAllocator, currentBody);
}



RigidBody* PhysicsWorld::GetBodiesList()
{
	return m_RigidBodies;
}



const RigidBody* PhysicsWorld::GetBodiesList() const
{
	return m_RigidBodies;
}



size_t PhysicsWorld::GetNumberOfBodies() const
{
	return m_NumberOfBodies;
}



void PhysicsWorld::SetWorldGravity(float worldGravity)
{
	worldGravity = m_WorldGravity;
}



float PhysicsWorld::GetWorldGravity() const
{
	return m_WorldGravity;
}



void PhysicsWorld::SetWorldLocked(bool locked)
{
	m_WorldLocked = locked;
}



bool PhysicsWorld::IsWorldLocked() const
{
	return m_WorldLocked;
}



void PhysicsWorld::SetNewFixturesCreated(bool created)
{
	m_NewFixturesCreated = created;
}



bool PhysicsWorld::HaveNewFixturesBeenCreated() const
{
	return m_NewFixturesCreated;
}