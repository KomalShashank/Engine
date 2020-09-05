#include "Engine/DataStructures/StackMemoryAllocator.hpp"
#include "Engine/ErrorHandling/ErrorWarningAssert.hpp"



StackMemoryAllocator::StackMemoryAllocator() :
	m_StackDataIndex(0),
	m_AllocationSize(0),
	m_MaximumAllocationSize(0),
	m_NumberOfStackNodes(0)
{

}



StackMemoryAllocator::~StackMemoryAllocator()
{
	ASSERT_OR_DIE(m_StackDataIndex == 0, "Stack Allocator is not empty.");
	ASSERT_OR_DIE(m_NumberOfStackNodes == 0, "Stack Allocator is not empty.");
}



void* StackMemoryAllocator::AllocateStackMemory(size_t stackSize)
{
	ASSERT_OR_DIE(m_NumberOfStackNodes < MAXIMUM_NUMBER_OF_STACK_NODES, "Number of stack nodes exceeds maximum limit.");

	StackNode* stackNode = m_StackNodes + m_NumberOfStackNodes;
	stackNode->m_BufferSize = stackSize;

	if (m_StackDataIndex + stackSize > MEMORY_STACK_SIZE)
	{
		stackNode->m_BufferData = (uint8_t*)malloc(stackSize);
		stackNode->m_UsedHeapAllocation = true;
	}
	else
	{
		stackNode->m_BufferData = m_StackData + m_StackDataIndex;
		stackNode->m_UsedHeapAllocation = false;
		m_StackDataIndex += stackSize;
	}

	m_AllocationSize += stackSize;
	m_MaximumAllocationSize = (m_MaximumAllocationSize > m_AllocationSize) ? m_MaximumAllocationSize : m_AllocationSize;
	++m_NumberOfStackNodes;

	return stackNode->m_BufferData;
}



void StackMemoryAllocator::FreeStackMemory(void* pointerToStackMemory)
{
	ASSERT_OR_DIE(m_NumberOfStackNodes > 0, "Stack Allocator is empty.");
	StackNode* stackNode = m_StackNodes + (m_NumberOfStackNodes - 1);
	ASSERT_OR_DIE(pointerToStackMemory == stackNode->m_BufferData, "Pointer does not point to stack data.");

	if (stackNode->m_UsedHeapAllocation)
	{
		free(pointerToStackMemory);
	}
	else
	{
		m_StackDataIndex -= stackNode->m_BufferSize;
	}

	m_AllocationSize -= stackNode->m_BufferSize;
	--m_NumberOfStackNodes;

	pointerToStackMemory = nullptr;
}



size_t StackMemoryAllocator::GetMaximumAllocationSize() const
{
	return m_MaximumAllocationSize;
}