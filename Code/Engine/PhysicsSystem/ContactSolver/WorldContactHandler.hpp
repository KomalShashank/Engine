#pragma once

#include "Engine/PhysicsSystem/General/PhysicsCommons.hpp"
#include "Engine/PhysicsSystem/CollisionDetection/BroadPhaseCollision.hpp"



class Contact;
class ContactCallbacks;
class BlockMemoryAllocator;



class WorldContactHandler
{
public:
	WorldContactHandler();
	~WorldContactHandler();

	void CreateNewContacts();
	void DestroyExistingContact(Contact* currentContact);
	
	void HandleCollision();
	void AddFixturePair(void* firstFixtureData, void* secondFixtureData);

public:
	BroadPhaseSystem m_BroadPhaseSystem;
	BlockMemoryAllocator* m_BlockAllocator;
	ContactCallbacks* m_ContactCallbacks;

	Contact* m_AllContacts;
	size_t m_NumberOfContacts;
};