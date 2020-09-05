#pragma once
#include <Engine/ErrorHandling/ErrorWarningAssert.hpp>



struct PageNode
{
	PageNode* m_NextNode;
};



template <typename data_type>
class ObjectPool
{
public:
	ObjectPool() :
	m_FreeStack(nullptr),
	m_DataBuffer(nullptr),
	m_MaximumNumberOfObjects(0),
	m_NumberOfActiveObjects(0)
	{
		
	}

	void InitializeObjectPool(const size_t& numberOfObjects)
	{
		m_MaximumNumberOfObjects = numberOfObjects;
		size_t bufferSize = sizeof(data_type) * numberOfObjects;
		m_DataBuffer = (data_type*)malloc(bufferSize);

		for (int32_t objectIndex = numberOfObjects - 1; objectIndex >= 0; --objectIndex)
		{
			data_type* pointerToObject = &m_DataBuffer[objectIndex];
			PageNode* nodePointer = (PageNode*)pointerToObject;
			nodePointer->m_NextNode = m_FreeStack;
			m_FreeStack = nodePointer;
		}
	}

	void UninitializeObjectPool()
	{
		free(m_DataBuffer);
	}

	data_type* AllocateObjectFromPool()
	{
		++m_NumberOfActiveObjects;
		
		data_type* pointerToObject = (data_type*)m_FreeStack;
		m_FreeStack = m_FreeStack->m_NextNode;
		new(pointerToObject)data_type();

		return pointerToObject;
	}

	void DeallocateObjectToPool(data_type* pointerToObject)
	{
		--m_NumberOfActiveObjects;
		
		pointerToObject->~data_type();
		PageNode* nodePointer = (PageNode*)pointerToObject;
		nodePointer->m_NextNode = m_FreeStack;
		m_FreeStack = nodePointer;
	}

	size_t GetSize() const
	{
		return m_MaximumNumberOfObjects - m_NumberOfActiveObjects;
	}

	bool IsEmpty() const
	{
		return (GetSize() == 0);
	}

public:
	PageNode* m_FreeStack;
	data_type* m_DataBuffer;

	size_t m_MaximumNumberOfObjects;
	size_t m_NumberOfActiveObjects;
};