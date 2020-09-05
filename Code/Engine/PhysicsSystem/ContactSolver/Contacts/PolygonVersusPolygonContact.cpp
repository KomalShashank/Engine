#include "Engine/PhysicsSystem/ContactSolver/Contacts/PolygonVersusPolygonContact.hpp"
#include "Engine/PhysicsSystem/CollisionShape/CollisionShape.hpp"
#include "Engine/PhysicsSystem/RigidBody/BodyFixture.hpp"
#include "Engine/DataStructures/BlockMemoryAllocator.hpp"



PolygonVersusPolygonContact::PolygonVersusPolygonContact(BodyFixture* firstFixture, BodyFixture* secondFixture) :
	Contact(firstFixture, secondFixture)
{
	ASSERT_OR_DIE(firstFixture->IsFixtureShapeOfType(POLYGON_SHAPE), "Shape does not match for contact.");
	ASSERT_OR_DIE(secondFixture->IsFixtureShapeOfType(POLYGON_SHAPE), "Shape does not match for contact.");
}



PolygonVersusPolygonContact::~PolygonVersusPolygonContact()
{

}



Contact* PolygonVersusPolygonContact::CreateContact(BodyFixture* firstFixture, BodyFixture* secondFixture, BlockMemoryAllocator* blockAllocator)
{
	void* allocatedMemory = blockAllocator->AllocateBlockMemory(sizeof(PolygonVersusPolygonContact));

	return new(allocatedMemory) PolygonVersusPolygonContact(firstFixture, secondFixture);
}



void PolygonVersusPolygonContact::DestroyContact(Contact* currentContact, BlockMemoryAllocator* blockAllocator)
{
	PolygonVersusPolygonContact* pvpContact = static_cast<PolygonVersusPolygonContact*>(currentContact);
	pvpContact->~PolygonVersusPolygonContact();
	blockAllocator->FreeBlockMemory(pvpContact, sizeof(PolygonVersusPolygonContact));
}



void PolygonVersusPolygonContact::GenerateLocalContactCluster(LocalContactCluster* localContactCluster, const Transform2D& firstFixtureTransform, const Transform2D& secondFixtureTransform)
{
	PolygonShape* firstFixtureShape = (PolygonShape*)(GetFirstFixture()->GetFixtureShape());
	PolygonShape* secondFixtureShape = (PolygonShape*)(GetSecondFixture()->GetFixtureShape());

	GenerateContactClusterForPolygonVersusPolygon(localContactCluster, firstFixtureShape, firstFixtureTransform, secondFixtureShape, secondFixtureTransform);
}