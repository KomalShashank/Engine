#include "Engine/PhysicsSystem/ContactSolver/Contacts/LineVersusPolygonContact.hpp"
#include "Engine/PhysicsSystem/CollisionShape/CollisionShape.hpp"
#include "Engine/PhysicsSystem/RigidBody/BodyFixture.hpp"
#include "Engine/DataStructures/BlockMemoryAllocator.hpp"



LineVersusPolygonContact::LineVersusPolygonContact(BodyFixture* firstFixture, BodyFixture* secondFixture) :
	Contact(firstFixture, secondFixture)
{
	ASSERT_OR_DIE(firstFixture->IsFixtureShapeOfType(LINE_SHAPE), "Shape does not match for contact.");
	ASSERT_OR_DIE(secondFixture->IsFixtureShapeOfType(POLYGON_SHAPE), "Shape does not match for contact.");
}



LineVersusPolygonContact::~LineVersusPolygonContact()
{

}



Contact* LineVersusPolygonContact::CreateContact(BodyFixture* firstFixture, BodyFixture* secondFixture, BlockMemoryAllocator* blockAllocator)
{
	void* allocatedMemory = blockAllocator->AllocateBlockMemory(sizeof(LineVersusPolygonContact));

	return new(allocatedMemory) LineVersusPolygonContact(firstFixture, secondFixture);
}



void LineVersusPolygonContact::DestroyContact(Contact* currentContact, BlockMemoryAllocator* blockAllocator)
{
	LineVersusPolygonContact* lvpContact = static_cast<LineVersusPolygonContact*>(currentContact);
	lvpContact->~LineVersusPolygonContact();
	blockAllocator->FreeBlockMemory(lvpContact, sizeof(LineVersusPolygonContact));
}



void LineVersusPolygonContact::GenerateLocalContactCluster(LocalContactCluster* localContactCluster, const Transform2D& firstFixtureTransform, const Transform2D& secondFixtureTransform)
{
	LineShape* firstFixtureShape = (LineShape*)(GetFirstFixture()->GetFixtureShape());
	PolygonShape* secondFixtureShape = (PolygonShape*)(GetSecondFixture()->GetFixtureShape());

	GenerateContactClusterForLineVersusPolygon(localContactCluster, firstFixtureShape, firstFixtureTransform, secondFixtureShape, secondFixtureTransform);
}