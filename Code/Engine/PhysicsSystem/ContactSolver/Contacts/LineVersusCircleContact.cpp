#include "Engine/PhysicsSystem/ContactSolver/Contacts/LineVersusCircleContact.hpp"
#include "Engine/PhysicsSystem/CollisionShape/CollisionShape.hpp"
#include "Engine/PhysicsSystem/RigidBody/BodyFixture.hpp"
#include "Engine/DataStructures/BlockMemoryAllocator.hpp"



LineVersusCircleContact::LineVersusCircleContact(BodyFixture* firstFixture, BodyFixture* secondFixture) :
	Contact(firstFixture, secondFixture)
{
	ASSERT_OR_DIE(firstFixture->IsFixtureShapeOfType(LINE_SHAPE), "Shape does not match for contact.");
	ASSERT_OR_DIE(secondFixture->IsFixtureShapeOfType(CIRCLE_SHAPE), "Shape does not match for contact.");
}



LineVersusCircleContact::~LineVersusCircleContact()
{

}



Contact* LineVersusCircleContact::CreateContact(BodyFixture* firstFixture, BodyFixture* secondFixture, BlockMemoryAllocator* blockAllocator)
{
	void* allocatedMemory = blockAllocator->AllocateBlockMemory(sizeof(LineVersusCircleContact));

	return new(allocatedMemory) LineVersusCircleContact(firstFixture, secondFixture);
}



void LineVersusCircleContact::DestroyContact(Contact* currentContact, BlockMemoryAllocator* blockAllocator)
{
	LineVersusCircleContact* lvcContact = static_cast<LineVersusCircleContact*>(currentContact);
	lvcContact->~LineVersusCircleContact();
	blockAllocator->FreeBlockMemory(lvcContact, sizeof(LineVersusCircleContact));
}



void LineVersusCircleContact::GenerateLocalContactCluster(LocalContactCluster* localContactCluster, const Transform2D& firstFixtureTransform, const Transform2D& secondFixtureTransform)
{
	LineShape* firstFixtureShape = (LineShape*)(GetFirstFixture()->GetFixtureShape());
	CircleShape* secondFixtureShape = (CircleShape*)(GetSecondFixture()->GetFixtureShape());

	GenerateContactClusterForLineVersusCircle(localContactCluster, firstFixtureShape, firstFixtureTransform, secondFixtureShape, secondFixtureTransform);
}