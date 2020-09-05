#include "Engine/PhysicsSystem/ContactSolver/ContactSolver.hpp"
#include "Engine/PhysicsSystem/ContactSolver/Contacts/Contact.hpp"
#include "Engine/PhysicsSystem/PhysicsWorld/SpacePartition.hpp"
#include "Engine/PhysicsSystem/RigidBody/BodyFixture.hpp"
#include "Engine/PhysicsSystem/RigidBody/RigidBody.hpp"
#include "Engine/DataStructures/StackMemoryAllocator.hpp"



ContactSolver::ContactSolver(const ContactSolverData& solverData) :
	m_SolverData(solverData)
{
	m_AllPositionalConstraints = (PositionalConstraint*)m_SolverData.m_StackAllocator->AllocateStackMemory(m_SolverData.m_NumberOfContacts * sizeof(PositionalConstraint));
	m_AllVelocityConstraints = (VelocityConstraint*)m_SolverData.m_StackAllocator->AllocateStackMemory(m_SolverData.m_NumberOfContacts * sizeof(VelocityConstraint));
}



ContactSolver::~ContactSolver()
{
	m_SolverData.m_StackAllocator->FreeStackMemory(m_AllVelocityConstraints);
	m_SolverData.m_StackAllocator->FreeStackMemory(m_AllPositionalConstraints);
}



void ContactSolver::InitializePositions(bool usingWarmStarting)
{
	for (size_t contactIndex = 0; contactIndex < m_SolverData.m_NumberOfContacts; ++contactIndex)
	{
		Contact* currentContact = m_SolverData.m_AllContacts[contactIndex];

		float firstBoundingRadius = currentContact->GetFirstFixture()->GetFixtureShape()->GetBoundingRadius();
		float secondBoundingRadius = currentContact->GetSecondFixture()->GetFixtureShape()->GetBoundingRadius();

		RigidBody* firstBody = currentContact->GetFirstFixture()->GetParentBody();
		RigidBody* secondBody = currentContact->GetSecondFixture()->GetParentBody();

		LocalContactCluster* localCluster = currentContact->GetLocalContactCluster();
		size_t numberOfContactPoints = localCluster->m_NumberOfContactPoints;
		ASSERT_OR_DIE(numberOfContactPoints > 0, "No contact points exist.");

		VelocityConstraint* currentVelocityConstraint = m_AllVelocityConstraints + contactIndex;

		currentVelocityConstraint->m_NormalMass = Matrix2x2::ZeroMatrix2x2();
		currentVelocityConstraint->m_InverseNormalMass = Matrix2x2::ZeroMatrix2x2();
		currentVelocityConstraint->m_FirstBodyInverseMass = firstBody->GetBodyMassInfo().m_InverseMass;
		currentVelocityConstraint->m_SecondBodyInverseMass = secondBody->GetBodyMassInfo().m_InverseMass;
		currentVelocityConstraint->m_FirstBodyInverseMomentOfInertia = firstBody->GetBodyMassInfo().m_InverseMomentOfInertia;
		currentVelocityConstraint->m_SecondBodyInverseMomentOfInertia = secondBody->GetBodyMassInfo().m_InverseMomentOfInertia;
		currentVelocityConstraint->m_CoefficientOfFriction = currentContact->GetCoefficientOfFriction();
		currentVelocityConstraint->m_CoefficientOfRestitution = currentContact->GetCoefficientOfRestitution();
		currentVelocityConstraint->m_TangentialSpeed = currentContact->GetTangentialSpeed();
		currentVelocityConstraint->m_FirstBodySpacePartitionID = firstBody->GetSpacePartitionID();
		currentVelocityConstraint->m_SecondBodySpacePartitionID = secondBody->GetSpacePartitionID();
		currentVelocityConstraint->m_ContactIndex = contactIndex;
		currentVelocityConstraint->m_NumberOfContactPoints = numberOfContactPoints;

		PositionalConstraint* currentPositionalConstraint = m_AllPositionalConstraints + contactIndex;

		currentPositionalConstraint->m_LocalReferencePoint = localCluster->m_LocalReferencePoint;
		currentPositionalConstraint->m_LocalReferenceNormal = localCluster->m_LocalReferenceNormal;
		currentPositionalConstraint->m_FirstBodyLocalCenter = firstBody->m_SweptShape.m_LocalCenter;
		currentPositionalConstraint->m_SecondBodyLocalCenter = secondBody->m_SweptShape.m_LocalCenter;
		currentPositionalConstraint->m_FirstBodyInverseMass = firstBody->GetBodyMassInfo().m_InverseMass;
		currentPositionalConstraint->m_SecondBodyInverseMass = secondBody->GetBodyMassInfo().m_InverseMass;
		currentPositionalConstraint->m_FirstBodyInverseMomentOfInertia = firstBody->GetBodyMassInfo().m_InverseMomentOfInertia;
		currentPositionalConstraint->m_SecondBodyInverseMomentOfInertia = secondBody->GetBodyMassInfo().m_InverseMomentOfInertia;
		currentPositionalConstraint->m_FirstBoundingRadius = firstBoundingRadius;
		currentPositionalConstraint->m_SecondBoundingRadius = secondBoundingRadius;
		currentPositionalConstraint->m_FirstBodySpacePartitionID = firstBody->GetSpacePartitionID();
		currentPositionalConstraint->m_SecondBodySpacePartitionID = secondBody->GetSpacePartitionID();
		currentPositionalConstraint->m_NumberOfContactPoints = numberOfContactPoints;
		currentPositionalConstraint->m_ClusterType = localCluster->m_ClusterType;

		for (size_t constraintIndex = 0; constraintIndex < numberOfContactPoints; ++constraintIndex)
		{
			ContactPoint* currentContactPoint = localCluster->m_AllContactPoints + constraintIndex;
			ConstraintPoint* currentConstraintPoint = currentVelocityConstraint->m_ConstraintPoints + constraintIndex;

			currentConstraintPoint->m_FirstPoint = Vector2D::ZERO;
			currentConstraintPoint->m_SecondPoint = Vector2D::ZERO;

			currentConstraintPoint->m_NormalImpulse = (usingWarmStarting) ? currentContactPoint->m_PushBackImpulse : 0.0f;
			currentConstraintPoint->m_TangentImpulse = (usingWarmStarting) ? currentContactPoint->m_FrictionImpulse : 0.0f;

			currentConstraintPoint->m_NormalMass = 0.0f;
			currentConstraintPoint->m_TangentMass = 0.0f;
			currentConstraintPoint->m_VelocityBias = 0.0f;

			currentPositionalConstraint->m_ContactPoints[constraintIndex] = currentContactPoint->m_PointOfContact;
		}
	}
}



