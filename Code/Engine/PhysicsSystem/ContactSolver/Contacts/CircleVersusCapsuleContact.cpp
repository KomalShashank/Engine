#include "Engine/PhysicsSystem/ContactSolver/Contacts/CircleVersusCapsuleContact.hpp"
#include "Engine/PhysicsSystem/CollisionShape/CollisionShape.hpp"
#include "Engine/PhysicsSystem/RigidBody/BodyFixture.hpp"
#include "Engine/DataStructures/BlockMemoryAllocator.hpp"



CircleVersusCapsuleContact::CircleVersusCapsuleContact(BodyFixture* firstFixture, BodyFixture* secondFixture) :
	Contact(firstFixture, secondFixture)
{
	ASSERT_OR_DIE(firstFixture->IsFixtureShapeOfType(CIRCLE_SHAPE), "Shape does not match for contact.");
	ASSERT_OR_DIE(secondFixture->IsFixtureShapeOfType(CAPSULE_SHAPE), "Shape does not match for contact.");
}



CircleVersusCapsuleContact::~CircleVersusCapsuleContact()
{

}



Contact* CircleVersusCapsuleContact::CreateContact(BodyFixture* firstFixture, BodyFixture* secondFixture, BlockMemoryAllocator* blockAllocator)
{
	void* allocatedMemory = blockAllocator->AllocateBlockMemory(sizeof(CircleVersusCapsuleContact));

	return new(allocatedMemory) CircleVersusCapsuleContact(firstFixture, secondFixture);
}



void CircleVersusCapsuleContact::DestroyContact(Contact* currentContact, BlockMemoryAllocator* blockAllocator)
{
	CircleVersusCapsuleContact* cvcContact = static_cast<CircleVersusCapsuleContact*>(currentContact);
	cvcContact->~CircleVersusCapsuleContact();
	blockAllocator->FreeBlockMemory(cvcContact, sizeof(CircleVersusCapsuleContact));
}



void CircleVersusCapsuleContact::GenerateLocalContactCluster(LocalContactCluster* localContactCluster, const Transform2D& firstFixtureTransform, const Transform2D& secondFixtureTransform)
{
	CircleShape* firstFixtureShape = (CircleShape*)(GetFirstFixture()->GetFixtureShape());
	CapsuleShape* secondFixtureShape = (CapsuleShape*)(GetSecondFixture()->GetFixtureShape());

	GenerateContactClusterForCircleVersusCapsule(localContactCluster, firstFixtureShape, firstFixtureTransform, secondFixtureShape, secondFixtureTransform);
}