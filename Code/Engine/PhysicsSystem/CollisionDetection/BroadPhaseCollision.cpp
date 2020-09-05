#include "Engine/PhysicsSystem/CollisionDetection/BroadPhaseCollision.hpp"



const Vector2D AABB_EXTENSION(0.1f, 0.1f);



DynamicAABBTree::DynamicAABBTree() :
	m_AllTreeNodes(nullptr),
	m_MaximumTreeSize(16U),
	m_NumberOfNodes(0U),
	m_RootNodeID(INVALID_ID),
	m_FreeNodeIDList(0U)
{
	m_AllTreeNodes = (DAT_Node*)malloc(m_MaximumTreeSize * sizeof(DAT_Node));
	memset(m_AllTreeNodes, 0, m_MaximumTreeSize * sizeof(DAT_Node));

	for (size_t nodeIndex = 0; nodeIndex < m_MaximumTreeSize - 1; ++nodeIndex)
	{
		m_AllTreeNodes[nodeIndex].m_ParentNodeID = nodeIndex + 1;
		m_AllTreeNodes[nodeIndex].m_NodeHeight = -1;
	}

	m_AllTreeNodes[m_MaximumTreeSize - 1].m_ParentNodeID = INVALID_ID;
	m_AllTreeNodes[m_MaximumTreeSize - 1].m_NodeHeight = -1;
}



DynamicAABBTree::~DynamicAABBTree()
{
	free(m_AllTreeNodes);
}



int32_t DynamicAABBTree::AddFixtureToTree(const AABB2D& fixtureAABB, void* fixtureReference)
{
	int32_t fixtureID = AllocateNodeToTree();

	m_AllTreeNodes[fixtureID].m_NodeAABB = AABB2D(fixtureAABB.minimums - AABB_EXTENSION, fixtureAABB.maximums + AABB_EXTENSION);
	m_AllTreeNodes[fixtureID].m_NodeData = fixtureReference;
	m_AllTreeNodes[fixtureID].m_NodeHeight = 0;

	AddLeafNodeToTree(fixtureID);

	return fixtureID;
}



void DynamicAABBTree::RemoveFixtureFromTree(int32_t currentFixtureID)
{
	ASSERT_OR_DIE(currentFixtureID >= 0 && currentFixtureID < (int32_t)m_MaximumTreeSize, "Current node ID is out of range.");
	ASSERT_OR_DIE(IsALeafNode(m_AllTreeNodes[currentFixtureID]), "Current node is not a leaf node.");

	RemoveLeafNodeFromTree(currentFixtureID);
	DeallocateNodeFromTree(currentFixtureID);
}



bool DynamicAABBTree::MoveFixtureWithinTree(int32_t currentFixtureID, const AABB2D& fixtureAABB, const Vector2D& fixtureDisplacement)
{
	ASSERT_OR_DIE(currentFixtureID >= 0 && currentFixtureID < (int32_t)m_MaximumTreeSize, "Current node ID is out of range.");
	ASSERT_OR_DIE(IsALeafNode(m_AllTreeNodes[currentFixtureID]), "Current node is not a leaf node.");

	if (m_AllTreeNodes[currentFixtureID].m_NodeAABB.ContainsAABB(fixtureAABB))
	{
		return false;
	}

	RemoveLeafNodeFromTree(currentFixtureID);

	AABB2D extendedAABB = AABB2D(fixtureAABB.minimums - AABB_EXTENSION, fixtureAABB.maximums + AABB_EXTENSION);
	Vector2D predictedDisplacement = fixtureDisplacement * 2.0f;

	if (predictedDisplacement.X < 0.0f)
	{
		extendedAABB.minimums.X += predictedDisplacement.X;
	}
	else
	{
		extendedAABB.maximums.X += predictedDisplacement.X;
	}

	if (predictedDisplacement.Y < 0.0f)
	{
		extendedAABB.minimums.Y += predictedDisplacement.Y;
	}
	else
	{
		extendedAABB.maximums.Y += predictedDisplacement.Y;
	}

	m_AllTreeNodes[currentFixtureID].m_NodeAABB = extendedAABB;
	AddLeafNodeToTree(currentFixtureID);

	return true;
}