void ContactSolver::InitializeVelocities()
{
	for (size_t constraintIndex = 0; constraintIndex < m_SolverData.m_NumberOfContacts; ++constraintIndex)
	{
		VelocityConstraint* currentVelocityConstraint = m_AllVelocityConstraints + constraintIndex;
		PositionalConstraint* currentPositionalConstraint = m_AllPositionalConstraints + constraintIndex;

		LocalContactCluster* localCluster = m_SolverData.m_AllContacts[currentVelocityConstraint->m_ContactIndex]->GetLocalContactCluster();

		int32_t firstBodyID = currentVelocityConstraint->m_FirstBodySpacePartitionID;
		int32_t secondBodyID = currentVelocityConstraint->m_SecondBodySpacePartitionID;

		float iMass_1 = currentVelocityConstraint->m_FirstBodyInverseMass;
		float iMass_2 = currentVelocityConstraint->m_SecondBodyInverseMass;
		float iMomentOfInertia_1 = currentVelocityConstraint->m_FirstBodyInverseMomentOfInertia;
		float iMomentOfInertia_2 = currentVelocityConstraint->m_SecondBodyInverseMomentOfInertia;

		Vector2D localCenter_1 = currentPositionalConstraint->m_FirstBodyLocalCenter;
		Vector2D localCenter_2 = currentPositionalConstraint->m_SecondBodyLocalCenter;

		Vector2D position_1 = m_SolverData.m_AllPositions[firstBodyID];
		float rotation_1 = m_SolverData.m_AllRotations[firstBodyID];
		Vector2D linearVelocity_1 = m_SolverData.m_AllLinearVelocities[firstBodyID];
		float angularVelocity_1 = m_SolverData.m_AllAngularVelocities[firstBodyID];

		Vector2D position_2 = m_SolverData.m_AllPositions[secondBodyID];
		float rotation_2 = m_SolverData.m_AllRotations[secondBodyID];
		Vector2D linearVelocity_2 = m_SolverData.m_AllLinearVelocities[secondBodyID];
		float angularVelocity_2 = m_SolverData.m_AllAngularVelocities[secondBodyID];

		ASSERT_OR_DIE(localCluster->m_NumberOfContactPoints > 0, "No contact points exist.");

		Transform2D firstTransform;
		firstTransform.m_Rotation.SetRotator2D(rotation_1);
		firstTransform.m_Position = position_1 - Multiply(firstTransform.m_Rotation, localCenter_1);

		Transform2D secondTransform;
		secondTransform.m_Rotation.SetRotator2D(rotation_2);
		secondTransform.m_Position = position_2 - Multiply(secondTransform.m_Rotation, localCenter_2);

		GlobalContactCluster globalCluster;
		globalCluster.GenerateGlobalCluster(localCluster, firstTransform, currentPositionalConstraint->m_FirstBoundingRadius, secondTransform, currentPositionalConstraint->m_SecondBoundingRadius);
		currentVelocityConstraint->m_Normal = globalCluster.m_GlobalReferenceNormal;

		for (size_t contactIndex = 0; contactIndex < currentVelocityConstraint->m_NumberOfContactPoints; ++contactIndex)
		{
			ConstraintPoint* currentConstraintPoint = currentVelocityConstraint->m_ConstraintPoints + contactIndex;
			currentConstraintPoint->m_FirstPoint = globalCluster.m_AllGlobalContactPoints[contactIndex] - position_1;
			currentConstraintPoint->m_SecondPoint = globalCluster.m_AllGlobalContactPoints[contactIndex] - position_2;

			float cpResult_1;
			float cpResult_2;

			cpResult_1 = Vector2D::CrossProduct(currentConstraintPoint->m_FirstPoint, currentVelocityConstraint->m_Normal);
			cpResult_2 = Vector2D::CrossProduct(currentConstraintPoint->m_SecondPoint, currentVelocityConstraint->m_Normal);

			float normalValue = iMass_1 + iMass_2 + (iMomentOfInertia_1 * cpResult_1 * cpResult_1) + (iMomentOfInertia_2 * cpResult_2 * cpResult_2);
			currentConstraintPoint->m_NormalMass = (normalValue > 0.0f) ? 1.0f / normalValue : 0.0f;

			Vector2D tangent = Vector2D::CrossProduct(currentVelocityConstraint->m_Normal, 1.0f);

			cpResult_1 = Vector2D::CrossProduct(currentConstraintPoint->m_FirstPoint, tangent);
			cpResult_2 = Vector2D::CrossProduct(currentConstraintPoint->m_SecondPoint, tangent);

			float tangentValue = iMass_1 + iMass_2 + (iMomentOfInertia_1 * cpResult_1 * cpResult_1) + (iMomentOfInertia_2 * cpResult_2 * cpResult_2);
			currentConstraintPoint->m_TangentMass = (tangentValue > 0.0f) ? 1.0f / tangentValue : 0.0f;

			currentConstraintPoint->m_VelocityBias = 0.0f;
			Vector2D firstVelocity = linearVelocity_1 + Vector2D::CrossProduct(angularVelocity_1, currentConstraintPoint->m_FirstPoint);
			Vector2D secondVelocity = linearVelocity_2 + Vector2D::CrossProduct(angularVelocity_2, currentConstraintPoint->m_SecondPoint);

			float relativeVelocity = Vector2D::DotProduct(currentVelocityConstraint->m_Normal, secondVelocity - firstVelocity);
			if (relativeVelocity < -1.0f)
			{
				currentConstraintPoint->m_VelocityBias = -currentVelocityConstraint->m_CoefficientOfRestitution * relativeVelocity;
			}
		}

		if (currentVelocityConstraint->m_NumberOfContactPoints == 2U)
		{
			ConstraintPoint* firstConstraintPoint = currentVelocityConstraint->m_ConstraintPoints + 0;
			ConstraintPoint* secondConstraintPoint = currentVelocityConstraint->m_ConstraintPoints + 1;

			float cpResult_11 = Vector2D::CrossProduct(firstConstraintPoint->m_FirstPoint, currentVelocityConstraint->m_Normal);
			float cpResult_12 = Vector2D::CrossProduct(firstConstraintPoint->m_SecondPoint, currentVelocityConstraint->m_Normal);
			float cpResult_21 = Vector2D::CrossProduct(secondConstraintPoint->m_FirstPoint, currentVelocityConstraint->m_Normal);
			float cpResult_22 = Vector2D::CrossProduct(secondConstraintPoint->m_SecondPoint, currentVelocityConstraint->m_Normal);

			float matrixValue_11 = iMass_1 + iMass_2 + (iMomentOfInertia_1 * cpResult_11 * cpResult_11) + (iMomentOfInertia_2 * cpResult_12 * cpResult_12);
			float matrixValue_12 = iMass_1 + iMass_2 + (iMomentOfInertia_1 * cpResult_11 * cpResult_21) + (iMomentOfInertia_2 * cpResult_12 * cpResult_22);
			float matrixValue_22 = iMass_1 + iMass_2 + (iMomentOfInertia_1 * cpResult_21 * cpResult_21) + (iMomentOfInertia_2 * cpResult_22 * cpResult_22);

			if (matrixValue_11 * matrixValue_11 < 1000.0f * (matrixValue_11 * matrixValue_22 - matrixValue_12 * matrixValue_12))
			{
				float matrixValues[4] = { matrixValue_11, matrixValue_12, matrixValue_12, matrixValue_22 };
				currentVelocityConstraint->m_InverseNormalMass = Matrix2x2(matrixValues);
				currentVelocityConstraint->m_InverseNormalMass.HasInverse(currentVelocityConstraint->m_NormalMass);
			}
			else
			{
				currentVelocityConstraint->m_NumberOfContactPoints = 1U;
			}
		}
	}
}



