#include "Engine/PhysicsSystem/ContactSolver/Contacts/CapsuleVersusPolygonContact.hpp"
#include "Engine/PhysicsSystem/CollisionShape/CollisionShape.hpp"
#include "Engine/PhysicsSystem/RigidBody/BodyFixture.hpp"
#include "Engine/DataStructures/BlockMemoryAllocator.hpp"



CapsuleVersusPolygonContact::CapsuleVersusPolygonContact(BodyFixture* firstFixture, BodyFixture* secondFixture) :
	Contact(firstFixture, secondFixture)
{
	ASSERT_OR_DIE(firstFixture->IsFixtureShapeOfType(CAPSULE_SHAPE), "Shape does not match for contact.");
	ASSERT_OR_DIE(secondFixture->IsFixtureShapeOfType(POLYGON_SHAPE), "Shape does not match for contact.");
}



CapsuleVersusPolygonContact::~CapsuleVersusPolygonContact()
{

}



Contact* CapsuleVersusPolygonContact::CreateContact(BodyFixture* firstFixture, BodyFixture* secondFixture, BlockMemoryAllocator* blockAllocator)
{
	void* allocatedMemory = blockAllocator->AllocateBlockMemory(sizeof(CapsuleVersusPolygonContact));

	return new(allocatedMemory) CapsuleVersusPolygonContact(firstFixture, secondFixture);
}



void CapsuleVersusPolygonContact::DestroyContact(Contact* currentContact, BlockMemoryAllocator* blockAllocator)
{
	CapsuleVersusPolygonContact* cvpContact = static_cast<CapsuleVersusPolygonContact*>(currentContact);
	cvpContact->~CapsuleVersusPolygonContact();
	blockAllocator->FreeBlockMemory(cvpContact, sizeof(CapsuleVersusPolygonContact));
}



void CapsuleVersusPolygonContact::GenerateLocalContactCluster(LocalContactCluster* localContactCluster, const Transform2D& firstFixtureTransform, const Transform2D& secondFixtureTransform)
{
	CapsuleShape* firstFixtureShape = (CapsuleShape*)(GetFirstFixture()->GetFixtureShape());
	PolygonShape* secondFixtureShape = (PolygonShape*)(GetSecondFixture()->GetFixtureShape());

	GenerateContactClusterForCapsuleVersusPolygon(localContactCluster, firstFixtureShape, firstFixtureTransform, secondFixtureShape, secondFixtureTransform);
}