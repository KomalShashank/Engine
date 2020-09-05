#include "Engine/PhysicsSystem/ContactSolver/Contacts/Contact.hpp"
#include "Engine/PhysicsSystem/ContactSolver/Contacts/LineVersusCircleContact.hpp"
#include "Engine/PhysicsSystem/ContactSolver/Contacts/LineVersusCapsuleContact.hpp"
#include "Engine/PhysicsSystem/ContactSolver/Contacts/LineVersusPolygonContact.hpp"

#include "Engine/PhysicsSystem/ContactSolver/Contacts/CircleVersusCircleContact.hpp"
#include "Engine/PhysicsSystem/ContactSolver/Contacts/CircleVersusCapsuleContact.hpp"
#include "Engine/PhysicsSystem/ContactSolver/Contacts/CircleVersusPolygonContact.hpp"

#include "Engine/PhysicsSystem/ContactSolver/Contacts/CapsuleVersusCapsuleContact.hpp"
#include "Engine/PhysicsSystem/ContactSolver/Contacts/CapsuleVersusPolygonContact.hpp"

#include "Engine/PhysicsSystem/ContactSolver/Contacts/PolygonVersusPolygonContact.hpp"

#include "Engine/PhysicsSystem/RigidBody/BodyFixture.hpp"
#include "Engine/PhysicsSystem/RigidBody/RigidBody.hpp"



ContactCallbacks::ContactCallbacks()
{

}



ContactCallbacks::~ContactCallbacks()
{

}



void ContactCallbacks::OnContactStarted(Contact*)
{
	// This is an event that can be overridden to do game-specific or application-specific stuff.
}



void ContactCallbacks::OnContactEnded(Contact*)
{
	// This is an event that can be overridden to do game-specific or application-specific stuff.
}



void ContactCallbacks::OnSimulationStart(Contact*, const LocalContactCluster*)
{
	// This is an event that can be overridden to do game-specific or application-specific stuff.
}



void ContactCallbacks::OnSimulationEnd(Contact*, float*, float*, size_t)
{
	// This is an event that can be overridden to do game-specific or application-specific stuff.
}



ContactFunctions Contact::s_ContactFunctionRegisty[NUMBER_OF_SHAPE_TYPES][NUMBER_OF_SHAPE_TYPES];
bool Contact::s_FunctionsInitialized = false;



Contact::Contact() :
	m_PreviousContact(nullptr),
	m_NextContact(nullptr),
	m_FirstFixture(nullptr),
	m_SecondFixture(nullptr),
	m_CoefficientOfFriction(0.0f),
	m_CoefficientOfRestitution(0.0f),
	m_TangentialSpeed(0.0f),
	m_TimeOfImpactDuration(0.0f),
	m_NumberOfTimesOfImpact(0U),
	m_ContactFlags(ENABLED_FLAG)
{

}



Contact::Contact(BodyFixture* firstFixture, BodyFixture* secondFixture) :
	m_PreviousContact(nullptr),
	m_NextContact(nullptr),
	m_FirstFixture(firstFixture),
	m_SecondFixture(secondFixture),
	m_CoefficientOfFriction(0.0f),
	m_CoefficientOfRestitution(0.0f),
	m_TangentialSpeed(0.0f),
	m_TimeOfImpactDuration(0.0f),
	m_NumberOfTimesOfImpact(0U),
	m_ContactFlags(ENABLED_FLAG)
{
	m_LocalContactCluster.m_NumberOfContactPoints = 0U;
	
	m_CoefficientOfFriction = CombineCoefficientsOfFriction(m_FirstFixture->GetCoefficientOfFriction(), m_SecondFixture->GetCoefficientOfFriction());
	m_CoefficientOfRestitution = CombineCoefficientsOfRestitution(m_FirstFixture->GetCoefficientOfRestitution(), m_SecondFixture->GetCoefficientOfRestitution());
}



Contact::~Contact()
{

}



float Contact::CombineCoefficientsOfFriction(float firstCoefficientOfFriction, float secondCoefficientOfFriction)
{
	return SquareRootOfValue(firstCoefficientOfFriction * secondCoefficientOfFriction);
}



float Contact::CombineCoefficientsOfRestitution(float firstCoefficientOfRestitution, float secondCoefficientOfRestitution)
{
	return (firstCoefficientOfRestitution > secondCoefficientOfRestitution) ? firstCoefficientOfRestitution : secondCoefficientOfRestitution;
}