void ContactSolver::WarmStartSolver()
{
	for (size_t constraintIndex = 0; constraintIndex < m_SolverData.m_NumberOfContacts; ++constraintIndex)
	{
		VelocityConstraint* currentVelocityConstraint = m_AllVelocityConstraints + constraintIndex;

		int32_t firstBodyID = currentVelocityConstraint->m_FirstBodySpacePartitionID;
		int32_t secondBodyID = currentVelocityConstraint->m_SecondBodySpacePartitionID;

		float iMass_1 = currentVelocityConstraint->m_FirstBodyInverseMass;
		float iMass_2 = currentVelocityConstraint->m_SecondBodyInverseMass;
		float iMomentOfInertia_1 = currentVelocityConstraint->m_FirstBodyInverseMomentOfInertia;
		float iMomentOfInertia_2 = currentVelocityConstraint->m_SecondBodyInverseMomentOfInertia;

		size_t numberOfContactPoints = currentVelocityConstraint->m_NumberOfContactPoints;

		Vector2D linearVelocity_1 = m_SolverData.m_AllLinearVelocities[firstBodyID];
		float angularVelocity_1 = m_SolverData.m_AllAngularVelocities[firstBodyID];
		Vector2D linearVelocity_2 = m_SolverData.m_AllLinearVelocities[secondBodyID];
		float angularVelocity_2 = m_SolverData.m_AllAngularVelocities[secondBodyID];

		Vector2D normalVector = currentVelocityConstraint->m_Normal;
		Vector2D tangentVector = Vector2D::CrossProduct(normalVector, 1.0f);

		for (size_t contactIndex = 0; contactIndex < numberOfContactPoints; ++contactIndex)
		{
			ConstraintPoint* currentConstraintPoint = currentVelocityConstraint->m_ConstraintPoints + contactIndex;
			Vector2D combinedImpulse = (normalVector * currentConstraintPoint->m_NormalImpulse) + (tangentVector * currentConstraintPoint->m_TangentImpulse);

			linearVelocity_1 -= combinedImpulse * iMass_1;
			angularVelocity_1 -= Vector2D::CrossProduct(currentConstraintPoint->m_FirstPoint, combinedImpulse) * iMomentOfInertia_1;

			linearVelocity_2 += combinedImpulse * iMass_2;
			angularVelocity_2 += Vector2D::CrossProduct(currentConstraintPoint->m_SecondPoint, combinedImpulse) * iMomentOfInertia_2;
		}

		m_SolverData.m_AllLinearVelocities[firstBodyID] = linearVelocity_1;
		m_SolverData.m_AllAngularVelocities[firstBodyID] = angularVelocity_1;
		m_SolverData.m_AllLinearVelocities[secondBodyID] = linearVelocity_2;
		m_SolverData.m_AllAngularVelocities[secondBodyID] = angularVelocity_2;
	}
}



