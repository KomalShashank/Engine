#pragma once

#include <stdlib.h>



template <typename data_type>
class CDL_ListNode
{
public:
	CDL_ListNode() :
		m_NodeData(data_type()),
		m_PreviousNode(nullptr),
		m_NextNode(nullptr)
	{

	}

public:
	data_type m_NodeData;
	CDL_ListNode* m_PreviousNode;
	CDL_ListNode* m_NextNode;
};



template <typename data_type>
class CDL_List
{
public:
	CDL_List()
	{
		m_FirstNode = nullptr;
	}

	void PushFront(const data_type& nodeData)
	{
		CDL_ListNode<data_type>* newNode = CreateNode(nodeData);

		if (m_FirstNode == nullptr)
		{
			m_FirstNode = newNode;
		}
		else
		{
			CDL_ListNode<data_type>* lastNode = m_FirstNode;
			while (lastNode->m_NextNode != m_FirstNode)
			{
				lastNode = lastNode->m_NextNode;
			}

			m_FirstNode->m_PreviousNode = newNode;
			newNode->m_NextNode = m_FirstNode;

			newNode->m_PreviousNode = lastNode;
			lastNode->m_NextNode = newNode;

			m_FirstNode = newNode;
		}
	}

	void PushBack(const data_type& nodeData)
	{
		CDL_ListNode<data_type>* newNode = CreateNode(nodeData);

		if (m_FirstNode == nullptr)
		{
			m_FirstNode = newNode;
		}
		else
		{
			CDL_ListNode<data_type>* lastNode = m_FirstNode;
			while (lastNode->m_NextNode != m_FirstNode)
			{
				lastNode = lastNode->m_NextNode;
			}

			m_FirstNode->m_PreviousNode = newNode;
			newNode->m_NextNode = m_FirstNode;

			newNode->m_PreviousNode = lastNode;
			lastNode->m_NextNode = newNode;
		}
	}

	void PopFront()
	{
		if (m_FirstNode == nullptr)
		{
			return;
		}
		else
		{
			CDL_ListNode<data_type>* oldFirstNode = m_FirstNode;
			CDL_ListNode<data_type>* lastNode = m_FirstNode;

			while (lastNode->m_NextNode != m_FirstNode)
			{
				lastNode = lastNode->m_NextNode;
			}

			m_FirstNode = oldFirstNode->m_NextNode;

			lastNode->m_NextNode = m_FirstNode;
			m_FirstNode->m_PreviousNode = lastNode;

			free(oldFirstNode);
		}
	}

	void PopBack()
	{
		if (m_FirstNode == nullptr)
		{
			return;
		}
		else
		{
			CDL_ListNode<data_type>* secondLastNode = m_FirstNode;
			CDL_ListNode<data_type>* lastNode = m_FirstNode;

			while (lastNode->m_NextNode != m_FirstNode)
			{
				secondLastNode = lastNode;
				lastNode = lastNode->m_NextNode;
			}

			secondLastNode->m_NextNode = m_FirstNode;
			m_FirstNode->m_PreviousNode = secondLastNode;

			free(lastNode);
		}
	}

private:
	CDL_ListNode<data_type>* CreateNode(const data_type& nodeData)
	{
		CDL_ListNode<data_type>* newNode = (CDL_ListNode<data_type>*)malloc(sizeof(CDL_ListNode<data_type>));
		newNode->m_NodeData = nodeData;
		newNode->m_PreviousNode = newNode;
		newNode->m_NextNode = newNode;

		return newNode;
	}

private:
	CDL_ListNode<data_type>* m_FirstNode;
};