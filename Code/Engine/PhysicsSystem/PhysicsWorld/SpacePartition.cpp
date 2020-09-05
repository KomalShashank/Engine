#include "Engine/PhysicsSystem/PhysicsWorld/SpacePartition.hpp"
#include "Engine/PhysicsSystem/RigidBody/RigidBody.hpp"
#include "Engine/PhysicsSystem/ContactSolver/ContactSolver.hpp"
#include "Engine/DataStructures/StackMemoryAllocator.hpp"



const float MAXIMUM_ALLOWED_LINEAR_DISPLACEMENT = 2.0f;
const float MAXIMUM_ALLOWED_ANGULAR_DISPLACEMENT = 90.0f;
const float ANGULAR_DISPLACEMENT_CONVERSION = 180.0f / PI_VALUE;



SpacePartition::SpacePartition(StackMemoryAllocator* stackAllocator, ContactCallbacks* contactCallbacks, size_t maximumNumberOfBodies, size_t maximumNumberOfContacts) :
	m_StackAllocator(stackAllocator),
	m_ContactCallbacks(contactCallbacks),
	m_AllBodies(nullptr),
	m_AllContacts(nullptr),
	m_AllPositions(nullptr),
	m_AllRotations(nullptr),
	m_AllLinearVelocities(nullptr),
	m_AllAngularVelocities(nullptr),
	m_NumberOfBodies(0U),
	m_MaximumNumberOfBodies(maximumNumberOfBodies),
	m_NumberOfContacts(0U),
	m_MaximumNumberOfContacts(maximumNumberOfContacts)
{
	m_AllBodies = (RigidBody**)m_StackAllocator->AllocateStackMemory(m_MaximumNumberOfBodies * sizeof(RigidBody*));
	m_AllContacts = (Contact**)m_StackAllocator->AllocateStackMemory(m_MaximumNumberOfContacts * sizeof(Contact*));

	m_AllPositions = (Vector2D*)m_StackAllocator->AllocateStackMemory(m_MaximumNumberOfBodies * sizeof(Vector2D));
	m_AllRotations = (float*)m_StackAllocator->AllocateStackMemory(m_MaximumNumberOfBodies * sizeof(float));

	m_AllLinearVelocities = (Vector2D*)m_StackAllocator->AllocateStackMemory(m_MaximumNumberOfBodies * sizeof(Vector2D));
	m_AllAngularVelocities = (float*)m_StackAllocator->AllocateStackMemory(m_MaximumNumberOfBodies * sizeof(float));
}



SpacePartition::~SpacePartition()
{
	m_StackAllocator->FreeStackMemory(m_AllAngularVelocities);
	m_StackAllocator->FreeStackMemory(m_AllLinearVelocities);

	m_StackAllocator->FreeStackMemory(m_AllRotations);
	m_StackAllocator->FreeStackMemory(m_AllPositions);

	m_StackAllocator->FreeStackMemory(m_AllContacts);
	m_StackAllocator->FreeStackMemory(m_AllBodies);
}



void SpacePartition::AddBody(RigidBody* currentBody)
{
	ASSERT_OR_DIE(m_NumberOfBodies < m_MaximumNumberOfBodies, "Number of bodies exceeded limit.");

	currentBody->SetSpacePartitionID(m_NumberOfBodies);
	m_AllBodies[m_NumberOfBodies] = currentBody;
	++m_NumberOfBodies;
}



void SpacePartition::AddContact(Contact* currentContact)
{
	ASSERT_OR_DIE(m_NumberOfContacts < m_MaximumNumberOfContacts, "Number of contacts exceeded limit.");

	m_AllContacts[m_NumberOfContacts] = currentContact;
	++m_NumberOfContacts;
}



