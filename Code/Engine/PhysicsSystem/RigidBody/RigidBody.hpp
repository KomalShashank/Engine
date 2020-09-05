#pragma once

#include "Engine/PhysicsSystem/General/PhysicsCommons.hpp"



enum BodyType : uint8_t
{
	STATIC_BODY,
	KINEMATIC_BODY,
	DYNAMIC_BODY,
	NUMBER_OF_BODY_TYPES,
	INVALID_BODY_TYPE = 255U
};



class PhysicsWorld;
class BodyFixture;
class BodyFixtureData;
class ContactNode;
class BroadPhaseSystem;
class BlockMemoryAllocator;



class BodySweptShape
{
public:
	BodySweptShape() :
		m_LocalCenter(Vector2D::ZERO),
		m_PreviousWorldPosition(Vector2D::ZERO),
		m_NextWorldPosition(Vector2D::ZERO),
		m_PreviousDelta(0.0f),
		m_PreviousWorldRotation(0.0f),
		m_NextWorldRotation(0.0f)
	{

	}

	Transform2D GetLocalTransformAtDelta(float delta) const
	{
		Vector2D positionAtDelta = (m_PreviousWorldPosition * (1.0f - delta)) + (m_NextWorldPosition * delta);
		float rotationAtDelta = (m_PreviousWorldRotation * (1.0f - delta)) + (m_NextWorldRotation * delta);

		Transform2D transformAtDelta;
		transformAtDelta.m_Rotation.SetRotator2D(rotationAtDelta);
		transformAtDelta.m_Position = positionAtDelta;
		transformAtDelta.m_Position -= Multiply(transformAtDelta.m_Rotation, m_LocalCenter);

		return transformAtDelta;
	}

	void AdvanceToDelta(float delta)
	{
		ASSERT_OR_DIE(delta < 1.0f, "Delta is out of range.");
		float advanceDelta = (delta - m_PreviousDelta) / (1.0f - m_PreviousDelta);
		m_PreviousWorldPosition += (m_NextWorldPosition - m_PreviousWorldPosition) * advanceDelta;
		m_PreviousWorldRotation += (m_NextWorldRotation - m_PreviousWorldRotation) * advanceDelta;
		m_PreviousDelta = delta;
	}

	void NormalizeAngles()
	{
		float difference = 360.0f * FloorOfValue(m_PreviousWorldRotation / 360.0f);
		m_PreviousWorldRotation -= difference;
		m_NextWorldRotation -= difference;
	}

public:
	Vector2D m_LocalCenter;
	Vector2D m_PreviousWorldPosition;
	Vector2D m_NextWorldPosition;

	float m_PreviousDelta;
	float m_PreviousWorldRotation;
	float m_NextWorldRotation;
};



class BodyMassInfo
{
public:
	BodyMassInfo() :
		m_Mass(0.0f),
		m_InverseMass(0.0f),
		m_MomentOfInertia(0.0f),
		m_InverseMomentOfInertia(0.0f),
		m_CenterOfMass(Vector2D::ZERO)
	{

	}

public:
	float m_Mass;
	float m_InverseMass;

	float m_MomentOfInertia;
	float m_InverseMomentOfInertia;

	Vector2D m_CenterOfMass;
};



class RigidBodyData
{
public:
	RigidBodyData();

	void SetAwake(bool awake);
	void SetActive(bool active);
	void SetHasNoRotation(bool hasNoRotation);
	void SetUsingCCD(bool usingCCD);

public:
	Transform2D m_WorldTransform;

	Vector2D m_LinearVelocity;
	float m_AngularVelocity;

	float m_LinearDamping;
	float m_AngularDamping;

	uint8_t m_BodyType;
	uint8_t m_BodyFlags;
};



class RigidBody
{
private:
	RigidBody();
	~RigidBody();

	void RenderAllFixtures() const;
	void RenderAllFixtureAABBs(const BroadPhaseSystem* broadPhaseSystem) const;

public:
	static RigidBody* CreateBody(BlockMemoryAllocator* blockAllocator, const RigidBodyData& bodyData, PhysicsWorld* parentWorld);
	static void DestroyBody(BlockMemoryAllocator* blockAllocator, RigidBody* currentBody);

