#include "Engine/PhysicsSystem/ContactSolver/Contacts/CapsuleVersusCapsuleContact.hpp"
#include "Engine/PhysicsSystem/CollisionShape/CollisionShape.hpp"
#include "Engine/PhysicsSystem/RigidBody/BodyFixture.hpp"
#include "Engine/DataStructures/BlockMemoryAllocator.hpp"



CapsuleVersusCapsuleContact::CapsuleVersusCapsuleContact(BodyFixture* firstFixture, BodyFixture* secondFixture) :
	Contact(firstFixture, secondFixture)
{
	ASSERT_OR_DIE(firstFixture->IsFixtureShapeOfType(CAPSULE_SHAPE), "Shape does not match for contact.");
	ASSERT_OR_DIE(secondFixture->IsFixtureShapeOfType(CAPSULE_SHAPE), "Shape does not match for contact.");
}



CapsuleVersusCapsuleContact::~CapsuleVersusCapsuleContact()
{

}



Contact* CapsuleVersusCapsuleContact::CreateContact(BodyFixture* firstFixture, BodyFixture* secondFixture, BlockMemoryAllocator* blockAllocator)
{
	void* allocatedMemory = blockAllocator->AllocateBlockMemory(sizeof(CapsuleVersusCapsuleContact));

	return new(allocatedMemory) CapsuleVersusCapsuleContact(firstFixture, secondFixture);
}



void CapsuleVersusCapsuleContact::DestroyContact(Contact* currentContact, BlockMemoryAllocator* blockAllocator)
{
	CapsuleVersusCapsuleContact* cvcContact = static_cast<CapsuleVersusCapsuleContact*>(currentContact);
	cvcContact->~CapsuleVersusCapsuleContact();
	blockAllocator->FreeBlockMemory(cvcContact, sizeof(CapsuleVersusCapsuleContact));
}



void CapsuleVersusCapsuleContact::GenerateLocalContactCluster(LocalContactCluster* localContactCluster, const Transform2D& firstFixtureTransform, const Transform2D& secondFixtureTransform)
{
	CapsuleShape* firstFixtureShape = (CapsuleShape*)(GetFirstFixture()->GetFixtureShape());
	CapsuleShape* secondFixtureShape = (CapsuleShape*)(GetSecondFixture()->GetFixtureShape());

	GenerateContactClusterForCapsuleVersusCapsule(localContactCluster, firstFixtureShape, firstFixtureTransform, secondFixtureShape, secondFixtureTransform);
}