void SpacePartition::ResolvePhysics(float deltaTimeInSeconds, float worldGravity)
{
	Vector2D gravitationalAcceleration = DOWN_DIRECTION * worldGravity;

	for (size_t bodyIndex = 0; bodyIndex < m_NumberOfBodies; ++bodyIndex)
	{
		RigidBody* currentBody = m_AllBodies[bodyIndex];

		Vector2D currentPosition = currentBody->m_SweptShape.m_NextWorldPosition;
		float currentRotation = currentBody->m_SweptShape.m_NextWorldRotation;
		
		Vector2D currentLinearVelocity = currentBody->GetLinearVelocity();
		float currentAngularVelocity = currentBody->GetAngularVelocity();

		currentBody->m_SweptShape.m_PreviousWorldPosition = currentBody->m_SweptShape.m_NextWorldPosition;
		currentBody->m_SweptShape.m_PreviousWorldRotation = currentBody->m_SweptShape.m_NextWorldRotation;

		if (currentBody->IsOfType(DYNAMIC_BODY))
		{
			currentLinearVelocity += (gravitationalAcceleration + (currentBody->GetAccumulatedNetForce() * currentBody->GetBodyMassInfo().m_InverseMass)) * deltaTimeInSeconds;
			currentAngularVelocity += (currentBody->GetAccumulatedNetTorque() * currentBody->GetBodyMassInfo().m_InverseMomentOfInertia) * deltaTimeInSeconds;

			currentLinearVelocity *= 1.0f / (1.0f + (currentBody->GetLinearDamping() * deltaTimeInSeconds));
			currentAngularVelocity *= 1.0f / (1.0f + (currentBody->GetAngularDamping() * deltaTimeInSeconds));
		}

		m_AllPositions[bodyIndex] = currentPosition;
		m_AllRotations[bodyIndex] = currentRotation;
		m_AllLinearVelocities[bodyIndex] = currentLinearVelocity;
		m_AllAngularVelocities[bodyIndex] = currentAngularVelocity;
	}

	ContactSolverData contactSolverData;
	contactSolverData.m_StackAllocator = m_StackAllocator;
	contactSolverData.m_AllPositions = m_AllPositions;
	contactSolverData.m_AllRotations = m_AllRotations;
	contactSolverData.m_AllLinearVelocities = m_AllLinearVelocities;
	contactSolverData.m_AllAngularVelocities = m_AllAngularVelocities;
	contactSolverData.m_AllContacts = m_AllContacts;
	contactSolverData.m_NumberOfContacts = m_NumberOfContacts;

	ContactSolver contactSolver = ContactSolver(contactSolverData);
	contactSolver.InitializePositions(true);
	contactSolver.InitializeVelocities();
	contactSolver.WarmStartSolver();

	for (size_t currentIteration = 0; currentIteration < NUMBER_OF_VELOCITY_ITERATIONS; ++currentIteration)
	{
		contactSolver.ResolveVelocityConstraints();
	}

	contactSolver.SaveContactImpulses();

	for (size_t bodyIndex = 0; bodyIndex < m_NumberOfBodies; ++bodyIndex)
	{
		Vector2D currentPosition = m_AllPositions[bodyIndex];
		float currentRotation = m_AllRotations[bodyIndex];

		Vector2D currentLinearVelocity = m_AllLinearVelocities[bodyIndex];
		float currentAngularVelocity = m_AllAngularVelocities[bodyIndex];

		Vector2D linearDisplacement = currentLinearVelocity * deltaTimeInSeconds;
		if (Vector2D::DotProduct(linearDisplacement, linearDisplacement) > MAXIMUM_ALLOWED_LINEAR_DISPLACEMENT * MAXIMUM_ALLOWED_LINEAR_DISPLACEMENT)
		{
			currentLinearVelocity *= (MAXIMUM_ALLOWED_LINEAR_DISPLACEMENT / linearDisplacement.GetVector2DMagnitude());
			linearDisplacement = currentLinearVelocity * deltaTimeInSeconds;
		}

		float angularDisplacement = currentAngularVelocity * deltaTimeInSeconds;
		angularDisplacement *= ANGULAR_DISPLACEMENT_CONVERSION;
		if (angularDisplacement * angularDisplacement > MAXIMUM_ALLOWED_ANGULAR_DISPLACEMENT * MAXIMUM_ALLOWED_ANGULAR_DISPLACEMENT)
		{
			currentAngularVelocity *= (MAXIMUM_ALLOWED_ANGULAR_DISPLACEMENT / Absolute(angularDisplacement));
			angularDisplacement = currentAngularVelocity * deltaTimeInSeconds;
			angularDisplacement *= ANGULAR_DISPLACEMENT_CONVERSION;
		}

		currentPosition += linearDisplacement;
		currentRotation += angularDisplacement;

		m_AllPositions[bodyIndex] = currentPosition;
		m_AllRotations[bodyIndex] = currentRotation;
		m_AllLinearVelocities[bodyIndex] = currentLinearVelocity;
		m_AllAngularVelocities[bodyIndex] = currentAngularVelocity;
	}

	bool positionalConstraintsSolved = false;
	for (size_t currentIteration = 0; currentIteration < NUMBER_OF_POSITION_ITERATIONS; ++currentIteration)
	{
		bool contactsSolved = contactSolver.ResolvePositionalConstraints();
		if (contactsSolved)
		{
			positionalConstraintsSolved = true;
			break;
		}
	}

	for (size_t bodyIndex = 0; bodyIndex < m_NumberOfBodies; ++bodyIndex)
	{
		RigidBody* currentBody = m_AllBodies[bodyIndex];
		currentBody->m_SweptShape.m_NextWorldPosition = m_AllPositions[bodyIndex];
		currentBody->m_SweptShape.m_NextWorldRotation = m_AllRotations[bodyIndex];
		currentBody->SetLinearVelocity(m_AllLinearVelocities[bodyIndex]);
		currentBody->SetAngularVelocity(m_AllAngularVelocities[bodyIndex]);
		currentBody->UpdateBodyTransform();
	}

	contactSolver.RunContactCallbacks(this);

	float minimumSleepDuration = FLT_MAX;
	for (size_t bodyIndex = 0; bodyIndex < m_NumberOfBodies; ++bodyIndex)
	{
		RigidBody* currentBody = m_AllBodies[bodyIndex];
		if (currentBody->IsOfType(STATIC_BODY))
		{
			continue;
		}

		Vector2D currentLinearVelocity = currentBody->GetLinearVelocity();
		float currentAngularVelocity = currentBody->GetAngularVelocity();

		if ((currentAngularVelocity * currentAngularVelocity > MINIMUM_ANGULAR_VELOCITY_THRESHOLD * MINIMUM_ANGULAR_VELOCITY_THRESHOLD) ||
			(Vector2D::DotProduct(currentLinearVelocity, currentLinearVelocity) > MINIMUM_LINEAR_VELOCITY_THRESHOLD * MINIMUM_LINEAR_VELOCITY_THRESHOLD))
		{
			currentBody->SetSleepDuration(0.0f);
			minimumSleepDuration = 0.0f;
		}
		else
		{
			float sleepDuration = currentBody->GetSleepDuration() + deltaTimeInSeconds;
			currentBody->SetSleepDuration(sleepDuration);
			minimumSleepDuration = GetMinimum(minimumSleepDuration, currentBody->GetSleepDuration());
		}
	}

	if (minimumSleepDuration >= 0.5f && positionalConstraintsSolved)
	{
		for (size_t bodyIndex = 0; bodyIndex < m_NumberOfBodies; ++bodyIndex)
		{
			m_AllBodies[bodyIndex]->SetBodyAwake(false);
		}
	}
}