void Contact::InitializeContactFunctions()
{
	AddContactFunction(LineVersusCircleContact::CreateContact, LineVersusCircleContact::DestroyContact, LINE_SHAPE, CIRCLE_SHAPE);
	AddContactFunction(LineVersusCapsuleContact::CreateContact, LineVersusCapsuleContact::DestroyContact, LINE_SHAPE, CAPSULE_SHAPE);
	AddContactFunction(LineVersusPolygonContact::CreateContact, LineVersusPolygonContact::DestroyContact, LINE_SHAPE, POLYGON_SHAPE);

	AddContactFunction(CircleVersusCircleContact::CreateContact, CircleVersusCircleContact::DestroyContact, CIRCLE_SHAPE, CIRCLE_SHAPE);
	AddContactFunction(CircleVersusCapsuleContact::CreateContact, CircleVersusCapsuleContact::DestroyContact, CIRCLE_SHAPE, CAPSULE_SHAPE);
	AddContactFunction(CircleVersusPolygonContact::CreateContact, CircleVersusPolygonContact::DestroyContact, CIRCLE_SHAPE, POLYGON_SHAPE);

	AddContactFunction(CapsuleVersusCapsuleContact::CreateContact, CapsuleVersusCapsuleContact::DestroyContact, CAPSULE_SHAPE, CAPSULE_SHAPE);
	AddContactFunction(CapsuleVersusPolygonContact::CreateContact, CapsuleVersusPolygonContact::DestroyContact, CAPSULE_SHAPE, POLYGON_SHAPE);

	AddContactFunction(PolygonVersusPolygonContact::CreateContact, PolygonVersusPolygonContact::DestroyContact, POLYGON_SHAPE, POLYGON_SHAPE);
}



void Contact::AddContactFunction(ContactCreator* contactCreator, ContactDestroyer* contactDestroyer, uint8_t firstShapeType, uint8_t secondShapeType)
{
	ASSERT_OR_DIE(firstShapeType != INVALID_SHAPE, "Invalid shape type.");
	ASSERT_OR_DIE(secondShapeType != INVALID_SHAPE, "Invalid shape type.");

	s_ContactFunctionRegisty[firstShapeType][secondShapeType].m_Creator = contactCreator;
	s_ContactFunctionRegisty[firstShapeType][secondShapeType].m_Destroyer = contactDestroyer;
	s_ContactFunctionRegisty[firstShapeType][secondShapeType].m_FirstShapeType = firstShapeType;
	s_ContactFunctionRegisty[firstShapeType][secondShapeType].m_SecondShapeType = secondShapeType;

	if (firstShapeType != secondShapeType)
	{
		s_ContactFunctionRegisty[secondShapeType][firstShapeType].m_Creator = contactCreator;
		s_ContactFunctionRegisty[secondShapeType][firstShapeType].m_Destroyer = contactDestroyer;
		s_ContactFunctionRegisty[secondShapeType][firstShapeType].m_FirstShapeType = firstShapeType;
		s_ContactFunctionRegisty[secondShapeType][firstShapeType].m_SecondShapeType = secondShapeType;
	}
}



Contact* Contact::CreateContact(BodyFixture* firstFixture, BodyFixture* secondFixture, BlockMemoryAllocator* blockAllocator)
{
	if (!s_FunctionsInitialized)
	{
		InitializeContactFunctions();
		s_FunctionsInitialized = true;
	}

	uint8_t firstShapeType = firstFixture->GetFixtureShape()->GetShapeType();
	uint8_t secondShapeType = secondFixture->GetFixtureShape()->GetShapeType();

	ASSERT_OR_DIE(firstShapeType != INVALID_SHAPE, "Invalid shape type.");
	ASSERT_OR_DIE(secondShapeType != INVALID_SHAPE, "Invalid shape type.");

	ContactCreator* contactCreator = s_ContactFunctionRegisty[firstShapeType][secondShapeType].m_Creator;
	if (contactCreator != nullptr)
	{
		uint8_t firstType = s_ContactFunctionRegisty[firstShapeType][secondShapeType].m_FirstShapeType;
		uint8_t secondType = s_ContactFunctionRegisty[firstShapeType][secondShapeType].m_SecondShapeType;

		if (firstType == firstShapeType && secondType == secondShapeType)
		{
			return contactCreator(firstFixture, secondFixture, blockAllocator);
		}
		else if (firstType == secondShapeType && secondType == firstShapeType)
		{
			return contactCreator(secondFixture, firstFixture, blockAllocator);
		}
	}

	return nullptr;
}



void Contact::DestroyContact(Contact* currentContact, BlockMemoryAllocator* blockAllocator)
{
	ASSERT_OR_DIE(s_FunctionsInitialized, "Contact Functions are not intialized.");

	BodyFixture* firstFixture = currentContact->m_FirstFixture;
	BodyFixture* secondFixture = currentContact->m_SecondFixture;

	if (currentContact->m_LocalContactCluster.m_NumberOfContactPoints > 0U && !firstFixture->IsOverlapOnly() && !secondFixture->IsOverlapOnly())
	{
		firstFixture->GetParentBody()->SetBodyAwake(true);
		secondFixture->GetParentBody()->SetBodyAwake(true);
	}

	uint8_t firstShapeType = firstFixture->GetFixtureShape()->GetShapeType();
	uint8_t secondShapeType = secondFixture->GetFixtureShape()->GetShapeType();

	ASSERT_OR_DIE(firstShapeType != INVALID_SHAPE, "Invalid shape type.");
	ASSERT_OR_DIE(secondShapeType != INVALID_SHAPE, "Invalid shape type.");

	ContactDestroyer* contactDestroyer = s_ContactFunctionRegisty[firstShapeType][secondShapeType].m_Destroyer;
	contactDestroyer(currentContact, blockAllocator);
}



