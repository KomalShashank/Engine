#pragma once

#include <stdint.h>



const size_t MEMORY_CHUNK_SIZE = 16U * 1024U;
const size_t MAXIMUM_MEMORY_BLOCK_SIZE = 640U;
const size_t NUMBER_OF_MEMORY_BLOCK_SIZES = 14U;
const size_t MEMORY_CHUNK_INCREMENT = 128U;



struct MemoryBlockNode
{
	MemoryBlockNode* m_NextNode;
};



struct MemoryChunk
{
	size_t m_BlockSize;
	MemoryBlockNode* m_BlockNodes;
};



class BlockMemoryAllocator
{
public:
	BlockMemoryAllocator();
	~BlockMemoryAllocator();

	void* AllocateBlockMemory(const size_t& blockSize);
	void FreeBlockMemory(void* pointerToBlockMemory, const size_t& blockSize);
	void ClearBlockMemory();

private:
	MemoryChunk* m_MemoryChunks;
	size_t m_NumberOfChunks;
	size_t m_ChunkSpace;

	MemoryBlockNode* m_FreeList[NUMBER_OF_MEMORY_BLOCK_SIZES];

	static size_t s_BlockSizes[NUMBER_OF_MEMORY_BLOCK_SIZES];
	static uint8_t s_BlockSizeLookup[MAXIMUM_MEMORY_BLOCK_SIZE + 1];
	static bool s_BlockSizeLookupInitialized;
};