void ContactSolver::SaveContactImpulses()
{
	for (size_t constraintIndex = 0; constraintIndex < m_SolverData.m_NumberOfContacts; ++constraintIndex)
	{
		VelocityConstraint* currentVelocityConstraint = m_AllVelocityConstraints + constraintIndex;
		LocalContactCluster* localCluster = m_SolverData.m_AllContacts[currentVelocityConstraint->m_ContactIndex]->GetLocalContactCluster();

		size_t numberOfContactPoints = currentVelocityConstraint->m_NumberOfContactPoints;

		for (size_t contactIndex = 0; contactIndex < numberOfContactPoints; ++contactIndex)
		{
			localCluster->m_AllContactPoints[contactIndex].m_PushBackImpulse = currentVelocityConstraint->m_ConstraintPoints[contactIndex].m_NormalImpulse;
			localCluster->m_AllContactPoints[contactIndex].m_FrictionImpulse = currentVelocityConstraint->m_ConstraintPoints[contactIndex].m_TangentImpulse;
		}
	}
}



void ContactSolver::RunContactCallbacks(SpacePartition* spacePartition)
{
	if (spacePartition->m_ContactCallbacks == nullptr)
	{
		return;
	}

	for (size_t constraintIndex = 0; constraintIndex < spacePartition->m_NumberOfContacts; ++constraintIndex)
	{
		Contact* currentContact = spacePartition->m_AllContacts[constraintIndex];
		VelocityConstraint* currentVelocityConstraint = m_AllVelocityConstraints + constraintIndex;
		size_t numberOfContactPoints = currentVelocityConstraint->m_NumberOfContactPoints;

		float allNormalImpulses[MAXIMUM_NUMBER_OF_CONTACT_POINTS];
		float allTangentImpulses[MAXIMUM_NUMBER_OF_CONTACT_POINTS];
		
		for (size_t contactIndex = 0; contactIndex < numberOfContactPoints; ++contactIndex)
		{
			allNormalImpulses[contactIndex] = currentVelocityConstraint->m_ConstraintPoints[contactIndex].m_NormalImpulse;
			allTangentImpulses[contactIndex] = currentVelocityConstraint->m_ConstraintPoints[contactIndex].m_TangentImpulse;
		}

		spacePartition->m_ContactCallbacks->OnSimulationEnd(currentContact, allNormalImpulses, allTangentImpulses, numberOfContactPoints);
	}
}



bool ContactSolver::ResolvePositionalConstraints()
{
	float minimumSeparationDistance = 0.0f;

	for (size_t constraintIndex = 0; constraintIndex < m_SolverData.m_NumberOfContacts; ++constraintIndex)
	{
		PositionalConstraint* currentPositionalConstraint = m_AllPositionalConstraints + constraintIndex;

		int32_t firstBodyID = currentPositionalConstraint->m_FirstBodySpacePartitionID;
		int32_t secondBodyID = currentPositionalConstraint->m_SecondBodySpacePartitionID;

		float iMass_1 = currentPositionalConstraint->m_FirstBodyInverseMass;
		float iMass_2 = currentPositionalConstraint->m_SecondBodyInverseMass;
		float iMomentOfInertia_1 = currentPositionalConstraint->m_FirstBodyInverseMomentOfInertia;
		float iMomentOfInertia_2 = currentPositionalConstraint->m_SecondBodyInverseMomentOfInertia;

		Vector2D localCenter_1 = currentPositionalConstraint->m_FirstBodyLocalCenter;
		Vector2D localCenter_2 = currentPositionalConstraint->m_SecondBodyLocalCenter;

		size_t numberOfContactPoints = currentPositionalConstraint->m_NumberOfContactPoints;

		Vector2D position_1 = m_SolverData.m_AllPositions[firstBodyID];
		float rotation_1 = m_SolverData.m_AllRotations[firstBodyID];
		Vector2D position_2 = m_SolverData.m_AllPositions[secondBodyID];
		float rotation_2 = m_SolverData.m_AllRotations[secondBodyID];

		for (size_t contactIndex = 0; contactIndex < numberOfContactPoints; ++contactIndex)
		{
			Transform2D firstTransform;
			firstTransform.m_Rotation.SetRotator2D(rotation_1);
			firstTransform.m_Position = position_1 - Multiply(firstTransform.m_Rotation, localCenter_1);

			Transform2D secondTransform;
			secondTransform.m_Rotation.SetRotator2D(rotation_2);
			secondTransform.m_Position = position_2 - Multiply(secondTransform.m_Rotation, localCenter_2);

			Vector2D normalVector = Vector2D::ZERO;
			Vector2D contactPoint = Vector2D::ZERO;
			float separationDistance = 0.0f;
			GetContactValues(currentPositionalConstraint, firstTransform, secondTransform, contactIndex, normalVector, contactPoint, separationDistance);

			Vector2D displacement_1 = contactPoint - position_1;
			Vector2D displacement_2 = contactPoint - position_2;

			minimumSeparationDistance = GetMinimum(minimumSeparationDistance, separationDistance);
			float clampedSeparation = ClampValue(0.2f * (separationDistance + LINEAR_DELTA), -0.2f, 0.0f);

			float cpResult_1 = Vector2D::CrossProduct(displacement_1, normalVector);
			float cpResult_2 = Vector2D::CrossProduct(displacement_2, normalVector);
			float normalValue = iMass_1 + iMass_2 + (iMomentOfInertia_1 * cpResult_1 * cpResult_1) + (iMomentOfInertia_2 * cpResult_2 * cpResult_2);

			float normalForce = (normalValue > 0.0f) ? -clampedSeparation / normalValue : 0.0f;
			Vector2D normalImpulse = normalVector * normalForce;

			position_1 -= normalImpulse * iMass_1;
			rotation_1 -= Vector2D::CrossProduct(displacement_1, normalImpulse) * iMomentOfInertia_1;

			position_2 += normalImpulse * iMass_2;
			rotation_2 += Vector2D::CrossProduct(displacement_2, normalImpulse) * iMomentOfInertia_2;
		}

		m_SolverData.m_AllPositions[firstBodyID] = position_1;
		m_SolverData.m_AllRotations[firstBodyID] = rotation_1;
		m_SolverData.m_AllPositions[secondBodyID] = position_2;
		m_SolverData.m_AllRotations[secondBodyID] = rotation_2;
	}

	return (minimumSeparationDistance >= -3.0f * LINEAR_DELTA);
}



