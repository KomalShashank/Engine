#pragma once

#include "Engine/ErrorHandling/ErrorWarningAssert.hpp"



template <typename data_type, size_t stackSize>
class ExtendableStack
{
public:
	ExtendableStack() :
		m_DataStack(m_stackArray),
		m_NumberOfElements(0U),
		m_MaximumNumberOfElements(stackSize)
	{

	}

	~ExtendableStack()
	{
		if (m_DataStack != m_stackArray)
		{
			free(m_DataStack);
			m_DataStack = nullptr;
		}
	}

	void PushToStack(const data_type& stackElement)
	{
		if (m_NumberOfElements >= m_MaximumNumberOfElements)
		{
			data_type* oldDataStack = m_DataStack;
			m_MaximumNumberOfElements *= 2U;
			m_DataStack = (data_type*)malloc(m_MaximumNumberOfElements * sizeof(data_type));
			memcpy(m_DataStack, oldDataStack, m_NumberOfElements * sizeof(data_type));

			if (oldDataStack != m_stackArray)
			{
				free(oldDataStack);
			}
		}

		m_DataStack[m_NumberOfElements] = stackElement;
		++m_NumberOfElements;
	}

	data_type PopFromStack()
	{
		ASSERT_OR_DIE(m_NumberOfElements > 0U, "Stack is empty.");
		--m_NumberOfElements;

		return m_DataStack[m_NumberOfElements];
	}

	size_t GetSize() const
	{
		return m_NumberOfElements;
	}

public:
	data_type* m_DataStack;
	data_type m_stackArray[stackSize];
	size_t m_NumberOfElements;
	size_t m_MaximumNumberOfElements;
};