#pragma once

#include "Engine/PhysicsSystem/General/PhysicsCommons.hpp"
#include "Engine/DataStructures/ExtendableStack.hpp"

#include <algorithm>



struct DAT_Node
{
	AABB2D m_NodeAABB;
	void* m_NodeData;
	
	int32_t m_ParentNodeID;
	int32_t m_FirstChildNodeID;
	int32_t m_SecondChildNodeID;

	int32_t m_NodeHeight;
};



inline bool IsALeafNode(const DAT_Node& currentNode)
{
	return (currentNode.m_FirstChildNodeID == INVALID_ID &&
			currentNode.m_SecondChildNodeID == INVALID_ID &&
			currentNode.m_NodeHeight == 0);
}



class DynamicAABBTree
{
public:
	DynamicAABBTree();
	~DynamicAABBTree();

	int32_t AddFixtureToTree(const AABB2D& fixtureAABB, void* fixtureReference);
	void RemoveFixtureFromTree(int32_t currentFixtureID);

	bool MoveFixtureWithinTree(int32_t currentFixtureID, const AABB2D& fixtureAABB, const Vector2D& fixtureDisplacement);

	void* GetNodeData(int32_t currentFixtureID) const;
	AABB2D GetExtendedAABB(int32_t currentFixtureID) const;

	template <typename data_type>
	void QueryAABB(data_type* queryCallback, const AABB2D& fixtureAABB) const;

	template <typename data_type>
	void Raycast(data_type* raycastCallback, const RaycastInput& raycastInput) const;

private:
	int32_t AllocateNodeToTree();
	void DeallocateNodeFromTree(int32_t currentNodeID);

	void AddLeafNodeToTree(int32_t leafNodeID);
	void RemoveLeafNodeFromTree(int32_t leafNodeID);

	int32_t BalanceTree(int32_t currentNodeID);

private:
	DAT_Node* m_AllTreeNodes;
	size_t m_MaximumTreeSize;
	size_t m_NumberOfNodes;

	int32_t m_RootNodeID;
	int32_t m_FreeNodeIDList;
};



template <typename data_type>
inline void DynamicAABBTree::QueryAABB(data_type* queryCallback, const AABB2D& fixtureAABB) const
{
	ExtendableStack<int32_t, 256U> nodeStack;
	nodeStack.PushToStack(m_RootNodeID);

	while (nodeStack.GetSize() > 0U)
	{
		int32_t currentNodeID = nodeStack.PopFromStack();
		if (currentNodeID == INVALID_ID)
		{
			continue;
		}

		const DAT_Node* currentNode = m_AllTreeNodes + currentNodeID;

		if (AABB2D::AreAABBsIntersecting(currentNode->m_NodeAABB, fixtureAABB))
		{
			if (IsALeafNode(*currentNode))
			{
				bool continueQuerying = queryCallback->QueryCallback(currentNodeID);
				if (!continueQuerying)
				{
					return;
				}
			}
			else
			{
				nodeStack.PushToStack(currentNode->m_FirstChildNodeID);
				nodeStack.PushToStack(currentNode->m_SecondChildNodeID);
			}
		}
	}
}



template <typename data_type>
inline void DynamicAABBTree::Raycast(data_type* raycastCallback, const RaycastInput& raycastInput) const
{
	Vector2D startingPosition = raycastInput.m_StartingPosition;
	Vector2D endingPosition = raycastInput.m_EndingPosition;
	Vector2D rayVector = endingPosition - startingPosition;

	ASSERT_OR_DIE(rayVector.GetVector2DMagnitude() > 0.0f, "Ray does not exist.");
	rayVector = rayVector.GetNormalizedVector2D();

	Vector2D perpendicularRay = Vector2D(1.0f, rayVector);
	Vector2D absolutePerpendicularRay = Vector2D(perpendicularRay);

	float maximumImpactFraction = raycastInput.m_MaximumImpactFraction;

	AABB2D segmentAABB;
	Vector2D fractionedRay = startingPosition + (rayVector * maximumImpactFraction);
	segmentAABB.minimums = GetMinimum(startingPosition, fractionedRay);
	segmentAABB.maximums = GetMaximum(startingPosition, fractionedRay);

	ExtendableStack<int32_t, 256U> nodeStack;
	nodeStack.PushToStack(m_RootNodeID);

	while (nodeStack.GetSize() > 0U)
	{
		int32_t currentNodeID = nodeStack.PopFromStack();
		if (currentNodeID == INVALID_ID)
		{
			continue;
		}

		const DAT_Node* currentNode = m_AllTreeNodes + currentNodeID;

		if (!AABB2D::AreAABBsIntersecting(currentNode->m_NodeAABB, segmentAABB))
		{
			continue;
		}

		Vector2D aabbCenter = currentNode->m_NodeAABB.GetAABBCenter();
		Vector2D aabbHalfDimensions = currentNode->m_NodeAABB.GetHalfDimensions();

		float separationAmount = Absolute(Vector2D::DotProduct(perpendicularRay, startingPosition - aabbCenter)) - Vector2D::DotProduct(absolutePerpendicularRay, aabbHalfDimensions);
		if (separationAmount > 0.0f)
		{
			continue;
		}

		if (IsALeafNode(currentNode))
		{
			RaycastInput leafRaycastInput;
			leafRaycastInput.m_StartingPosition = raycastInput.m_StartingPosition;
			leafRaycastInput.m_EndingPosition = raycastInput.m_EndingPosition;
			leafRaycastInput.m_MaximumImpactFraction = maximumImpactFraction;

			float callbackResult = raycastCallback->RaycastCallback(leafRaycastInput, currentNodeID);
			if (callbackResult == 0.0f)
			{
				return;
			}
			else if (callbackResult > 0.0f)
			{
				maximumImpactFraction = callbackResult;
				Vector2D newFractionedRay = startingPosition + (rayVector * maximumImpactFraction);
				segmentAABB.minimums = GetMinimum(startingPosition, newFractionedRay);
				segmentAABB.maximums = GetMaximum(startingPosition, newFractionedRay);
			}
		}
		else
		{
			nodeStack.PushToStack(currentNode->m_FirstChildNodeID);
			nodeStack.PushToStack(currentNode->m_SecondChildNodeID);
		}
	}
}



