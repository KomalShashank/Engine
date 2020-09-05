#pragma once

#include "Engine/PhysicsSystem/General/PhysicsCommons.hpp"



class CollisionShape;
class RigidBody;
class BodyMassInfo;
class BroadPhaseSystem;
class BlockMemoryAllocator;
class RGBA;



class BodyFixtureData
{
public:
	BodyFixtureData() :
		m_Shape(nullptr),
		m_Density(0.0f),
		m_CoefficientOfFriction(0.0f),
		m_CoefficientOfRestitution(0.0f),
		m_OverlapOnly(false)
	{

	}

public:
	CollisionShape* m_Shape;

	float m_Density;
	float m_CoefficientOfFriction;
	float m_CoefficientOfRestitution;

	bool m_OverlapOnly;
};



struct FixtureReference
{
	AABB2D m_FixtureAABB;
	class BodyFixture* m_BodyFixture;
	int32_t m_FixtureID;
};



class BodyFixture
{
private:
	BodyFixture();
	~BodyFixture();

	void RenderFixtureShape(const Transform2D& shapeTransform) const;
	void GetShapeColorFromParentBodyState(RGBA* shapeColor) const;

public:
	static BodyFixture* CreateFixture(BlockMemoryAllocator* blockAllocator, const BodyFixtureData& fixtureData, RigidBody* parentBody);
	static void DestroyFixture(BlockMemoryAllocator* blockAllocator, BodyFixture* currentFixture);

	void AddToBroadPhaseSystem(BroadPhaseSystem* broadPhaseSystem, const Transform2D& fixtureTransform);
	void RemoveFromBroadPhaseSystem(BroadPhaseSystem* broadPhaseSystem);

	void SynchronizeFixture(BroadPhaseSystem* broadPhaseSystem, const Transform2D& firstTransform, const Transform2D& secondTransform);

	CollisionShape* GetFixtureShape();
	const CollisionShape* GetFixtureShape() const;
	bool IsFixtureShapeOfType(uint8_t currentShapeType) const;

	void SetDensity(float density);
	float GetDensity() const;

	void SetCoefficientOfFriction(float coefficientOfFriction);
	float GetCoefficientOfFriction() const;

	void SetCoefficientOfRestitution(float coefficientOfRestitution);
	float GetCoefficientOfRestitution() const;

	void SetOverlapOnly(bool overlapOnly);
	bool IsOverlapOnly() const;

	RigidBody* GetParentBody();
	const RigidBody* GetParentBody() const;

	void SetNextFixture(BodyFixture* nextFixture);
	BodyFixture* GetNextFixture();
	const BodyFixture* GetNextFixture() const;

	void CalculateMassInfo(BodyMassInfo* bodyMassInfo) const;
	bool IsPointInside(const Vector2D& testPoint) const;
	bool Raycast(const RaycastInput& raycastInput, RaycastResult& raycastResult) const;
	AABB2D GetAABB() const;
	int32_t GetFixtureID() const;

	void RenderFixture(const Transform2D& fixtureTransform) const;
	void RenderFixtureAABB(const BroadPhaseSystem* broadPhaseSystem) const;

private:
	BodyFixtureData m_FixtureData;
	FixtureReference m_FixtureReference;

	RigidBody* m_ParentBody;
	BodyFixture* m_NextFixture;
};