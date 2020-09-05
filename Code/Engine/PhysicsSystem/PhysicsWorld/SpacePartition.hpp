#pragma once

#include "Engine/PhysicsSystem/General/PhysicsCommons.hpp"



class RigidBody;
class Contact;
class ContactCallbacks;
class StackMemoryAllocator;



class SpacePartition
{
public:
	SpacePartition(StackMemoryAllocator* stackAllocator, ContactCallbacks* contactCallbacks, size_t maximumNumberOfBodies, size_t maximumNumberOfContacts);
	~SpacePartition();

	void AddBody(RigidBody* currentBody);
	void AddContact(Contact* currentContact);

	void ResolvePhysics(float deltaTimeInSeconds, float worldGravity);
	void ResolveTimeOfImpactPhysics(float deltaTimeInSeconds, int32_t firstSpacePartitionID, int32_t secondSpacePartitionID);

	void ResetPartition();

public:
	StackMemoryAllocator* m_StackAllocator;
	ContactCallbacks* m_ContactCallbacks;

	RigidBody** m_AllBodies;
	Contact** m_AllContacts;

	Vector2D* m_AllPositions;
	float* m_AllRotations;

	Vector2D* m_AllLinearVelocities;
	float* m_AllAngularVelocities;

	size_t m_NumberOfBodies;
	size_t m_MaximumNumberOfBodies;

	size_t m_NumberOfContacts;
	size_t m_MaximumNumberOfContacts;
};