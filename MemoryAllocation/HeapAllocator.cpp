#include "HeapAllocator.h"
#include "HeapTest.h"
#include <iostream>

HeapAllocator* HeapAllocator::hm_instance = nullptr;

HeapAllocator* CreateHeapAllocator(void* pHeapMemory, size_t heapSize)
{
	if (HeapAllocator::hm_instance)
		return HeapAllocator::hm_instance;
	else
	{
		HeapAllocator::hm_instance = new(pHeapMemory) HeapAllocator(pHeapMemory, heapSize);
		return HeapAllocator::hm_instance;
	}
}

HeapAllocator::HeapAllocator(void* start, size_t size)
{
	heapSize = size - sizeof(HeapAllocator);
	void* heapStart = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(start) + sizeof(HeapAllocator));

	//Creating head
	head = GetBlockAtLocation(heapStart);
	head->free = false;

	//Creating first free block
	MemoryBlock* firstFreeBlock = GetBlockAtLocation(reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(heapStart) + sizeof(MemoryBlock)));
	firstFreeBlock->dataSize = heapSize - 3 * sizeof(MemoryBlock);
	head->nextBlock = firstFreeBlock;
	firstFreeBlock->prevBlock = head;

	//Creating tail
	tail = GetBlockAtLocation(reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(firstFreeBlock->baseAddress) + firstFreeBlock->dataSize));
	firstFreeBlock->nextBlock = tail;
	tail->prevBlock = firstFreeBlock;
	tail->free = false;

}


void* HeapAllocator::alloc(size_t dataSize, int alignment)
{
	MemoryBlock* curr = tail;
	MemoryBlock* prev = tail->prevBlock;

	while (prev != head) {
		MemoryBlock* prevBlockReference = prev->prevBlock;
		if (prev->free)
		{
			size_t availableSize = reinterpret_cast<uintptr_t>(curr) - reinterpret_cast<uintptr_t>(prev);

			size_t alignedSize = dataSize;
			alignedSize += (reinterpret_cast<uintptr_t>(curr) - dataSize) % alignment;
			size_t requiredSize = alignedSize + sizeof(MemoryBlock);

			if (availableSize > requiredSize)
			{
				return CreateDataBlock(availableSize, requiredSize, curr, alignedSize, prev, prevBlockReference);
			}
		}
		curr = prev;
		prev = prev->prevBlock;
	}
	return nullptr;
}

void* HeapAllocator::CreateDataBlock(const size_t& availableSize, const size_t& requiredSize, MemoryBlock* curr, const size_t& alignedSize, MemoryBlock* prev, MemoryBlock* prevBlockReference)
{

	size_t remainingSize = availableSize - requiredSize;
	MemoryBlock* newBlock = GetBlockAtLocation(reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(curr) - alignedSize - sizeof(MemoryBlock)));
	newBlock->dataSize = alignedSize;
	newBlock->free = false;

	if (remainingSize > sizeof(MemoryBlock))
	{
		newBlock->prevBlock = prev;
		prev->nextBlock = newBlock;
		size_t newSize = reinterpret_cast<uintptr_t>(newBlock) - reinterpret_cast<uintptr_t>(prev) - sizeof(MemoryBlock);
		prev->dataSize = newSize;
		curr->prevBlock = newBlock;
		newBlock->nextBlock = curr;
	}
	else
	{
		newBlock->prevBlock = prevBlockReference;
		prevBlockReference->nextBlock = newBlock;

		curr->prevBlock = newBlock;
		newBlock->nextBlock = curr;
	}
	return newBlock->baseAddress;
}

bool HeapAllocator::free(void* dataPtr) 
{

	uintptr_t ptr = reinterpret_cast<uintptr_t>(dataPtr) - sizeof(MemoryBlock);
	MemoryBlock* mb = reinterpret_cast<MemoryBlock*>(ptr);
	mb->free = true;
	return true;
}

void HeapAllocator::coalesce() const
 {
	MemoryBlock* curr = head->nextBlock;
	MemoryBlock* next = curr->nextBlock;
	while (next != nullptr && next != tail)
	{
		if (curr->free && next->free)
		{
			uintptr_t difference = reinterpret_cast<uintptr_t>(next->nextBlock) - reinterpret_cast<uintptr_t>(curr);
			curr->dataSize = difference - sizeof(MemoryBlock);

			next = next->nextBlock;
			curr->nextBlock = next;
			next->prevBlock = curr;
		}
		else
		{
			curr = curr->nextBlock;
			next = curr->nextBlock;
		}
	}
}

MemoryBlock* HeapAllocator::GetBlockAtLocation(void* pointer)
{
	MemoryBlock* block = new(pointer) MemoryBlock(reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(pointer) + sizeof(MemoryBlock)));
	return block;
}

bool HeapAllocator::contains(void* ptr) const
{
	return true;
}

bool HeapAllocator::isAllocated(void* ptr) const
{
	return true;
}

void HeapAllocator::showFreeBlocks() const
{
	std::cout << "<------------- Printing free blocks ------------>" << std::endl;
	MemoryBlock* mb = tail;
	while (mb != nullptr)
	{
		std::cout << *mb << std::endl;
		mb = mb->prevBlock;
	}
	std::cout << "<------------------------------------------------>" << std::endl;
}