void Contact::UpdateContact(ContactCallbacks* contactCallbacks)
{
	LocalContactCluster existingLocalContactCluster = m_LocalContactCluster;
	SetContactEnabled(true);

	bool fixturesCurrentlyInContact = false;
	bool fixturesPreviouslyInContact = AreFixturesInContact();

	bool overlapOnly = m_FirstFixture->IsOverlapOnly() || m_SecondFixture->IsOverlapOnly();

	Transform2D firstFixtureTransform = m_FirstFixture->GetParentBody()->GetWorldTransform2D();
	Transform2D secondFixtureTransform = m_SecondFixture->GetParentBody()->GetWorldTransform2D();

	if (overlapOnly)
	{
		fixturesCurrentlyInContact = AreShapesOverlapping(m_FirstFixture->GetFixtureShape(), firstFixtureTransform, m_SecondFixture->GetFixtureShape(), secondFixtureTransform);
		m_LocalContactCluster.m_NumberOfContactPoints = 0U;
	}
	else
	{
		GenerateLocalContactCluster(&m_LocalContactCluster, firstFixtureTransform, secondFixtureTransform);
		fixturesCurrentlyInContact = (m_LocalContactCluster.m_NumberOfContactPoints > 0U);

		for (size_t newContactPointIndex = 0; newContactPointIndex < m_LocalContactCluster.m_NumberOfContactPoints; ++newContactPointIndex)
		{
			ContactPoint* newContactPoint = m_LocalContactCluster.m_AllContactPoints + newContactPointIndex;
			newContactPoint->m_PushBackImpulse = 0.0f;
			newContactPoint->m_FrictionImpulse = 0.0f;

			for (size_t existingContactPointIndex = 0; existingContactPointIndex < existingLocalContactCluster.m_NumberOfContactPoints; ++existingContactPointIndex)
			{
				ContactPoint* existingContactPoint = existingLocalContactCluster.m_AllContactPoints + existingContactPointIndex;
				if (newContactPoint->m_TypeID.m_ID == existingContactPoint->m_TypeID.m_ID)
				{
					newContactPoint->m_PushBackImpulse = existingContactPoint->m_PushBackImpulse;
					newContactPoint->m_FrictionImpulse = existingContactPoint->m_FrictionImpulse;
					break;
				}
			}
		}

		if (fixturesCurrentlyInContact != fixturesPreviouslyInContact)
		{
			m_FirstFixture->GetParentBody()->SetBodyAwake(true);
			m_SecondFixture->GetParentBody()->SetBodyAwake(true);
		}
	}

	SetFixturesInContact(fixturesCurrentlyInContact);

	if (contactCallbacks != nullptr)
	{
		if (!fixturesPreviouslyInContact && fixturesCurrentlyInContact)
		{
			contactCallbacks->OnContactStarted(this);
		}

		if (fixturesPreviouslyInContact && !fixturesCurrentlyInContact)
		{
			contactCallbacks->OnContactEnded(this);
		}

		if (!overlapOnly && fixturesCurrentlyInContact)
		{
			contactCallbacks->OnSimulationStart(this, &existingLocalContactCluster);
		}
	}
}



void Contact::GetGlobalContactCluster(GlobalContactCluster* globalContactCluster) const
{
	Transform2D firstFixtureTransform = m_FirstFixture->GetParentBody()->GetWorldTransform2D();
	Transform2D secondFixtureTransform = m_SecondFixture->GetParentBody()->GetWorldTransform2D();

	float firstFixtureBoundingRadius = m_FirstFixture->GetFixtureShape()->GetBoundingRadius();
	float secondFixtureBoundingRadius = m_SecondFixture->GetFixtureShape()->GetBoundingRadius();

	globalContactCluster->GenerateGlobalCluster(&m_LocalContactCluster, firstFixtureTransform, firstFixtureBoundingRadius, secondFixtureTransform, secondFixtureBoundingRadius);
}



LocalContactCluster* Contact::GetLocalContactCluster()
{
	return &m_LocalContactCluster;
}



const LocalContactCluster* Contact::GetLocalContactCluster() const
{
	return &m_LocalContactCluster;
}



void Contact::SetPreviousContact(Contact* previousContact)
{
	m_PreviousContact = previousContact;
}



