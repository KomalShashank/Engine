#include "Engine/PhysicsSystem/ContactSolver/Contacts/LineVersusCapsuleContact.hpp"
#include "Engine/PhysicsSystem/CollisionShape/CollisionShape.hpp"
#include "Engine/PhysicsSystem/RigidBody/BodyFixture.hpp"
#include "Engine/DataStructures/BlockMemoryAllocator.hpp"



LineVersusCapsuleContact::LineVersusCapsuleContact(BodyFixture* firstFixture, BodyFixture* secondFixture) :
	Contact(firstFixture, secondFixture)
{
	ASSERT_OR_DIE(firstFixture->IsFixtureShapeOfType(LINE_SHAPE), "Shape does not match for contact.");
	ASSERT_OR_DIE(secondFixture->IsFixtureShapeOfType(CAPSULE_SHAPE), "Shape does not match for contact.");
}



LineVersusCapsuleContact::~LineVersusCapsuleContact()
{

}



Contact* LineVersusCapsuleContact::CreateContact(BodyFixture* firstFixture, BodyFixture* secondFixture, BlockMemoryAllocator* blockAllocator)
{
	void* allocatedMemory = blockAllocator->AllocateBlockMemory(sizeof(LineVersusCapsuleContact));

	return new(allocatedMemory) LineVersusCapsuleContact(firstFixture, secondFixture);
}



void LineVersusCapsuleContact::DestroyContact(Contact* currentContact, BlockMemoryAllocator* blockAllocator)
{
	LineVersusCapsuleContact* lvcContact = static_cast<LineVersusCapsuleContact*>(currentContact);
	lvcContact->~LineVersusCapsuleContact();
	blockAllocator->FreeBlockMemory(lvcContact, sizeof(LineVersusCapsuleContact));
}



void LineVersusCapsuleContact::GenerateLocalContactCluster(LocalContactCluster* localContactCluster, const Transform2D& firstFixtureTransform, const Transform2D& secondFixtureTransform)
{
	LineShape* firstFixtureShape = (LineShape*)(GetFirstFixture()->GetFixtureShape());
	CapsuleShape* secondFixtureShape = (CapsuleShape*)(GetSecondFixture()->GetFixtureShape());

	GenerateContactClusterForLineVersusCapsule(localContactCluster, firstFixtureShape, firstFixtureTransform, secondFixtureShape, secondFixtureTransform);
}