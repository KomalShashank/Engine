#include "Engine/DataStructures/BlockMemoryAllocator.hpp"
#include "Engine/ErrorHandling/ErrorWarningAssert.hpp"



size_t BlockMemoryAllocator::s_BlockSizes[NUMBER_OF_MEMORY_BLOCK_SIZES] =
{
	16, 32, 64, 96, 128, 160, 192,
	224, 256, 320, 384, 448, 512, 640
};

uint8_t BlockMemoryAllocator::s_BlockSizeLookup[MAXIMUM_MEMORY_BLOCK_SIZE + 1];
bool BlockMemoryAllocator::s_BlockSizeLookupInitialized = false;



BlockMemoryAllocator::BlockMemoryAllocator()
{
	m_ChunkSpace = MEMORY_CHUNK_INCREMENT;
	m_NumberOfChunks = 0;
	m_MemoryChunks = (MemoryChunk*)malloc(m_ChunkSpace * sizeof(MemoryChunk));

	memset(m_MemoryChunks, 0, m_ChunkSpace * sizeof(MemoryChunk));
	memset(m_FreeList, 0, sizeof(m_FreeList));

	if (!s_BlockSizeLookupInitialized)
	{
		size_t blockSizeIndex = 0;
		for (size_t lookupIndex = 1; lookupIndex <= MAXIMUM_MEMORY_BLOCK_SIZE; ++lookupIndex)
		{
			ASSERT_OR_DIE(blockSizeIndex < NUMBER_OF_MEMORY_BLOCK_SIZES, "Block size index exceeded.");
			
			if (lookupIndex > s_BlockSizes[blockSizeIndex])
			{
				++blockSizeIndex;
			}

			s_BlockSizeLookup[lookupIndex] = (uint8_t)blockSizeIndex;
		}

		s_BlockSizeLookupInitialized = true;
	}
}



BlockMemoryAllocator::~BlockMemoryAllocator()
{
	for (size_t chunkIndex = 0; chunkIndex < m_NumberOfChunks; ++chunkIndex)
	{
		free(m_MemoryChunks[chunkIndex].m_BlockNodes);
	}

	free(m_MemoryChunks);
}



void* BlockMemoryAllocator::AllocateBlockMemory(const size_t& blockSize)
{
	if (blockSize == 0)
	{
		return nullptr;
	}

	ASSERT_OR_DIE(blockSize > 0, "Block size is less than minimum.");

	if (blockSize > MAXIMUM_MEMORY_BLOCK_SIZE)
	{
		return malloc(blockSize);
	}

	size_t freeListIndex = s_BlockSizeLookup[blockSize];
	ASSERT_OR_DIE(freeListIndex >= 0 && freeListIndex < NUMBER_OF_MEMORY_BLOCK_SIZES, "Free List index is not in range.");

	if (m_FreeList[freeListIndex] != nullptr)
	{
		MemoryBlockNode* blockNode = m_FreeList[freeListIndex];
		m_FreeList[freeListIndex] = blockNode->m_NextNode;

		return blockNode;
	}
	else
	{
		if (m_NumberOfChunks == m_ChunkSpace)
		{
			MemoryChunk* oldMemoryChunks = m_MemoryChunks;
			m_ChunkSpace += MEMORY_CHUNK_INCREMENT;
			m_MemoryChunks = (MemoryChunk*)malloc(m_ChunkSpace * sizeof(MemoryChunk));
			memcpy(m_MemoryChunks, oldMemoryChunks, m_NumberOfChunks * sizeof(MemoryChunk));
			memset(m_MemoryChunks + m_NumberOfChunks, 0, MEMORY_CHUNK_INCREMENT * sizeof(MemoryChunk));
			free(oldMemoryChunks);
		}

		MemoryChunk* newChunk = m_MemoryChunks + m_NumberOfChunks;
		newChunk->m_BlockNodes = (MemoryBlockNode*)malloc(MEMORY_CHUNK_SIZE);

		size_t sizeOfBlock = s_BlockSizes[freeListIndex];
		newChunk->m_BlockSize = sizeOfBlock;
		size_t numberOfBlocks = MEMORY_CHUNK_SIZE / sizeOfBlock;
		ASSERT_OR_DIE(numberOfBlocks * sizeOfBlock <= MEMORY_CHUNK_SIZE, "Number of blocks exceed chunk size.");
		
		for (size_t blockIndex = 0; blockIndex < numberOfBlocks - 1; ++blockIndex)
		{
			MemoryBlockNode* currentBlock = (MemoryBlockNode*)((uint8_t*)newChunk->m_BlockNodes + (sizeOfBlock * blockIndex));
			MemoryBlockNode* nextBlock = (MemoryBlockNode*)((uint8_t*)newChunk->m_BlockNodes + (sizeOfBlock * (blockIndex + 1)));
			currentBlock->m_NextNode = nextBlock;
		}

		MemoryBlockNode* lastBlock = (MemoryBlockNode*)((uint8_t*)newChunk->m_BlockNodes + (sizeOfBlock * (numberOfBlocks - 1)));
		lastBlock->m_NextNode = nullptr;

		m_FreeList[freeListIndex] = newChunk->m_BlockNodes->m_NextNode;
		++m_NumberOfChunks;

		return newChunk->m_BlockNodes;
	}
}



void BlockMemoryAllocator::FreeBlockMemory(void* pointerToBlockMemory, const size_t& blockSize)
{
	if (blockSize == 0)
	{
		return;
	}

	ASSERT_OR_DIE(blockSize > 0, "Block size is less than minimum.");

	if (blockSize > MAXIMUM_MEMORY_BLOCK_SIZE)
	{
		free(pointerToBlockMemory);
		return;
	}

	size_t freeListIndex = s_BlockSizeLookup[blockSize];
	ASSERT_OR_DIE(freeListIndex >= 0 && freeListIndex < NUMBER_OF_MEMORY_BLOCK_SIZES, "Free List index is not in range.");

	MemoryBlockNode* blockNode = (MemoryBlockNode*)pointerToBlockMemory;
	blockNode->m_NextNode = m_FreeList[freeListIndex];
	m_FreeList[freeListIndex] = blockNode;
}



void BlockMemoryAllocator::ClearBlockMemory()
{
	for (size_t chunkIndex = 0; chunkIndex < m_NumberOfChunks; ++chunkIndex)
	{
		free(m_MemoryChunks[chunkIndex].m_BlockNodes);
	}

	m_NumberOfChunks = 0;
	memset(m_MemoryChunks, 0, m_ChunkSpace * sizeof(MemoryChunk));
	memset(m_FreeList, 0, sizeof(m_FreeList));
}