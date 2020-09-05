#include "Engine/PhysicsSystem/ContactSolver/Contacts/CircleVersusCircleContact.hpp"
#include "Engine/PhysicsSystem/CollisionShape/CollisionShape.hpp"
#include "Engine/PhysicsSystem/RigidBody/BodyFixture.hpp"
#include "Engine/DataStructures/BlockMemoryAllocator.hpp"



CircleVersusCircleContact::CircleVersusCircleContact(BodyFixture* firstFixture, BodyFixture* secondFixture) :
	Contact(firstFixture, secondFixture)
{
	ASSERT_OR_DIE(firstFixture->IsFixtureShapeOfType(CIRCLE_SHAPE), "Shape does not match for contact.");
	ASSERT_OR_DIE(secondFixture->IsFixtureShapeOfType(CIRCLE_SHAPE), "Shape does not match for contact.");
}



CircleVersusCircleContact::~CircleVersusCircleContact()
{

}



Contact* CircleVersusCircleContact::CreateContact(BodyFixture* firstFixture, BodyFixture* secondFixture, BlockMemoryAllocator* blockAllocator)
{
	void* allocatedMemory = blockAllocator->AllocateBlockMemory(sizeof(CircleVersusCircleContact));

	return new(allocatedMemory) CircleVersusCircleContact(firstFixture, secondFixture);
}



void CircleVersusCircleContact::DestroyContact(Contact* currentContact, BlockMemoryAllocator* blockAllocator)
{
	CircleVersusCircleContact* cvcContact = static_cast<CircleVersusCircleContact*>(currentContact);
	cvcContact->~CircleVersusCircleContact();
	blockAllocator->FreeBlockMemory(cvcContact, sizeof(CircleVersusCircleContact));
}



void CircleVersusCircleContact::GenerateLocalContactCluster(LocalContactCluster* localContactCluster, const Transform2D& firstFixtureTransform, const Transform2D& secondFixtureTransform)
{
	CircleShape* firstFixtureShape = (CircleShape*)(GetFirstFixture()->GetFixtureShape());
	CircleShape* secondFixtureShape = (CircleShape*)(GetSecondFixture()->GetFixtureShape());

	GenerateContactClusterForCircleVersusCircle(localContactCluster, firstFixtureShape, firstFixtureTransform, secondFixtureShape, secondFixtureTransform);
}