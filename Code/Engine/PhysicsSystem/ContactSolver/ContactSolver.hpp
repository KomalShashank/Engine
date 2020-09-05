#pragma once

#include "Engine/PhysicsSystem/General/PhysicsCommons.hpp"



class Contact;
class SpacePartition;
class StackMemoryAllocator;



struct ConstraintPoint
{
	Vector2D m_FirstPoint;
	Vector2D m_SecondPoint;

	float m_NormalImpulse;
	float m_TangentImpulse;

	float m_NormalMass;
	float m_TangentMass;

	float m_VelocityBias;
};



struct PositionalConstraint
{
	Vector2D m_ContactPoints[MAXIMUM_NUMBER_OF_CONTACT_POINTS];
	Vector2D m_LocalReferencePoint;
	Vector2D m_LocalReferenceNormal;

	Vector2D m_FirstBodyLocalCenter;
	Vector2D m_SecondBodyLocalCenter;

	float m_FirstBodyInverseMass;
	float m_SecondBodyInverseMass;

	float m_FirstBodyInverseMomentOfInertia;
	float m_SecondBodyInverseMomentOfInertia;

	float m_FirstBoundingRadius;
	float m_SecondBoundingRadius;

	int32_t m_FirstBodySpacePartitionID;
	int32_t m_SecondBodySpacePartitionID;

	size_t m_NumberOfContactPoints;
	uint8_t m_ClusterType;
};



struct VelocityConstraint
{
	ConstraintPoint m_ConstraintPoints[MAXIMUM_NUMBER_OF_CONTACT_POINTS];

	Matrix2x2 m_NormalMass;
	Matrix2x2 m_InverseNormalMass;

	Vector2D m_Normal;

	float m_FirstBodyInverseMass;
	float m_SecondBodyInverseMass;

	float m_FirstBodyInverseMomentOfInertia;
	float m_SecondBodyInverseMomentOfInertia;

	float m_CoefficientOfFriction;
	float m_CoefficientOfRestitution;
	float m_TangentialSpeed;

	int32_t m_FirstBodySpacePartitionID;
	int32_t m_SecondBodySpacePartitionID;

	size_t m_ContactIndex;
	size_t m_NumberOfContactPoints;
};



class ContactSolverData
{
public:
	ContactSolverData() :
		m_StackAllocator(nullptr),
		m_AllPositions(nullptr),
		m_AllRotations(nullptr),
		m_AllLinearVelocities(nullptr),
		m_AllAngularVelocities(nullptr),
		m_AllContacts(nullptr),
		m_NumberOfContacts(0U)
	{

	}

public:
	StackMemoryAllocator* m_StackAllocator;

	Vector2D* m_AllPositions;
	float* m_AllRotations;

	Vector2D* m_AllLinearVelocities;
	float* m_AllAngularVelocities;

	Contact** m_AllContacts;
	size_t m_NumberOfContacts;
};



class ContactSolver
{
public:
	ContactSolver(const ContactSolverData& solverData);
	~ContactSolver();

	void InitializePositions(bool usingWarmStarting);
	void InitializeVelocities();

	void WarmStartSolver();
	void SaveContactImpulses();
	void RunContactCallbacks(SpacePartition* spacePartition);

	bool ResolvePositionalConstraints();
	bool ResolveTimeOfImpactPositionalConstraints(int32_t firstSpacePartitionID, int32_t secondSpacePartitionID);
	void ResolveVelocityConstraints();

private:
	void GetContactValues(PositionalConstraint* positionalConstraint,
							const Transform2D& firstBodyTransform,
							const Transform2D& secondBodyTransform,
							size_t contactIndex,
							Vector2D& normalVector,
							Vector2D& contactPoint,
							float& separationDistance);

public:
	ContactSolverData m_SolverData;

	PositionalConstraint* m_AllPositionalConstraints;
	VelocityConstraint* m_AllVelocityConstraints;
};