void* DynamicAABBTree::GetNodeData(int32_t currentFixtureID) const
{
	ASSERT_OR_DIE(currentFixtureID >= 0 && currentFixtureID < (int32_t)m_MaximumTreeSize, "Current node ID is out of range.");

	return m_AllTreeNodes[currentFixtureID].m_NodeData;
}



AABB2D DynamicAABBTree::GetExtendedAABB(int32_t currentFixtureID) const
{
	ASSERT_OR_DIE(currentFixtureID >= 0 && currentFixtureID < (int32_t)m_MaximumTreeSize, "Current node ID is out of range.");

	return m_AllTreeNodes[currentFixtureID].m_NodeAABB;
}



int32_t DynamicAABBTree::AllocateNodeToTree()
{
	if (m_FreeNodeIDList == INVALID_ID)
	{
		ASSERT_OR_DIE(m_NumberOfNodes >= m_MaximumTreeSize, "The number of nodes has not exceeded maximum size.");

		DAT_Node* allExistingTreeNodes = m_AllTreeNodes;
		m_MaximumTreeSize *= 2U;
		m_AllTreeNodes = (DAT_Node*)malloc(m_MaximumTreeSize * sizeof(DAT_Node));
		memcpy(m_AllTreeNodes, allExistingTreeNodes, m_NumberOfNodes * sizeof(DAT_Node));
		free(allExistingTreeNodes);

		for (size_t nodeIndex = m_NumberOfNodes; nodeIndex < m_MaximumTreeSize - 1; ++nodeIndex)
		{
			m_AllTreeNodes[nodeIndex].m_ParentNodeID = nodeIndex + 1;
			m_AllTreeNodes[nodeIndex].m_NodeHeight = -1;
		}

		m_AllTreeNodes[m_MaximumTreeSize - 1].m_ParentNodeID = INVALID_ID;
		m_AllTreeNodes[m_MaximumTreeSize - 1].m_NodeHeight = -1;
		m_FreeNodeIDList = m_NumberOfNodes;
	}

	int32_t currentNodeID = m_FreeNodeIDList;
	m_FreeNodeIDList = m_AllTreeNodes[currentNodeID].m_ParentNodeID;

	m_AllTreeNodes[currentNodeID].m_ParentNodeID = INVALID_ID;
	m_AllTreeNodes[currentNodeID].m_FirstChildNodeID = INVALID_ID;
	m_AllTreeNodes[currentNodeID].m_SecondChildNodeID = INVALID_ID;
	m_AllTreeNodes[currentNodeID].m_NodeData = nullptr;
	++m_NumberOfNodes;

	return currentNodeID;
}



void DynamicAABBTree::DeallocateNodeFromTree(int32_t currentNodeID)
{
	ASSERT_OR_DIE(currentNodeID >= 0 && currentNodeID < (int32_t)m_MaximumTreeSize, "Current node ID is out of range.");
	ASSERT_OR_DIE(m_NumberOfNodes > 0, "Tree consists of no nodes.");

	m_AllTreeNodes[currentNodeID].m_ParentNodeID = m_FreeNodeIDList;
	m_AllTreeNodes[currentNodeID].m_NodeHeight = -1;
	m_FreeNodeIDList = currentNodeID;
	--m_NumberOfNodes;
}