struct FixturePair
{
	int32_t m_FirstFixtureID;
	int32_t m_SecondFixtureID;
};



class BroadPhaseSystem
{
public:
	BroadPhaseSystem();
	~BroadPhaseSystem();

	int32_t AddFixture(const AABB2D& fixtureAABB, void* fixtureReference);
	void RemoveFixture(int32_t currentFixtureID);

	void MoveFixture(int32_t currentFixtureID, const AABB2D& fixtureAABB, const Vector2D& fixtureDisplacement);
	void TriggerFixture(int32_t currentFixtureID);

	void* GetFixtureReference(int32_t currentFixtureID) const;
	AABB2D GetExtendedAABB(int32_t currentFixtureID) const;

	bool AreFixturesOverlapping(int32_t firstFixtureID, int32_t secondFixtureID) const;
	bool QueryCallback(int32_t currentFixtureID);

	template <typename data_type>
	void UpdateFixturePairs(data_type* updateCallback);

	template <typename data_type>
	void QueryAABB(data_type* queryCallback, const AABB2D& fixtureAABB) const;

	template <typename data_type>
	void Raycast(data_type* raycastCallback, const RaycastInput& raycastInput) const;

private:
	void AddToMovingFixtureIDs(int32_t currentFixtureID);
	void RemoveFromMovingFixtureIDs(int32_t currentFixtureID);

private:
	DynamicAABBTree m_DynamicTree;

	FixturePair* m_AllFixturePairs;
	size_t m_MaximumNumberOfFixturePairs;
	size_t m_NumberOfFixturePairs;

	int32_t* m_MovingFixtureIDs;
	size_t m_MaximumNumberOfFixtureIDs;
	size_t m_NumberOfFixtureIDs;

	int32_t m_QueryFixtureID;
};



inline bool CompareFixturePairs(const FixturePair& firstFixturePair, const FixturePair& secondFixturePair)
{
	if (firstFixturePair.m_FirstFixtureID < secondFixturePair.m_FirstFixtureID)
	{
		return true;
	}

	if (firstFixturePair.m_FirstFixtureID == secondFixturePair.m_FirstFixtureID)
	{
		return (firstFixturePair.m_SecondFixtureID < secondFixturePair.m_SecondFixtureID);
	}

	return false;
}



template <typename data_type>
void BroadPhaseSystem::UpdateFixturePairs(data_type* updateCallback)
{
	m_NumberOfFixturePairs = 0U;

	for (size_t movingFixtureIndex = 0; movingFixtureIndex < m_NumberOfFixtureIDs; ++movingFixtureIndex)
	{
		m_QueryFixtureID = m_MovingFixtureIDs[movingFixtureIndex];
		if (m_QueryFixtureID == INVALID_ID)
		{
			continue;
		}

		AABB2D extendedAABB = m_DynamicTree.GetExtendedAABB(m_QueryFixtureID);
		m_DynamicTree.QueryAABB(this, extendedAABB);
	}

	m_NumberOfFixtureIDs = 0U;

	std::sort(m_AllFixturePairs, m_AllFixturePairs + m_NumberOfFixturePairs, CompareFixturePairs);

	size_t currentPairIndex = 0;
	while (currentPairIndex < m_NumberOfFixturePairs)
	{
		FixturePair* currentPair = m_AllFixturePairs + currentPairIndex;
		void* firstFixtureData = m_DynamicTree.GetNodeData(currentPair->m_FirstFixtureID);
		void* secondFixtureData = m_DynamicTree.GetNodeData(currentPair->m_SecondFixtureID);

		updateCallback->AddFixturePair(firstFixtureData, secondFixtureData);
		++currentPairIndex;

		while (currentPairIndex < m_NumberOfFixturePairs)
		{
			FixturePair* possibleDuplicatePair = m_AllFixturePairs + currentPairIndex;
			if (possibleDuplicatePair->m_FirstFixtureID != currentPair->m_FirstFixtureID ||
				possibleDuplicatePair->m_SecondFixtureID != currentPair->m_SecondFixtureID)
			{
				break;
			}

			++currentPairIndex;
		}
	}
}



template <typename data_type>
inline void BroadPhaseSystem::QueryAABB(data_type* queryCallback, const AABB2D& fixtureAABB) const
{
	m_DynamicTree.QueryAABB(queryCallback, fixtureAABB);
}



template <typename data_type>
inline void BroadPhaseSystem::Raycast(data_type* raycastCallback, const RaycastInput& raycastInput) const
{
	m_DynamicTree.Raycast(raycastCallback, raycastInput);
}