#pragma once
#include "MemoryBlock.h"

class HeapAllocator {
private:
	size_t heapSize;
	MemoryBlock* head;
	MemoryBlock* tail;
	size_t numAllocators = 0;
	MemoryBlock* GetBlockAtLocation(void* pointer);

public:
	HeapAllocator(void* start, size_t size);
	void* alloc(size_t size, int alignment);
	void* CreateDataBlock(const size_t& availableSize, const size_t& requiredSize, MemoryBlock* curr, const size_t& alignedSize, MemoryBlock* prev, MemoryBlock* prevBlockReference);
	bool free(void* dataPtr);
	void destroy();
	void coalesce() const;
	bool contains(void*) const;
	bool isAllocated(void*) const;
	void showFreeBlocks() const;
	void DisplayOutstandingAllocations() const;
	void init_FSA(size_t* blockSizes, size_t* numBlocks, size_t count);
	static HeapAllocator* hm_instance;
};

HeapAllocator* CreateHeapAllocator(void* pHeapMemory, size_t heapSize);