Contact* Contact::GetPreviousContact()
{
	return m_PreviousContact;
}



const Contact* Contact::GetPreviousContact() const
{
	return m_PreviousContact;
}



void Contact::SetNextContact(Contact* nextContact)
{
	m_NextContact = nextContact;
}



Contact* Contact::GetNextContact()
{
	return m_NextContact;
}



const Contact* Contact::GetNextContact() const
{
	return m_NextContact;
}



ContactNode& Contact::GetFirstContactNode()
{
	return m_FirstContactNode;
}



ContactNode& Contact::GetSecondContactNode()
{
	return m_SecondContactNode;
}



BodyFixture* Contact::GetFirstFixture()
{
	return m_FirstFixture;
}



const BodyFixture* Contact::GetFirstFixture() const
{
	return m_FirstFixture;
}



BodyFixture* Contact::GetSecondFixture()
{
	return m_SecondFixture;
}



const BodyFixture* Contact::GetSecondFixture() const
{
	return m_SecondFixture;
}



void Contact::SetCoefficientOfFriction(float coefficientOfFriction)
{
	m_CoefficientOfFriction = coefficientOfFriction;
}



float Contact::GetCoefficientOfFriction() const
{
	return m_CoefficientOfFriction;
}



void Contact::ResetCoefficientOfFriction()
{
	m_CoefficientOfFriction = CombineCoefficientsOfFriction(m_FirstFixture->GetCoefficientOfFriction(), m_SecondFixture->GetCoefficientOfFriction());
}



void Contact::SetCoefficientOfRestitution(float coefficientOfRestitution)
{
	m_CoefficientOfRestitution = coefficientOfRestitution;
}



float Contact::GetCoefficientOfRestitution() const
{
	return m_CoefficientOfRestitution;
}



void Contact::ResetCoefficientOfRestitution()
{
	m_CoefficientOfRestitution = CombineCoefficientsOfRestitution(m_FirstFixture->GetCoefficientOfRestitution(), m_SecondFixture->GetCoefficientOfRestitution());
}



void Contact::SetTangentialSpeed(float tangentialSpeed)
{
	m_TangentialSpeed = tangentialSpeed;
}



float Contact::GetTangentialSpeed() const
{
	return m_TangentialSpeed;
}



void Contact::SetTimeOfImpactDuration(float timeOfImpactDuration)
{
	m_TimeOfImpactDuration = timeOfImpactDuration;
}



float Contact::GetTimeOfImpactDuration() const
{
	return m_TimeOfImpactDuration;
}



void Contact::SetNumberOfTimesOfImpact(size_t numberOfTimesOfImpact)
{
	m_NumberOfTimesOfImpact = numberOfTimesOfImpact;
}



size_t Contact::GetNumberOfTimesOfImpact() const
{
	return m_NumberOfTimesOfImpact;
}



void Contact::SetContactEnabled(bool enabled)
{
	if (enabled)
	{
		m_ContactFlags |= ENABLED_FLAG;
	}
	else
	{
		m_ContactFlags &= ~ENABLED_FLAG;
	}
}



bool Contact::IsContactEnabled() const
{
	return ((m_ContactFlags & ENABLED_FLAG) == ENABLED_FLAG);
}



void Contact::SetFixturesInContact(bool fixturesInContact)
{
	if (fixturesInContact)
	{
		m_ContactFlags |= IN_CONTACT_FLAG;
	}
	else
	{
		m_ContactFlags &= ~IN_CONTACT_FLAG;
	}
}



bool Contact::AreFixturesInContact() const
{
	return ((m_ContactFlags & IN_CONTACT_FLAG) == IN_CONTACT_FLAG);
}



void Contact::SetContactPartOfSpacePartition(bool partOfSpacePartition)
{
	if (partOfSpacePartition)
	{
		m_ContactFlags |= PART_OF_SPACE_PARTITION_FLAG;
	}
	else
	{
		m_ContactFlags &= ~PART_OF_SPACE_PARTITION_FLAG;
	}
}



bool Contact::IsContactPartOfSpacePartition() const
{
	return ((m_ContactFlags & PART_OF_SPACE_PARTITION_FLAG) == PART_OF_SPACE_PARTITION_FLAG);
}



void Contact::SetValidTimeOfImpact(bool validTimeOfImpact)
{
	if (validTimeOfImpact)
	{
		m_ContactFlags |= VALID_TIME_OF_IMPACT_FLAG;
	}
	else
	{
		m_ContactFlags &= ~VALID_TIME_OF_IMPACT_FLAG;
	}
}



bool Contact::DoesHaveAValidTimeOfImpact() const
{
	return ((m_ContactFlags & VALID_TIME_OF_IMPACT_FLAG) == VALID_TIME_OF_IMPACT_FLAG);
}