bool ContactSolver::ResolveTimeOfImpactPositionalConstraints(int32_t firstSpacePartitionID, int32_t secondSpacePartitionID)
{
	float minimumSeparationDistance = 0.0f;

	for (size_t constraintIndex = 0; constraintIndex < m_SolverData.m_NumberOfContacts; ++constraintIndex)
	{
		PositionalConstraint* currentPositionalConstraint = m_AllPositionalConstraints + constraintIndex;

		int32_t firstBodyID = currentPositionalConstraint->m_FirstBodySpacePartitionID;
		int32_t secondBodyID = currentPositionalConstraint->m_SecondBodySpacePartitionID;

		float iMass_1 = 0.0f;
		float iMass_2 = 0.0f;
		float iMomentOfInertia_1 = 0.0f;
		float iMomentOfInertia_2 = 0.0f;

		if (firstBodyID == firstSpacePartitionID || firstBodyID == secondSpacePartitionID)
		{
			iMass_1 = currentPositionalConstraint->m_FirstBodyInverseMass;
			iMomentOfInertia_1 = currentPositionalConstraint->m_FirstBodyInverseMomentOfInertia;
		}

		if (secondBodyID == firstSpacePartitionID || secondBodyID == secondSpacePartitionID)
		{
			iMass_2 = currentPositionalConstraint->m_SecondBodyInverseMass;
			iMomentOfInertia_2 = currentPositionalConstraint->m_SecondBodyInverseMomentOfInertia;
		}

		Vector2D localCenter_1 = currentPositionalConstraint->m_FirstBodyLocalCenter;
		Vector2D localCenter_2 = currentPositionalConstraint->m_SecondBodyLocalCenter;

		size_t numberOfContactPoints = currentPositionalConstraint->m_NumberOfContactPoints;

		Vector2D position_1 = m_SolverData.m_AllPositions[firstBodyID];
		float rotation_1 = m_SolverData.m_AllRotations[firstBodyID];
		Vector2D position_2 = m_SolverData.m_AllPositions[secondBodyID];
		float rotation_2 = m_SolverData.m_AllRotations[secondBodyID];

		for (size_t contactIndex = 0; contactIndex < numberOfContactPoints; ++contactIndex)
		{
			Transform2D firstTransform;
			firstTransform.m_Rotation.SetRotator2D(rotation_1);
			firstTransform.m_Position = position_1 - Multiply(firstTransform.m_Rotation, localCenter_1);

			Transform2D secondTransform;
			secondTransform.m_Rotation.SetRotator2D(rotation_2);
			secondTransform.m_Position = position_2 - Multiply(secondTransform.m_Rotation, localCenter_2);

			Vector2D normalVector = Vector2D::ZERO;
			Vector2D contactPoint = Vector2D::ZERO;
			float separationDistance = 0.0f;
			GetContactValues(currentPositionalConstraint, firstTransform, secondTransform, contactIndex, normalVector, contactPoint, separationDistance);

			Vector2D displacement_1 = contactPoint - position_1;
			Vector2D displacement_2 = contactPoint - position_2;

			minimumSeparationDistance = GetMinimum(minimumSeparationDistance, separationDistance);
			float clampedSeparation = ClampValue(0.75f * (separationDistance + LINEAR_DELTA), -0.2f, 0.0f);

			float cpResult_1 = Vector2D::CrossProduct(displacement_1, normalVector);
			float cpResult_2 = Vector2D::CrossProduct(displacement_2, normalVector);
			float normalValue = iMass_1 + iMass_2 + (iMomentOfInertia_1 * cpResult_1 * cpResult_1) + (iMomentOfInertia_2 * cpResult_2 * cpResult_2);

			float normalForce = (normalValue > 0.0f) ? -clampedSeparation / normalValue : 0.0f;
			Vector2D normalImpulse = normalVector * normalForce;

			position_1 -= normalImpulse * iMass_1;
			rotation_1 -= Vector2D::CrossProduct(displacement_1, normalImpulse) * iMomentOfInertia_1;

			position_2 += normalImpulse * iMass_2;
			rotation_2 += Vector2D::CrossProduct(displacement_2, normalImpulse) * iMomentOfInertia_2;
		}

		m_SolverData.m_AllPositions[firstBodyID] = position_1;
		m_SolverData.m_AllRotations[firstBodyID] = rotation_1;
		m_SolverData.m_AllPositions[secondBodyID] = position_2;
		m_SolverData.m_AllRotations[secondBodyID] = rotation_2;
	}

	return (minimumSeparationDistance >= -1.5f * LINEAR_DELTA);
}



