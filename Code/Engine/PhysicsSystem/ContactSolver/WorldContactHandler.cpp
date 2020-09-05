#include "Engine/PhysicsSystem/ContactSolver/WorldContactHandler.hpp"
#include "Engine/PhysicsSystem/ContactSolver/Contacts/Contact.hpp"
#include "Engine/PhysicsSystem/RigidBody/BodyFixture.hpp"
#include "Engine/PhysicsSystem/RigidBody/RigidBody.hpp"



ContactCallbacks g_ContactCallbacks;



WorldContactHandler::WorldContactHandler() :
m_BlockAllocator(nullptr),
m_AllContacts(nullptr),
m_NumberOfContacts(0U)
{
	m_ContactCallbacks = &g_ContactCallbacks;
}



WorldContactHandler::~WorldContactHandler()
{

}



void WorldContactHandler::CreateNewContacts()
{
	m_BroadPhaseSystem.UpdateFixturePairs(this);
}



void WorldContactHandler::DestroyExistingContact(Contact* currentContact)
{
	RigidBody* firstBody = currentContact->GetFirstFixture()->GetParentBody();
	RigidBody* secondBody = currentContact->GetSecondFixture()->GetParentBody();

	if (m_ContactCallbacks != nullptr)
	{
		if (currentContact->AreFixturesInContact())
		{
			m_ContactCallbacks->OnContactEnded(currentContact);
		}
	}

	Contact* previousContact = currentContact->GetPreviousContact();
	Contact* nextContact = currentContact->GetNextContact();

	if (previousContact != nullptr)
	{
		previousContact->SetNextContact(nextContact);
	}

	if (nextContact != nullptr)
	{
		nextContact->SetPreviousContact(previousContact);
	}

	if (currentContact == m_AllContacts)
	{
		m_AllContacts = currentContact->GetNextContact();
	}

	ContactNode& firstContactNode = currentContact->GetFirstContactNode();

	if (firstContactNode.m_PreviousNode != nullptr)
	{
		firstContactNode.m_PreviousNode->m_NextNode = firstContactNode.m_NextNode;
	}

	if (firstContactNode.m_NextNode != nullptr)
	{
		firstContactNode.m_NextNode->m_PreviousNode = firstContactNode.m_PreviousNode;
	}

	if (&firstContactNode == firstBody->GetContactNodeList())
	{
		firstBody->SetContactNodeList(firstContactNode.m_NextNode);
	}

	ContactNode& secondContactNode = currentContact->GetSecondContactNode();

	if (secondContactNode.m_PreviousNode != nullptr)
	{
		secondContactNode.m_PreviousNode->m_NextNode = secondContactNode.m_NextNode;
	}

	if (secondContactNode.m_NextNode != nullptr)
	{
		secondContactNode.m_NextNode->m_PreviousNode = secondContactNode.m_PreviousNode;
	}

	if (&secondContactNode == secondBody->GetContactNodeList())
	{
		secondBody->SetContactNodeList(secondContactNode.m_NextNode);
	}

	Contact::DestroyContact(currentContact, m_BlockAllocator);
	--m_NumberOfContacts;
}



void WorldContactHandler::HandleCollision()
{
	for (Contact* currentContact = m_AllContacts; currentContact != nullptr;)
	{
		BodyFixture* firstFixture = currentContact->GetFirstFixture();
		BodyFixture* secondFixture = currentContact->GetSecondFixture();

		RigidBody* firstBody = firstFixture->GetParentBody();
		RigidBody* secondBody = secondFixture->GetParentBody();

		bool firstBodyIsAwake = firstBody->IsBodyAwake() && !firstBody->IsOfType(STATIC_BODY);
		bool secondBodyIsAwake = secondBody->IsBodyAwake() && !secondBody->IsOfType(STATIC_BODY);

		if (!firstBodyIsAwake && !secondBodyIsAwake)
		{
			currentContact = currentContact->GetNextContact();
			continue;
		}

		int32_t firstFixtureID = firstFixture->GetFixtureID();
		int32_t secondFixtureID = secondFixture->GetFixtureID();
		bool fixturesAreOverlapping = m_BroadPhaseSystem.AreFixturesOverlapping(firstFixtureID, secondFixtureID);

		if (!fixturesAreOverlapping)
		{
			Contact* destroyableContact = currentContact;
			currentContact = currentContact->GetNextContact();
			DestroyExistingContact(destroyableContact);
			continue;
		}

		currentContact->UpdateContact(m_ContactCallbacks);
		currentContact = currentContact->GetNextContact();
	}
}