void DynamicAABBTree::AddLeafNodeToTree(int32_t leafNodeID)
{
	if (m_RootNodeID == INVALID_ID)
	{
		m_RootNodeID = leafNodeID;
		m_AllTreeNodes[m_RootNodeID].m_ParentNodeID = INVALID_ID;

		return;
	}

	AABB2D leafNodeAABB = m_AllTreeNodes[leafNodeID].m_NodeAABB;
	int32_t currentID = m_RootNodeID;

	while (!IsALeafNode(m_AllTreeNodes[currentID]))
	{
		int32_t firstChildNodeID = m_AllTreeNodes[currentID].m_FirstChildNodeID;
		int32_t secondChildNodeID = m_AllTreeNodes[currentID].m_SecondChildNodeID;

		float nodeSurfaceArea = m_AllTreeNodes[currentID].m_NodeAABB.GetAABBPerimeter();

		AABB2D combinedAABB = AABB2D::GetCombinedAABB(m_AllTreeNodes[currentID].m_NodeAABB, leafNodeAABB);
		float combinedSurfaceArea = combinedAABB.GetAABBPerimeter();

		float parentCreationCost = 2.0f * combinedSurfaceArea;
		float leafInheritanceCost = 2.0f * (combinedSurfaceArea - nodeSurfaceArea);

		float firstChildNodeCost;
		AABB2D firstChildCombinedAABB = AABB2D::GetCombinedAABB(leafNodeAABB, m_AllTreeNodes[firstChildNodeID].m_NodeAABB);
		if (IsALeafNode(m_AllTreeNodes[firstChildNodeID]))
		{
			firstChildNodeCost = firstChildCombinedAABB.GetAABBPerimeter() + leafInheritanceCost;
		}
		else
		{
			float oldSurfaceArea = m_AllTreeNodes[firstChildNodeID].m_NodeAABB.GetAABBPerimeter();
			float newSurfaceArea = firstChildCombinedAABB.GetAABBPerimeter();
			firstChildNodeCost = (newSurfaceArea - oldSurfaceArea) + leafInheritanceCost;
		}

		float secondChildNodeCost;
		AABB2D secondChildCombinedAABB = AABB2D::GetCombinedAABB(leafNodeAABB, m_AllTreeNodes[secondChildNodeID].m_NodeAABB);
		if (IsALeafNode(m_AllTreeNodes[secondChildNodeID]))
		{
			secondChildNodeCost = secondChildCombinedAABB.GetAABBPerimeter() + leafInheritanceCost;
		}
		else
		{
			float oldSurfaceArea = m_AllTreeNodes[secondChildNodeID].m_NodeAABB.GetAABBPerimeter();
			float newSurfaceArea = secondChildCombinedAABB.GetAABBPerimeter();
			secondChildNodeCost = (newSurfaceArea - oldSurfaceArea) + leafInheritanceCost;
		}

		if (parentCreationCost < firstChildNodeCost && parentCreationCost < secondChildNodeCost)
		{
			break;
		}

		currentID = (firstChildNodeCost < secondChildNodeCost) ? firstChildNodeID : secondChildNodeID;
	}

	int32_t siblingNodeID = currentID;

	int32_t oldParentID = m_AllTreeNodes[siblingNodeID].m_ParentNodeID;
	int32_t newParentID = AllocateNodeToTree();
	m_AllTreeNodes[newParentID].m_ParentNodeID = oldParentID;
	m_AllTreeNodes[newParentID].m_NodeAABB = AABB2D::GetCombinedAABB(leafNodeAABB, m_AllTreeNodes[siblingNodeID].m_NodeAABB);
	m_AllTreeNodes[newParentID].m_NodeHeight = m_AllTreeNodes[siblingNodeID].m_NodeHeight + 1;

	if (oldParentID != INVALID_ID)
	{
		if (m_AllTreeNodes[oldParentID].m_FirstChildNodeID == siblingNodeID)
		{
			m_AllTreeNodes[oldParentID].m_FirstChildNodeID = newParentID;
		}
		else
		{
			m_AllTreeNodes[oldParentID].m_SecondChildNodeID = newParentID;
		}
	}
	else
	{
		m_RootNodeID = newParentID;
	}

	m_AllTreeNodes[newParentID].m_FirstChildNodeID = siblingNodeID;
	m_AllTreeNodes[newParentID].m_SecondChildNodeID = leafNodeID;
	m_AllTreeNodes[siblingNodeID].m_ParentNodeID = newParentID;
	m_AllTreeNodes[leafNodeID].m_ParentNodeID = newParentID;

	currentID = m_AllTreeNodes[leafNodeID].m_ParentNodeID;
	while (currentID != INVALID_ID)
	{
		currentID = BalanceTree(currentID);

		int32_t firstChildNodeID = m_AllTreeNodes[currentID].m_FirstChildNodeID;
		int32_t secondChildNodeID = m_AllTreeNodes[currentID].m_SecondChildNodeID;

		ASSERT_OR_DIE(firstChildNodeID != INVALID_ID, "First Child ID Invalid.");
		ASSERT_OR_DIE(secondChildNodeID != INVALID_ID, "Second Child ID Invalid.");

		m_AllTreeNodes[currentID].m_NodeHeight = 1 + GetMaximum(m_AllTreeNodes[firstChildNodeID].m_NodeHeight, m_AllTreeNodes[secondChildNodeID].m_NodeHeight);
		m_AllTreeNodes[currentID].m_NodeAABB = AABB2D::GetCombinedAABB(m_AllTreeNodes[firstChildNodeID].m_NodeAABB, m_AllTreeNodes[secondChildNodeID].m_NodeAABB);

		currentID = m_AllTreeNodes[currentID].m_ParentNodeID;
	}
}



