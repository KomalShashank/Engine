#pragma once

#include "Engine/PhysicsSystem/General/PhysicsCommons.hpp"
#include "Engine/PhysicsSystem/CollisionShape/CollisionShape.hpp"
#include "Engine/PhysicsSystem/CollisionDetection/NarrowPhaseCollision.hpp"



class Contact;
class BodyFixture;
class RigidBody;
class BlockMemoryAllocator;



typedef Contact* (ContactCreator)(BodyFixture* firstFixture, BodyFixture* secondFixture, BlockMemoryAllocator* blockAllocator);
typedef void (ContactDestroyer)(Contact* currentContact, BlockMemoryAllocator* blockAllocator);



struct ContactFunctions
{
	ContactCreator* m_Creator;
	ContactDestroyer* m_Destroyer;
	uint8_t m_FirstShapeType;
	uint8_t m_SecondShapeType;
};



class ContactNode
{
public:
	ContactNode() :
		m_OtherBody(nullptr),
		m_Contact(nullptr),
		m_PreviousNode(nullptr),
		m_NextNode(nullptr)
	{

	}

public:
	RigidBody* m_OtherBody;
	Contact* m_Contact;
	ContactNode* m_PreviousNode;
	ContactNode* m_NextNode;
};



class ContactCallbacks
{
public:
	ContactCallbacks();
	virtual ~ContactCallbacks();

	virtual void OnContactStarted(Contact*);
	virtual void OnContactEnded(Contact*);

	virtual void OnSimulationStart(Contact*, const LocalContactCluster*);
	virtual void OnSimulationEnd(Contact*, float*, float*, size_t);
};



class Contact
{
protected:
	Contact();
	Contact(BodyFixture* firstFixture, BodyFixture* secondFixture);
	virtual ~Contact();

	float CombineCoefficientsOfFriction(float firstCoefficientOfFriction, float secondCoefficientOfFriction);
	float CombineCoefficientsOfRestitution(float firstCoefficientOfRestitution, float secondCoefficientOfRestitution);

	static void InitializeContactFunctions();
	static void AddContactFunction(ContactCreator* contactCreator, ContactDestroyer* contactDestroyer, uint8_t firstShapeType, uint8_t secondShapeType);

public:
	static Contact* CreateContact(BodyFixture* firstFixture, BodyFixture* secondFixture, BlockMemoryAllocator* blockAllocator);
	static void DestroyContact(Contact* currentContact, BlockMemoryAllocator* blockAllocator);

	void UpdateContact(ContactCallbacks* contactCallbacks);
	virtual void GenerateLocalContactCluster(LocalContactCluster* localContactCluster, const Transform2D& firstFixtureTransform, const Transform2D& secondFixtureTransform) = 0;

	void GetGlobalContactCluster(GlobalContactCluster* globalContactCluster) const;

	LocalContactCluster* GetLocalContactCluster();
	const LocalContactCluster* GetLocalContactCluster() const;

	void SetPreviousContact(Contact* previousContact);
	Contact* GetPreviousContact();
	const Contact* GetPreviousContact() const;

	void SetNextContact(Contact* nextContact);
	Contact* GetNextContact();
	const Contact* GetNextContact() const;

	ContactNode& GetFirstContactNode();
	ContactNode& GetSecondContactNode();

	BodyFixture* GetFirstFixture();
	const BodyFixture* GetFirstFixture() const;

	BodyFixture* GetSecondFixture();
	const BodyFixture* GetSecondFixture() const;

	void SetCoefficientOfFriction(float coefficientOfFriction);
	float GetCoefficientOfFriction() const;
	void ResetCoefficientOfFriction();

	void SetCoefficientOfRestitution(float coefficientOfRestitution);
	float GetCoefficientOfRestitution() const;
	void ResetCoefficientOfRestitution();

	void SetTangentialSpeed(float tangentialSpeed);
	float GetTangentialSpeed() const;

	void SetTimeOfImpactDuration(float timeOfImpactDuration);
	float GetTimeOfImpactDuration() const;

	void SetNumberOfTimesOfImpact(size_t numberOfTimesOfImpact);
	size_t GetNumberOfTimesOfImpact() const;

	void SetContactEnabled(bool enabled);
	bool IsContactEnabled() const;

	void SetFixturesInContact(bool fixturesInContact);
	bool AreFixturesInContact() const;

	void SetContactPartOfSpacePartition(bool partOfSpacePartition);
	bool IsContactPartOfSpacePartition() const;

	void SetValidTimeOfImpact(bool validTimeOfImpact);
	bool DoesHaveAValidTimeOfImpact() const;

private:
	enum
	{
		ENABLED_FLAG = 0x0001,
		IN_CONTACT_FLAG = 0x0002,
		PART_OF_SPACE_PARTITION_FLAG = 0x0004,
		VALID_TIME_OF_IMPACT_FLAG = 0x0008
	};

	static ContactFunctions s_ContactFunctionRegisty[NUMBER_OF_SHAPE_TYPES][NUMBER_OF_SHAPE_TYPES];
	static bool s_FunctionsInitialized;

	LocalContactCluster m_LocalContactCluster;

	Contact* m_PreviousContact;
	Contact* m_NextContact;

	ContactNode m_FirstContactNode;
	ContactNode m_SecondContactNode;

	BodyFixture* m_FirstFixture;
	BodyFixture* m_SecondFixture;

	float m_CoefficientOfFriction;
	float m_CoefficientOfRestitution;
	float m_TangentialSpeed;

	float m_TimeOfImpactDuration;
	size_t m_NumberOfTimesOfImpact;

	uint8_t m_ContactFlags;
};