void ContactSolver::ResolveVelocityConstraints()
{
	for (size_t constraintIndex = 0; constraintIndex < m_SolverData.m_NumberOfContacts; ++constraintIndex)
	{
		VelocityConstraint* currentVelocityConstraint = m_AllVelocityConstraints + constraintIndex;

		int32_t firstBodyID = currentVelocityConstraint->m_FirstBodySpacePartitionID;
		int32_t secondBodyID = currentVelocityConstraint->m_SecondBodySpacePartitionID;

		float iMass_1 = currentVelocityConstraint->m_FirstBodyInverseMass;
		float iMass_2 = currentVelocityConstraint->m_SecondBodyInverseMass;
		float iMomentOfInertia_1 = currentVelocityConstraint->m_FirstBodyInverseMomentOfInertia;
		float iMomentOfInertia_2 = currentVelocityConstraint->m_SecondBodyInverseMomentOfInertia;

		size_t numberOfContactPoints = currentVelocityConstraint->m_NumberOfContactPoints;

		Vector2D linearVelocity_1 = m_SolverData.m_AllLinearVelocities[firstBodyID];
		float angularVelocity_1 = m_SolverData.m_AllAngularVelocities[firstBodyID];
		Vector2D linearVelocity_2 = m_SolverData.m_AllLinearVelocities[secondBodyID];
		float angularVelocity_2 = m_SolverData.m_AllAngularVelocities[secondBodyID];

		Vector2D normalVector = currentVelocityConstraint->m_Normal;
		Vector2D tangentVector = Vector2D::CrossProduct(normalVector, 1.0f);

		ASSERT_OR_DIE(numberOfContactPoints > 0U || numberOfContactPoints <= 2U, "Number of contact points out of range.");

		for (size_t contactIndex = 0; contactIndex < numberOfContactPoints; ++contactIndex)
		{
			ConstraintPoint* currentConstraintPoint = currentVelocityConstraint->m_ConstraintPoints + contactIndex;

			Vector2D firstVelocity = linearVelocity_1 + Vector2D::CrossProduct(angularVelocity_1, currentConstraintPoint->m_FirstPoint);
			Vector2D secondVelocity = linearVelocity_2 + Vector2D::CrossProduct(angularVelocity_2, currentConstraintPoint->m_SecondPoint);
			Vector2D relativeVelocity = secondVelocity - firstVelocity;

			float tangentialSpeed = Vector2D::DotProduct(relativeVelocity, tangentVector) - currentVelocityConstraint->m_TangentialSpeed;
			float tangentialForce = currentConstraintPoint->m_TangentMass * (-tangentialSpeed);

			float maximumCoefficientOfFriction = currentVelocityConstraint->m_CoefficientOfFriction * currentConstraintPoint->m_NormalImpulse;
			float newImpulse = ClampValue(currentConstraintPoint->m_TangentImpulse + tangentialForce, -maximumCoefficientOfFriction, maximumCoefficientOfFriction);
			tangentialForce = newImpulse - currentConstraintPoint->m_TangentImpulse;
			currentConstraintPoint->m_TangentImpulse = newImpulse;

			Vector2D tangentImpulse = tangentVector * tangentialForce;

			linearVelocity_1 -= tangentImpulse * iMass_1;
			angularVelocity_1 -= Vector2D::CrossProduct(currentConstraintPoint->m_FirstPoint, tangentImpulse) * iMomentOfInertia_1;

			linearVelocity_2 += tangentImpulse * iMass_2;
			angularVelocity_2 += Vector2D::CrossProduct(currentConstraintPoint->m_SecondPoint, tangentImpulse) * iMomentOfInertia_2;
		}

		if (numberOfContactPoints == 1U)
		{
			for (size_t contactIndex = 0; contactIndex < numberOfContactPoints; ++contactIndex)
			{
				ConstraintPoint* currentConstraintPoint = currentVelocityConstraint->m_ConstraintPoints + contactIndex;

				Vector2D firstVelocity = linearVelocity_1 + Vector2D::CrossProduct(angularVelocity_1, currentConstraintPoint->m_FirstPoint);
				Vector2D secondVelocity = linearVelocity_2 + Vector2D::CrossProduct(angularVelocity_2, currentConstraintPoint->m_SecondPoint);
				Vector2D relativeVelocity = secondVelocity - firstVelocity;

				float normalSpeed = Vector2D::DotProduct(relativeVelocity, normalVector);
				float normalForce = -currentConstraintPoint->m_NormalMass * (normalSpeed - currentConstraintPoint->m_VelocityBias);

				float newImpulse = GetMaximum(currentConstraintPoint->m_NormalImpulse + normalForce, 0.0f);
				normalForce = newImpulse - currentConstraintPoint->m_NormalImpulse;
				currentConstraintPoint->m_NormalImpulse = newImpulse;

				Vector2D normalImpulse = normalVector * normalForce;

				linearVelocity_1 -= normalImpulse * iMass_1;
				angularVelocity_1 -= Vector2D::CrossProduct(currentConstraintPoint->m_FirstPoint, normalImpulse) * iMomentOfInertia_1;

				linearVelocity_2 += normalImpulse * iMass_2;
				angularVelocity_2 += Vector2D::CrossProduct(currentConstraintPoint->m_SecondPoint, normalImpulse) * iMomentOfInertia_2;
			}
		}
		else
		{
			ConstraintPoint* firstConstraintPoint = currentVelocityConstraint->m_ConstraintPoints + 0;
			ConstraintPoint* secondConstraintPoint = currentVelocityConstraint->m_ConstraintPoints + 1;

			Vector2D normalImpulseVector = Vector2D(firstConstraintPoint->m_NormalImpulse, secondConstraintPoint->m_NormalImpulse);
			ASSERT_OR_DIE(normalImpulseVector.X >= 0.0f && normalImpulseVector.Y >= 0.0f, "Impulses are negative.");

			Vector2D firstVelocity_1 = linearVelocity_1 + Vector2D::CrossProduct(angularVelocity_1, firstConstraintPoint->m_FirstPoint);
			Vector2D secondVelocity_1 = linearVelocity_2 + Vector2D::CrossProduct(angularVelocity_2, firstConstraintPoint->m_SecondPoint);
			Vector2D relativeVelocity_1 = secondVelocity_1 - firstVelocity_1;

			Vector2D firstVelocity_2 = linearVelocity_1 + Vector2D::CrossProduct(angularVelocity_1, secondConstraintPoint->m_FirstPoint);
			Vector2D secondVelocity_2 = linearVelocity_2 + Vector2D::CrossProduct(angularVelocity_2, secondConstraintPoint->m_SecondPoint);
			Vector2D relativeVelocity_2 = secondVelocity_2 - firstVelocity_2;

			float normalSpeed_1 = Vector2D::DotProduct(relativeVelocity_1, normalVector);
			float normalSpeed_2 = Vector2D::DotProduct(relativeVelocity_2, normalVector);

			Vector2D velocityDifference = Vector2D(normalSpeed_1 - firstConstraintPoint->m_VelocityBias, normalSpeed_2 - secondConstraintPoint->m_VelocityBias);
			velocityDifference -= Multiply(currentVelocityConstraint->m_InverseNormalMass, normalImpulseVector);

			while (true)
			{
				Vector2D resultantImpulse = Multiply(currentVelocityConstraint->m_NormalMass, velocityDifference).GetNegatedVector2D();
				if (resultantImpulse.X >= 0.0f && resultantImpulse.Y >= 0.0f)
				{
					Vector2D incrementalImpulse = resultantImpulse - normalImpulseVector;

					Vector2D normalImpulse_1 = normalVector * incrementalImpulse.X;
					Vector2D normalImpulse_2 = normalVector * incrementalImpulse.Y;

					linearVelocity_1 -= (normalImpulse_1 + normalImpulse_2) * iMass_1;
					angularVelocity_1 -= (Vector2D::CrossProduct(firstConstraintPoint->m_FirstPoint, normalImpulse_1) + Vector2D::CrossProduct(secondConstraintPoint->m_FirstPoint, normalImpulse_2)) * iMomentOfInertia_1;

					linearVelocity_2 += (normalImpulse_1 + normalImpulse_2) * iMass_2;
					angularVelocity_2 += (Vector2D::CrossProduct(firstConstraintPoint->m_SecondPoint, normalImpulse_1) + Vector2D::CrossProduct(secondConstraintPoint->m_SecondPoint, normalImpulse_2)) * iMomentOfInertia_2;

					firstConstraintPoint->m_NormalImpulse = resultantImpulse.X;
					secondConstraintPoint->m_NormalImpulse = resultantImpulse.Y;

					break;
				}

				resultantImpulse.X = -firstConstraintPoint->m_NormalMass * velocityDifference.X;
				resultantImpulse.Y = 0.0f;
				
				normalSpeed_1 = 0.0f;
				normalSpeed_2 = (currentVelocityConstraint->m_InverseNormalMass.m_Matrix2x2[2] * resultantImpulse.X) + velocityDifference.Y;
				
				if (resultantImpulse.X >= 0.0f && normalSpeed_2 >= 0.0f)
				{
					Vector2D incrementalImpulse = resultantImpulse - normalImpulseVector;

					Vector2D normalImpulse_1 = normalVector * incrementalImpulse.X;
					Vector2D normalImpulse_2 = normalVector * incrementalImpulse.Y;

					linearVelocity_1 -= (normalImpulse_1 + normalImpulse_2) * iMass_1;
					angularVelocity_1 -= (Vector2D::CrossProduct(firstConstraintPoint->m_FirstPoint, normalImpulse_1) + Vector2D::CrossProduct(secondConstraintPoint->m_FirstPoint, normalImpulse_2)) * iMomentOfInertia_1;

					linearVelocity_2 += (normalImpulse_1 + normalImpulse_2) * iMass_2;
					angularVelocity_2 += (Vector2D::CrossProduct(firstConstraintPoint->m_SecondPoint, normalImpulse_1) + Vector2D::CrossProduct(secondConstraintPoint->m_SecondPoint, normalImpulse_2)) * iMomentOfInertia_2;

					firstConstraintPoint->m_NormalImpulse = resultantImpulse.X;
					secondConstraintPoint->m_NormalImpulse = resultantImpulse.Y;

					break;
				}

				resultantImpulse.X = 0.0f;
				resultantImpulse.Y = -secondConstraintPoint->m_NormalMass * velocityDifference.Y;

				normalSpeed_1 = (currentVelocityConstraint->m_InverseNormalMass.m_Matrix2x2[1] * resultantImpulse.Y) + velocityDifference.X;
				normalSpeed_2 = 0.0f;

				if (resultantImpulse.Y >= 0.0f && normalSpeed_1 >= 0.0f)
				{
					Vector2D incrementalImpulse = resultantImpulse - normalImpulseVector;

					Vector2D normalImpulse_1 = normalVector * incrementalImpulse.X;
					Vector2D normalImpulse_2 = normalVector * incrementalImpulse.Y;

					linearVelocity_1 -= (normalImpulse_1 + normalImpulse_2) * iMass_1;
					angularVelocity_1 -= (Vector2D::CrossProduct(firstConstraintPoint->m_FirstPoint, normalImpulse_1) + Vector2D::CrossProduct(secondConstraintPoint->m_FirstPoint, normalImpulse_2)) * iMomentOfInertia_1;

					linearVelocity_2 += (normalImpulse_1 + normalImpulse_2) * iMass_2;
					angularVelocity_2 += (Vector2D::CrossProduct(firstConstraintPoint->m_SecondPoint, normalImpulse_1) + Vector2D::CrossProduct(secondConstraintPoint->m_SecondPoint, normalImpulse_2)) * iMomentOfInertia_2;

					firstConstraintPoint->m_NormalImpulse = resultantImpulse.X;
					secondConstraintPoint->m_NormalImpulse = resultantImpulse.Y;

					break;
				}

				resultantImpulse.X = 0.0f;
				resultantImpulse.Y = 0.0f;

				normalSpeed_1 = velocityDifference.X;
				normalSpeed_2 = velocityDifference.Y;

				if (normalSpeed_1 >= 0.0f && normalSpeed_2 >= 0.0f)
				{
					Vector2D incrementalImpulse = resultantImpulse - normalImpulseVector;

					Vector2D normalImpulse_1 = normalVector * incrementalImpulse.X;
					Vector2D normalImpulse_2 = normalVector * incrementalImpulse.Y;

					linearVelocity_1 -= (normalImpulse_1 + normalImpulse_2) * iMass_1;
					angularVelocity_1 -= (Vector2D::CrossProduct(firstConstraintPoint->m_FirstPoint, normalImpulse_1) + Vector2D::CrossProduct(secondConstraintPoint->m_FirstPoint, normalImpulse_2)) * iMomentOfInertia_1;

					linearVelocity_2 += (normalImpulse_1 + normalImpulse_2) * iMass_2;
					angularVelocity_2 += (Vector2D::CrossProduct(firstConstraintPoint->m_SecondPoint, normalImpulse_1) + Vector2D::CrossProduct(secondConstraintPoint->m_SecondPoint, normalImpulse_2)) * iMomentOfInertia_2;

					firstConstraintPoint->m_NormalImpulse = resultantImpulse.X;
					secondConstraintPoint->m_NormalImpulse = resultantImpulse.Y;

					break;
				}

				break;
			}
		}

		m_SolverData.m_AllLinearVelocities[firstBodyID] = linearVelocity_1;
		m_SolverData.m_AllAngularVelocities[firstBodyID] = angularVelocity_1;
		m_SolverData.m_AllLinearVelocities[secondBodyID] = linearVelocity_2;
		m_SolverData.m_AllAngularVelocities[secondBodyID] = angularVelocity_2;
	}
}