void SpacePartition::ResolveTimeOfImpactPhysics(float deltaTimeInSeconds, int32_t firstSpacePartitionID, int32_t secondSpacePartitionID)
{
	ASSERT_OR_DIE(firstSpacePartitionID < (int32_t)m_NumberOfBodies, "Limit exceeded.");
	ASSERT_OR_DIE(secondSpacePartitionID < (int32_t)m_NumberOfBodies, "Limit exceeded.");

	for (size_t bodyIndex = 0; bodyIndex < m_NumberOfBodies; ++bodyIndex)
	{
		RigidBody* currentBody = m_AllBodies[bodyIndex];

		m_AllPositions[bodyIndex] = currentBody->m_SweptShape.m_NextWorldPosition;
		m_AllRotations[bodyIndex] = currentBody->m_SweptShape.m_NextWorldRotation;

		m_AllLinearVelocities[bodyIndex] = currentBody->GetLinearVelocity();
		m_AllAngularVelocities[bodyIndex] = currentBody->GetAngularVelocity();
	}

	ContactSolverData contactSolverData;
	contactSolverData.m_StackAllocator = m_StackAllocator;
	contactSolverData.m_AllPositions = m_AllPositions;
	contactSolverData.m_AllRotations = m_AllRotations;
	contactSolverData.m_AllLinearVelocities = m_AllLinearVelocities;
	contactSolverData.m_AllAngularVelocities = m_AllAngularVelocities;
	contactSolverData.m_AllContacts = m_AllContacts;
	contactSolverData.m_NumberOfContacts = m_NumberOfContacts;

	ContactSolver contactSolver = ContactSolver(contactSolverData);
	contactSolver.InitializePositions(false);

	for (size_t currentIteration = 0; currentIteration < NUMBER_OF_TIME_OF_IMPACT_POSITION_ITERATIONS; ++currentIteration)
	{
		bool contactsSolved = contactSolver.ResolveTimeOfImpactPositionalConstraints(firstSpacePartitionID, secondSpacePartitionID);
		if (contactsSolved)
		{
			break;
		}
	}

	m_AllBodies[firstSpacePartitionID]->m_SweptShape.m_PreviousWorldPosition = m_AllPositions[firstSpacePartitionID];
	m_AllBodies[firstSpacePartitionID]->m_SweptShape.m_PreviousWorldRotation = m_AllRotations[firstSpacePartitionID];
	m_AllBodies[secondSpacePartitionID]->m_SweptShape.m_PreviousWorldPosition = m_AllPositions[secondSpacePartitionID];
	m_AllBodies[secondSpacePartitionID]->m_SweptShape.m_PreviousWorldRotation = m_AllRotations[secondSpacePartitionID];

	contactSolver.InitializeVelocities();
	
	for (size_t currentIteration = 0; currentIteration < NUMBER_OF_VELOCITY_ITERATIONS; ++currentIteration)
	{
		contactSolver.ResolveVelocityConstraints();
	}

	for (size_t bodyIndex = 0; bodyIndex < m_NumberOfBodies; ++bodyIndex)
	{
		Vector2D currentPosition = m_AllPositions[bodyIndex];
		float currentRotation = m_AllRotations[bodyIndex];

		Vector2D currentLinearVelocity = m_AllLinearVelocities[bodyIndex];
		float currentAngularVelocity = m_AllAngularVelocities[bodyIndex];

		Vector2D linearDisplacement = currentLinearVelocity * deltaTimeInSeconds;
		if (Vector2D::DotProduct(linearDisplacement, linearDisplacement) > MAXIMUM_ALLOWED_LINEAR_DISPLACEMENT * MAXIMUM_ALLOWED_LINEAR_DISPLACEMENT)
		{
			currentLinearVelocity *= (MAXIMUM_ALLOWED_LINEAR_DISPLACEMENT / linearDisplacement.GetVector2DMagnitude());
			linearDisplacement = currentLinearVelocity * deltaTimeInSeconds;
		}

		float angularDisplacement = currentAngularVelocity * deltaTimeInSeconds;
		angularDisplacement *= ANGULAR_DISPLACEMENT_CONVERSION;
		if (angularDisplacement * angularDisplacement > MAXIMUM_ALLOWED_ANGULAR_DISPLACEMENT * MAXIMUM_ALLOWED_ANGULAR_DISPLACEMENT)
		{
			currentAngularVelocity *= (MAXIMUM_ALLOWED_ANGULAR_DISPLACEMENT / Absolute(angularDisplacement));
			angularDisplacement = currentAngularVelocity * deltaTimeInSeconds;
			angularDisplacement *= ANGULAR_DISPLACEMENT_CONVERSION;
		}

		currentPosition += linearDisplacement;
		currentRotation += angularDisplacement;

		m_AllPositions[bodyIndex] = currentPosition;
		m_AllRotations[bodyIndex] = currentRotation;
		m_AllLinearVelocities[bodyIndex] = currentLinearVelocity;
		m_AllAngularVelocities[bodyIndex] = currentAngularVelocity;

		RigidBody* currentBody = m_AllBodies[bodyIndex];
		currentBody->m_SweptShape.m_NextWorldPosition = currentPosition;
		currentBody->m_SweptShape.m_NextWorldRotation = currentRotation;
		currentBody->SetLinearVelocity(currentLinearVelocity);
		currentBody->SetAngularVelocity(currentAngularVelocity);
		currentBody->UpdateBodyTransform();
	}

	contactSolver.RunContactCallbacks(this);
}



void SpacePartition::ResetPartition()
{
	m_NumberOfBodies = 0U;
	m_NumberOfContacts = 0U;
}