void DynamicAABBTree::RemoveLeafNodeFromTree(int32_t leafNodeID)
{
	if (leafNodeID == m_RootNodeID)
	{
		m_RootNodeID = INVALID_ID;

		return;
	}

	int32_t parentID = m_AllTreeNodes[leafNodeID].m_ParentNodeID;
	int32_t grandParentID = m_AllTreeNodes[parentID].m_ParentNodeID;
	int32_t siblingNodeID = (m_AllTreeNodes[parentID].m_FirstChildNodeID == leafNodeID) ? m_AllTreeNodes[parentID].m_SecondChildNodeID : m_AllTreeNodes[parentID].m_FirstChildNodeID;

	if (grandParentID != INVALID_ID)
	{
		if (m_AllTreeNodes[grandParentID].m_FirstChildNodeID == parentID)
		{
			m_AllTreeNodes[grandParentID].m_FirstChildNodeID = siblingNodeID;
		}
		else
		{
			m_AllTreeNodes[grandParentID].m_SecondChildNodeID = siblingNodeID;
		}

		m_AllTreeNodes[siblingNodeID].m_ParentNodeID = grandParentID;
		DeallocateNodeFromTree(parentID);

		int32_t currentID = grandParentID;
		while (currentID != INVALID_ID)
		{
			currentID = BalanceTree(currentID);

			int32_t firstChildNodeID = m_AllTreeNodes[currentID].m_FirstChildNodeID;
			int32_t secondChildNodeID = m_AllTreeNodes[currentID].m_SecondChildNodeID;

			m_AllTreeNodes[currentID].m_NodeHeight = 1 + GetMaximum(m_AllTreeNodes[firstChildNodeID].m_NodeHeight, m_AllTreeNodes[secondChildNodeID].m_NodeHeight);
			m_AllTreeNodes[currentID].m_NodeAABB = AABB2D::GetCombinedAABB(m_AllTreeNodes[firstChildNodeID].m_NodeAABB, m_AllTreeNodes[secondChildNodeID].m_NodeAABB);

			currentID = m_AllTreeNodes[currentID].m_ParentNodeID;
		}
	}
	else
	{
		m_RootNodeID = siblingNodeID;
		m_AllTreeNodes[siblingNodeID].m_ParentNodeID = INVALID_ID;
		DeallocateNodeFromTree(parentID);
	}
}