void ContactSolver::GetContactValues(PositionalConstraint* positionalConstraint, const Transform2D& firstBodyTransform, const Transform2D& secondBodyTransform, size_t contactIndex, Vector2D& normalVector, Vector2D& contactPoint, float& separationDistance)
{
	ASSERT_OR_DIE(positionalConstraint->m_NumberOfContactPoints > 0, "No contact points.");

	switch (positionalConstraint->m_ClusterType)
	{
	case CIRCLE_CLUSTER_TYPE:
	{
		Vector2D firstPoint = Multiply(firstBodyTransform, positionalConstraint->m_LocalReferencePoint);
		Vector2D secondPoint = Multiply(secondBodyTransform, positionalConstraint->m_ContactPoints[0]);
		normalVector = secondPoint - firstPoint;
		normalVector = normalVector.GetNormalizedVector2D();
		contactPoint = (firstPoint + secondPoint) * 0.5f;
		separationDistance = Vector2D::DotProduct(secondPoint - firstPoint, normalVector) - (positionalConstraint->m_FirstBoundingRadius + positionalConstraint->m_SecondBoundingRadius);
	}
		break;

	case FIRST_EDGE_CLUSTER_TYPE:
	{
		normalVector = Multiply(firstBodyTransform.m_Rotation, positionalConstraint->m_LocalReferenceNormal);
		Vector2D firstPoint = Multiply(firstBodyTransform, positionalConstraint->m_LocalReferencePoint);
		Vector2D secondPoint = Multiply(secondBodyTransform, positionalConstraint->m_ContactPoints[contactIndex]);
		separationDistance = Vector2D::DotProduct(secondPoint - firstPoint, normalVector) - (positionalConstraint->m_FirstBoundingRadius + positionalConstraint->m_SecondBoundingRadius);
		contactPoint = secondPoint;
	}
		break;

	case SECOND_EDGE_CLUSTER_TYPE:
	{
		normalVector = Multiply(secondBodyTransform.m_Rotation, positionalConstraint->m_LocalReferenceNormal);
		Vector2D firstPoint = Multiply(secondBodyTransform, positionalConstraint->m_LocalReferencePoint);
		Vector2D secondPoint = Multiply(firstBodyTransform, positionalConstraint->m_ContactPoints[contactIndex]);
		separationDistance = Vector2D::DotProduct(secondPoint - firstPoint, normalVector) - (positionalConstraint->m_FirstBoundingRadius + positionalConstraint->m_SecondBoundingRadius);
		contactPoint = secondPoint;

		normalVector = normalVector.GetNegatedVector2D();
	}
		break;

	default:
		break;
	}
}