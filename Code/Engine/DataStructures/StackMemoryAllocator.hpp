#pragma once

#include <stdint.h>



const size_t MEMORY_STACK_SIZE = 100U * 1024U;
const size_t MAXIMUM_NUMBER_OF_STACK_NODES = 32U;



struct StackNode
{
	uint8_t* m_BufferData;
	size_t m_BufferSize;
	bool m_UsedHeapAllocation;
};



class StackMemoryAllocator
{
public:
	StackMemoryAllocator();
	~StackMemoryAllocator();

	void* AllocateStackMemory(size_t stackSize);
	void FreeStackMemory(void* pointerToStackMemory);

	size_t GetMaximumAllocationSize() const;

private:
	uint8_t m_StackData[MEMORY_STACK_SIZE];
	size_t m_StackDataIndex;

	size_t m_AllocationSize;
	size_t m_MaximumAllocationSize;

	StackNode m_StackNodes[MAXIMUM_NUMBER_OF_STACK_NODES];
	size_t m_NumberOfStackNodes;
};