int32_t DynamicAABBTree::BalanceTree(int32_t currentNodeID)
{
	ASSERT_OR_DIE(currentNodeID != INVALID_ID, "Current Node ID Invalid.");

	DAT_Node* currentNode = m_AllTreeNodes + currentNodeID;
	if (IsALeafNode(*currentNode) || currentNode->m_NodeHeight < 2)
	{
		return currentNodeID;
	}

	int32_t firstChildNodeID = currentNode->m_FirstChildNodeID;
	int32_t secondChildNodeID = currentNode->m_SecondChildNodeID;

	ASSERT_OR_DIE(firstChildNodeID >= 0 && firstChildNodeID < (int32_t)m_MaximumTreeSize, "First child node ID is out of range.");
	ASSERT_OR_DIE(secondChildNodeID >= 0 && secondChildNodeID < (int32_t)m_MaximumTreeSize, "Second child node ID is out of range.");

	DAT_Node* firstChildNode = m_AllTreeNodes + firstChildNodeID;
	DAT_Node* secondChildNode = m_AllTreeNodes + secondChildNodeID;

	int32_t treeBalance = secondChildNode->m_NodeHeight - firstChildNode->m_NodeHeight;

	if (treeBalance < -1)
	{
		int32_t firstChildFirstChildID = firstChildNode->m_FirstChildNodeID;
		int32_t firstChildSecondChildID = firstChildNode->m_SecondChildNodeID;

		ASSERT_OR_DIE(firstChildFirstChildID >= 0 && firstChildFirstChildID < (int32_t)m_MaximumTreeSize, "First child node ID is out of range.");
		ASSERT_OR_DIE(firstChildSecondChildID >= 0 && firstChildSecondChildID < (int32_t)m_MaximumTreeSize, "Second child node ID is out of range.");

		DAT_Node* firstChildFirstChild = m_AllTreeNodes + firstChildFirstChildID;
		DAT_Node* firstChildSecondChild = m_AllTreeNodes + firstChildSecondChildID;

		firstChildNode->m_FirstChildNodeID = currentNodeID;
		firstChildNode->m_ParentNodeID = currentNode->m_ParentNodeID;
		currentNode->m_ParentNodeID = firstChildNodeID;

		if (firstChildNode->m_ParentNodeID != INVALID_ID)
		{
			if (m_AllTreeNodes[firstChildNode->m_ParentNodeID].m_FirstChildNodeID == currentNodeID)
			{
				m_AllTreeNodes[firstChildNode->m_ParentNodeID].m_FirstChildNodeID = firstChildNodeID;
			}
			else
			{
				ASSERT_OR_DIE(m_AllTreeNodes[firstChildNode->m_ParentNodeID].m_SecondChildNodeID == currentNodeID, "");
				m_AllTreeNodes[firstChildNode->m_ParentNodeID].m_SecondChildNodeID = firstChildNodeID;
			}
		}
		else
		{
			m_RootNodeID = firstChildNodeID;
		}

		if (firstChildFirstChild->m_NodeHeight > firstChildSecondChild->m_NodeHeight)
		{
			firstChildNode->m_SecondChildNodeID = firstChildFirstChildID;
			currentNode->m_FirstChildNodeID = firstChildSecondChildID;
			firstChildSecondChild->m_ParentNodeID = currentNodeID;

			currentNode->m_NodeAABB = AABB2D::GetCombinedAABB(secondChildNode->m_NodeAABB, firstChildSecondChild->m_NodeAABB);
			firstChildNode->m_NodeAABB = AABB2D::GetCombinedAABB(currentNode->m_NodeAABB, firstChildFirstChild->m_NodeAABB);

			currentNode->m_NodeHeight = 1 + GetMaximum(secondChildNode->m_NodeHeight, firstChildSecondChild->m_NodeHeight);
			firstChildNode->m_NodeHeight = 1 + GetMaximum(currentNode->m_NodeHeight, firstChildFirstChild->m_NodeHeight);
		}
		else
		{
			firstChildNode->m_SecondChildNodeID = firstChildSecondChildID;
			currentNode->m_FirstChildNodeID = firstChildFirstChildID;
			firstChildFirstChild->m_ParentNodeID = currentNodeID;

			currentNode->m_NodeAABB = AABB2D::GetCombinedAABB(secondChildNode->m_NodeAABB, firstChildFirstChild->m_NodeAABB);
			firstChildNode->m_NodeAABB = AABB2D::GetCombinedAABB(currentNode->m_NodeAABB, firstChildSecondChild->m_NodeAABB);

			currentNode->m_NodeHeight = 1 + GetMaximum(secondChildNode->m_NodeHeight, firstChildFirstChild->m_NodeHeight);
			firstChildNode->m_NodeHeight = 1 + GetMaximum(currentNode->m_NodeHeight, firstChildSecondChild->m_NodeHeight);
		}

		return firstChildNodeID;
	}

	if (treeBalance > 1)
	{
		int32_t secondChildFirstChildID = secondChildNode->m_FirstChildNodeID;
		int32_t secondChildSecondChildID = secondChildNode->m_SecondChildNodeID;

		ASSERT_OR_DIE(secondChildFirstChildID >= 0 && secondChildFirstChildID < (int32_t)m_MaximumTreeSize, "First child node ID is out of range.");
		ASSERT_OR_DIE(secondChildSecondChildID >= 0 && secondChildSecondChildID < (int32_t)m_MaximumTreeSize, "Second child node ID is out of range.");

		DAT_Node* secondChildFirstChild = m_AllTreeNodes + secondChildFirstChildID;
		DAT_Node* secondChildSecondChild = m_AllTreeNodes + secondChildSecondChildID;

		secondChildNode->m_FirstChildNodeID = currentNodeID;
		secondChildNode->m_ParentNodeID = currentNode->m_ParentNodeID;
		currentNode->m_ParentNodeID = secondChildNodeID;

		if (secondChildNode->m_ParentNodeID != INVALID_ID)
		{
			if (m_AllTreeNodes[secondChildNode->m_ParentNodeID].m_FirstChildNodeID == currentNodeID)
			{
				m_AllTreeNodes[secondChildNode->m_ParentNodeID].m_FirstChildNodeID = secondChildNodeID;
			}
			else
			{
				ASSERT_OR_DIE(m_AllTreeNodes[secondChildNode->m_ParentNodeID].m_SecondChildNodeID == currentNodeID, "");
				m_AllTreeNodes[secondChildNode->m_ParentNodeID].m_SecondChildNodeID = secondChildNodeID;
			}
		}
		else
		{
			m_RootNodeID = secondChildNodeID;
		}

		if (secondChildFirstChild->m_NodeHeight > secondChildSecondChild->m_NodeHeight)
		{
			secondChildNode->m_SecondChildNodeID = secondChildFirstChildID;
			currentNode->m_SecondChildNodeID = secondChildSecondChildID;
			secondChildSecondChild->m_ParentNodeID = currentNodeID;

			currentNode->m_NodeAABB = AABB2D::GetCombinedAABB(firstChildNode->m_NodeAABB, secondChildSecondChild->m_NodeAABB);
			secondChildNode->m_NodeAABB = AABB2D::GetCombinedAABB(currentNode->m_NodeAABB, secondChildFirstChild->m_NodeAABB);

			currentNode->m_NodeHeight = 1 + GetMaximum(firstChildNode->m_NodeHeight, secondChildSecondChild->m_NodeHeight);
			secondChildNode->m_NodeHeight = 1 + GetMaximum(currentNode->m_NodeHeight, secondChildFirstChild->m_NodeHeight);
		}
		else
		{
			secondChildNode->m_SecondChildNodeID = secondChildSecondChildID;
			currentNode->m_SecondChildNodeID = secondChildFirstChildID;
			secondChildFirstChild->m_ParentNodeID = currentNodeID;

			currentNode->m_NodeAABB = AABB2D::GetCombinedAABB(firstChildNode->m_NodeAABB, secondChildFirstChild->m_NodeAABB);
			secondChildNode->m_NodeAABB = AABB2D::GetCombinedAABB(currentNode->m_NodeAABB, secondChildSecondChild->m_NodeAABB);

			currentNode->m_NodeHeight = 1 + GetMaximum(firstChildNode->m_NodeHeight, secondChildFirstChild->m_NodeHeight);
			secondChildNode->m_NodeHeight = 1 + GetMaximum(currentNode->m_NodeHeight, secondChildSecondChild->m_NodeHeight);
		}

		return secondChildNodeID;
	}

	return currentNodeID;
}



