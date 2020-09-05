#pragma once

#include "Engine/PhysicsSystem/General/PhysicsCommons.hpp"
#include "Engine/PhysicsSystem/ContactSolver/Contacts/Contact.hpp"



class CapsuleVersusPolygonContact final : public Contact
{
private:
	CapsuleVersusPolygonContact(BodyFixture* firstFixture, BodyFixture* secondFixture);
	~CapsuleVersusPolygonContact();

public:
	static Contact* CreateContact(BodyFixture* firstFixture, BodyFixture* secondFixture, BlockMemoryAllocator* blockAllocator);
	static void DestroyContact(Contact* currentContact, BlockMemoryAllocator* blockAllocator);

	void GenerateLocalContactCluster(LocalContactCluster* localContactCluster, const Transform2D& firstFixtureTransform, const Transform2D& secondFixtureTransform) override;
};