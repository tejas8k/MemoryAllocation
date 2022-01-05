#pragma once
#include "MemoryBlock.h"

// List of all supported flags
#ifndef SUPPORT_FLAGS
#define SUPPORT_FLAGS
#define SUPPORTS_ALIGNMENT
#define SUPPORTS_SHOWFREEBLOCKS
#endif

// TODO: Override new for this that takes a heap pointer
class HeapManager {
private:
	size_t _heapSize;
	int _numDescriptors;
	MemoryBlock* _head, * _tail;
	uintptr_t _heapStart;
public:
	void Initialize(void* start, size_t size, int num_descriptors);
	void* alloc(size_t size);
	void* alloc(size_t size, int alignment);
	bool free(void* dataPtr);
	void coalesce() const;
	void debug() const;
	bool contains(void*);
	bool isAllocated(void*);
	void showFreeBlocks();
private:
	MemoryBlock* CreateNewBlock(void* pointer, size_t size);
};

HeapManager* CreateHeapManager(void* pHeapMemory, size_t heapSize, int numDescriptors);