BroadPhaseSystem::BroadPhaseSystem() :
	m_AllFixturePairs(nullptr),
	m_MaximumNumberOfFixturePairs(16U),
	m_NumberOfFixturePairs(0U),
	m_MovingFixtureIDs(nullptr),
	m_MaximumNumberOfFixtureIDs(16U),
	m_NumberOfFixtureIDs(0U)
{
	m_AllFixturePairs = (FixturePair*)malloc(m_MaximumNumberOfFixturePairs * sizeof(FixturePair));
	m_MovingFixtureIDs = (int32_t*)malloc(m_MaximumNumberOfFixtureIDs * sizeof(int32_t));
}



BroadPhaseSystem::~BroadPhaseSystem()
{
	free(m_MovingFixtureIDs);
	free(m_AllFixturePairs);
}



int32_t BroadPhaseSystem::AddFixture(const AABB2D& fixtureAABB, void* fixtureReference)
{
	int32_t fixtureID = m_DynamicTree.AddFixtureToTree(fixtureAABB, fixtureReference);
	AddToMovingFixtureIDs(fixtureID);

	return fixtureID;
}



void BroadPhaseSystem::RemoveFixture(int32_t currentFixtureID)
{
	RemoveFromMovingFixtureIDs(currentFixtureID);
	m_DynamicTree.RemoveFixtureFromTree(currentFixtureID);
}



