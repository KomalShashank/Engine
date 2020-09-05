#pragma once

#include "Engine/PhysicsSystem/General/PhysicsCommons.hpp"
#include "Engine/PhysicsSystem/ContactSolver/Contacts/Contact.hpp"



class LineVersusCapsuleContact final : public Contact
{
private:
	LineVersusCapsuleContact(BodyFixture* firstFixture, BodyFixture* secondFixture);
	~LineVersusCapsuleContact();

public:
	static Contact* CreateContact(BodyFixture* firstFixture, BodyFixture* secondFixture, BlockMemoryAllocator* blockAllocator);
	static void DestroyContact(Contact* currentContact, BlockMemoryAllocator* blockAllocator);

	void GenerateLocalContactCluster(LocalContactCluster* localContactCluster, const Transform2D& firstFixtureTransform, const Transform2D& secondFixtureTransform) override;
};