#pragma once

#include "Engine/PhysicsSystem/General/PhysicsCommons.hpp"
#include "Engine/PhysicsSystem/ContactSolver/Contacts/Contact.hpp"



class CapsuleVersusCapsuleContact final : public Contact
{
private:
	CapsuleVersusCapsuleContact(BodyFixture* firstFixture, BodyFixture* secondFixture);
	~CapsuleVersusCapsuleContact();

public:
	static Contact* CreateContact(BodyFixture* firstFixture, BodyFixture* secondFixture, BlockMemoryAllocator* blockAllocator);
	static void DestroyContact(Contact* currentContact, BlockMemoryAllocator* blockAllocator);

	void GenerateLocalContactCluster(LocalContactCluster* localContactCluster, const Transform2D& firstFixtureTransform, const Transform2D& secondFixtureTransform) override;
};