void BroadPhaseSystem::MoveFixture(int32_t currentFixtureID, const AABB2D& fixtureAABB, const Vector2D& fixtureDisplacement)
{
	bool fixtureMoved = m_DynamicTree.MoveFixtureWithinTree(currentFixtureID, fixtureAABB, fixtureDisplacement);
	if (fixtureMoved)
	{
		AddToMovingFixtureIDs(currentFixtureID);
	}
}



void BroadPhaseSystem::TriggerFixture(int32_t currentFixtureID)
{
	AddToMovingFixtureIDs(currentFixtureID);
}



void* BroadPhaseSystem::GetFixtureReference(int32_t currentFixtureID) const
{
	return m_DynamicTree.GetNodeData(currentFixtureID);
}



AABB2D BroadPhaseSystem::GetExtendedAABB(int32_t currentFixtureID) const
{
	return m_DynamicTree.GetExtendedAABB(currentFixtureID);
}



bool BroadPhaseSystem::AreFixturesOverlapping(int32_t firstFixtureID, int32_t secondFixtureID) const
{
	AABB2D firstFixtureAABB = m_DynamicTree.GetExtendedAABB(firstFixtureID);
	AABB2D secondFixtureAABB = m_DynamicTree.GetExtendedAABB(secondFixtureID);

	return AABB2D::AreAABBsIntersecting(firstFixtureAABB, secondFixtureAABB);
}



bool BroadPhaseSystem::QueryCallback(int32_t currentFixtureID)
{
	if (currentFixtureID == m_QueryFixtureID)
	{
		return true;
	}

	if (m_NumberOfFixturePairs >= m_MaximumNumberOfFixturePairs)
	{
		FixturePair* allOldFixturePairs = m_AllFixturePairs;
		m_MaximumNumberOfFixturePairs *= 2U;
		m_AllFixturePairs = (FixturePair*)malloc(m_MaximumNumberOfFixturePairs * sizeof(FixturePair));
		memcpy(m_AllFixturePairs, allOldFixturePairs, m_NumberOfFixturePairs * sizeof(FixturePair));
		free(allOldFixturePairs);
	}

	m_AllFixturePairs[m_NumberOfFixturePairs].m_FirstFixtureID = GetMinimum(currentFixtureID, m_QueryFixtureID);
	m_AllFixturePairs[m_NumberOfFixturePairs].m_SecondFixtureID = GetMaximum(currentFixtureID, m_QueryFixtureID);
	++m_NumberOfFixturePairs;

	return true;
}



void BroadPhaseSystem::AddToMovingFixtureIDs(int32_t currentFixtureID)
{
	if (m_NumberOfFixtureIDs >= m_MaximumNumberOfFixtureIDs)
	{
		int32_t* existingMovingFixtureIDs = m_MovingFixtureIDs;
		m_MaximumNumberOfFixtureIDs *= 2U;
		m_MovingFixtureIDs = (int32_t*)malloc(m_MaximumNumberOfFixtureIDs * sizeof(int32_t));
		memcpy(m_MovingFixtureIDs, existingMovingFixtureIDs, m_NumberOfFixtureIDs * sizeof(int32_t));
		free(existingMovingFixtureIDs);
	}

	m_MovingFixtureIDs[m_NumberOfFixtureIDs] = currentFixtureID;
	++m_NumberOfFixtureIDs;
}



void BroadPhaseSystem::RemoveFromMovingFixtureIDs(int32_t currentFixtureID)
{
	for (size_t fixtureIndex = 0; fixtureIndex < m_NumberOfFixtureIDs; ++fixtureIndex)
	{
		if (m_MovingFixtureIDs[fixtureIndex] == currentFixtureID)
		{
			m_MovingFixtureIDs[fixtureIndex] = INVALID_ID;
		}
	}
}