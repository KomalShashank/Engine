#include "Engine/PhysicsSystem/ContactSolver/Contacts/CircleVersusPolygonContact.hpp"
#include "Engine/PhysicsSystem/CollisionShape/CollisionShape.hpp"
#include "Engine/PhysicsSystem/RigidBody/BodyFixture.hpp"
#include "Engine/DataStructures/BlockMemoryAllocator.hpp"



CircleVersusPolygonContact::CircleVersusPolygonContact(BodyFixture* firstFixture, BodyFixture* secondFixture) :
	Contact(firstFixture, secondFixture)
{
	ASSERT_OR_DIE(firstFixture->IsFixtureShapeOfType(CIRCLE_SHAPE), "Shape does not match for contact.");
	ASSERT_OR_DIE(secondFixture->IsFixtureShapeOfType(POLYGON_SHAPE), "Shape does not match for contact.");
}



CircleVersusPolygonContact::~CircleVersusPolygonContact()
{

}



Contact* CircleVersusPolygonContact::CreateContact(BodyFixture* firstFixture, BodyFixture* secondFixture, BlockMemoryAllocator* blockAllocator)
{
	void* allocatedMemory = blockAllocator->AllocateBlockMemory(sizeof(CircleVersusPolygonContact));

	return new(allocatedMemory) CircleVersusPolygonContact(firstFixture, secondFixture);
}



void CircleVersusPolygonContact::DestroyContact(Contact* currentContact, BlockMemoryAllocator* blockAllocator)
{
	CircleVersusPolygonContact* cvpContact = static_cast<CircleVersusPolygonContact*>(currentContact);
	cvpContact->~CircleVersusPolygonContact();
	blockAllocator->FreeBlockMemory(cvpContact, sizeof(CircleVersusPolygonContact));
}



void CircleVersusPolygonContact::GenerateLocalContactCluster(LocalContactCluster* localContactCluster, const Transform2D& firstFixtureTransform, const Transform2D& secondFixtureTransform)
{
	CircleShape* firstFixtureShape = (CircleShape*)(GetFirstFixture()->GetFixtureShape());
	PolygonShape* secondFixtureShape = (PolygonShape*)(GetSecondFixture()->GetFixtureShape());

	GenerateContactClusterForCircleVersusPolygon(localContactCluster, firstFixtureShape, firstFixtureTransform, secondFixtureShape, secondFixtureTransform);
}