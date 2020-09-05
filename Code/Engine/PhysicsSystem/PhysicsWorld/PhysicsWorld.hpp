#pragma once

#include "Engine/PhysicsSystem/General/PhysicsCommons.hpp"
#include "Engine/PhysicsSystem/ContactSolver/WorldContactHandler.hpp"
#include "Engine/DataStructures/BlockMemoryAllocator.hpp"
#include "Engine/DataStructures/StackMemoryAllocator.hpp"



class RigidBody;
class RigidBodyData;



class PhysicsWorld
{
private:
	PhysicsWorld(float deltaTimeConstant, float worldGravity);
	~PhysicsWorld();

	void ResolvePhysics();
	void ResolveTimeOfImpactPhysics();
	void ResetForcesOnAllBodies();

	void ToggleAABBs();

	void RenderAllBodies() const;
	void RenderAllBodyAABBs(const BroadPhaseSystem* broadPhaseSystem) const;

public:
	static void InitializePhysicsWorld();
	static void InitializePhysicsWorld(float deltaTimeConstant, float worldGravity);
	static void UninitializePhysicsWorld();

	static PhysicsWorld* SingletonInstance();

	void SimulateWorld();
	void RenderWorld() const;

	void SetContactCallbacks(ContactCallbacks* contactCallbacks);

	RigidBody* CreateRigidBody(const RigidBodyData* bodyData);
	void DestroyRigidBody(RigidBody* currentBody);

	RigidBody* GetBodiesList();
	const RigidBody* GetBodiesList() const;
	size_t GetNumberOfBodies() const;

	void SetWorldGravity(float worldGravity);
	float GetWorldGravity() const;

	void SetWorldLocked(bool locked);
	bool IsWorldLocked() const;

	void SetNewFixturesCreated(bool created);
	bool HaveNewFixturesBeenCreated() const;

public:
	BlockMemoryAllocator m_BlockAllocator;
	StackMemoryAllocator m_StackAllocator;

	WorldContactHandler m_ContactHandler;

private:
	RigidBody* m_RigidBodies;
	size_t m_NumberOfBodies;

	float m_DeltaTimeConstant;
	float m_InverseDeltaTimeConstant;
	float m_WorldGravity;

	bool m_WorldLocked;
	bool m_NewFixturesCreated;

	bool m_ShowAABBs;
};