	BodyFixture* CreateBodyFixture(const BodyFixtureData* fixtureData);
	void DestroyBodyFixture(BodyFixture* currentFixture);

	void SynchronizeAllBodyFixtures();
	void UpdateBodyTransform();
	void AdvanceBodyToDelta(float delta);

	void ApplyForceToBody(const Vector2D& appliedForce);
	void ApplyForceToBodyAtPosition(const Vector2D& appliedForce, const Vector2D& appliedPosition);
	void ApplyTorqueToBody(float appliedTorque);
	void ResetAllForces();

	void ApplyLinearImpulseToBody(const Vector2D& appliedLinearImpulse);
	void ApplyLinearImpulseToBodyAtPosition(const Vector2D& appliedLinearImpulse, const Vector2D& appliedPosition);
	void ApplyAngularImpulse(float appliedAngularImpulse);

	Vector2D GetWorldCenter() const;
	Vector2D GetLocalCenter() const;

	void SetWorldTransform2D(const Vector2D& transformPosition, float transformRotation);
	Transform2D GetWorldTransform2D() const;

	Vector2D GetWorldPosition() const;
	float GetWorldRotation() const;

	void SetLinearVelocity(const Vector2D& linearVelocity);
	Vector2D GetLinearVelocity() const;

	void SetAngularVelocity(float angularVelocity);
	float GetAngularVelocity() const;

	void SetLinearDamping(float linearDamping);
	float GetLinearDamping() const;

	void SetAngularDamping(float angularDamping);
	float GetAngularDamping() const;

	void SetBodyAwake(bool awake);
	bool IsBodyAwake() const;

	void SetBodyActive(bool active);
	bool IsBodyActive() const;

	void SetNoRotation(bool noRotation);
	bool HasNoRotation() const;

	void UseCCD(bool usingCCD);
	bool IsUsingCCD() const;

	void SetBodyType(uint8_t currentBodyType);
	bool IsOfType(uint8_t currentBodyType);

	BodyMassInfo GetBodyMassInfo() const;
	void RecalculateBodyMassInfo();

	float GetMass() const;
	float GetInverseMass() const;
	float GetLocalMomentOfInertia() const;
	float GetInverseLocalMomentOfInertia() const;

	PhysicsWorld* GetParentWorld();
	const PhysicsWorld* GetParentWorld() const;

	void SetPreviousBody(RigidBody* previousBody);
	RigidBody* GetPreviousBody();
	const RigidBody* GetPreviousBody() const;

	void SetNextBody(RigidBody* nextBody);
	RigidBody* GetNextBody();
	const RigidBody* GetNextBody() const;

	BodyFixture* GetFixturesList();
	const BodyFixture* GetFixturesList() const;
	size_t GetNumberOfFixtures() const;

	void SetContactNodeList(ContactNode* contactNodes);
	ContactNode* GetContactNodeList();
	const ContactNode* GetContactNodeList() const;

	Vector2D GetAccumulatedNetForce() const;
	float GetAccumulatedNetTorque() const;

	void SetSleepDuration(float sleepDuration);
	float GetSleepDuration() const;

	void SetSpacePartitionID(int32_t spacePartitionID);
	int32_t GetSpacePartitionID() const;

	void SetBodyPartOfSpacePartition(bool partOfSpacePartition);
	bool IsBodyPartOfSpacePartition() const;

	static bool CanBodiesCollide(RigidBody* firstBody, RigidBody* secondBody);

	void RenderBody() const;
	void RenderBodyAABBs(const BroadPhaseSystem* broadPhaseSystem) const;

public:
	BodySweptShape m_SweptShape;

private:
	RigidBodyData m_BodyData;
	BodyMassInfo m_BodyMassInfo;

	PhysicsWorld* m_ParentWorld;
	RigidBody* m_PreviousBody;
	RigidBody* m_NextBody;

	BodyFixture* m_BodyFixtures;
	size_t m_NumberOfFixtures;

	ContactNode* m_AllContactNodes;

	Vector2D m_AccumulatedNetForce;
	float m_AccumulatedNetTorque;

	float m_SleepDuration;
	int32_t m_SpacePartitionID;
};