void WorldContactHandler::AddFixturePair(void* firstFixtureData, void* secondFixtureData)
{
	BodyFixture* firstFixture = ((FixtureReference*)firstFixtureData)->m_BodyFixture;
	BodyFixture* secondFixture = ((FixtureReference*)secondFixtureData)->m_BodyFixture;

	RigidBody* firstBody = firstFixture->GetParentBody();
	RigidBody* secondBody = secondFixture->GetParentBody();

	if (firstBody == secondBody)
	{
		return;
	}

	for (ContactNode* currentContactNode = secondBody->GetContactNodeList(); currentContactNode != nullptr; currentContactNode = currentContactNode->m_NextNode)
	{
		if (currentContactNode->m_OtherBody == firstBody)
		{
			BodyFixture* currentFirstFixture = currentContactNode->m_Contact->GetFirstFixture();
			BodyFixture* currentSecondFixture = currentContactNode->m_Contact->GetSecondFixture();

			if (currentFirstFixture == firstFixture && currentSecondFixture == secondFixture)
			{
				return;
			}

			if (currentFirstFixture == secondFixture && currentSecondFixture == firstFixture)
			{
				return;
			}
		}
	}

	if (!RigidBody::CanBodiesCollide(firstBody, secondBody))
	{
		return;
	}

	Contact* newContact = Contact::CreateContact(firstFixture, secondFixture, m_BlockAllocator);
	if (newContact == nullptr)
	{
		return;
	}

	firstFixture = newContact->GetFirstFixture();
	secondFixture = newContact->GetSecondFixture();

	firstBody = firstFixture->GetParentBody();
	secondBody = secondFixture->GetParentBody();

	newContact->SetPreviousContact(nullptr);
	newContact->SetNextContact(m_AllContacts);
	if (m_AllContacts != nullptr)
	{
		m_AllContacts->SetPreviousContact(newContact);
	}

	m_AllContacts = newContact;

	ContactNode& firstContactNode = newContact->GetFirstContactNode();
	firstContactNode.m_Contact = newContact;
	firstContactNode.m_OtherBody = secondBody;

	ContactNode* firstBodyContactList = firstBody->GetContactNodeList();
	firstContactNode.m_PreviousNode = nullptr;
	firstContactNode.m_NextNode = firstBodyContactList;
	
	if (firstBodyContactList != nullptr)
	{
		firstBodyContactList->m_PreviousNode = &firstContactNode;
	}

	firstBody->SetContactNodeList(&firstContactNode);

	ContactNode& secondContactNode = newContact->GetSecondContactNode();
	secondContactNode.m_Contact = newContact;
	secondContactNode.m_OtherBody = firstBody;

	ContactNode* secondBodyContactList = secondBody->GetContactNodeList();
	secondContactNode.m_PreviousNode = nullptr;
	secondContactNode.m_NextNode = secondBodyContactList;

	if (secondBodyContactList != nullptr)
	{
		secondBodyContactList->m_PreviousNode = &secondContactNode;
	}

	secondBody->SetContactNodeList(&secondContactNode);

	if (!firstFixture->IsOverlapOnly() && !secondFixture->IsOverlapOnly())
	{
		firstBody->SetBodyAwake(true);
		secondBody->SetBodyAwake(true);
	}

	++m_NumberOfContacts;
}