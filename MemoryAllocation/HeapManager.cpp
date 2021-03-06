#include "HeapManager.h"
#include "TestHelpers.h"
#include <iostream>

HeapManager* CreateHeapManager(void* pHeapMemory, size_t heapSize, int numDescriptors) {
	char* ch = static_cast<char*>(pHeapMemory);
	ch = WriteChars(ch, "HEAP");
	HeapManager* hm = (HeapManager*)(ch);
	size_t hSize = sizeof(HeapManager) + static_cast<size_t>(8);

	std::cout << "Heap Size =" << hSize << " Available Size=" << heapSize - hSize << std::endl;
	std::cout << "Start memory location = " << pHeapMemory << std::endl;

	hm->Initialize(static_cast<char*>(pHeapMemory) + hSize, heapSize - hSize, numDescriptors);
	ch += sizeof(HeapManager);
	ch = WriteChars(ch, "HEAP");


	return hm;
}

void HeapManager::Initialize(void* start, size_t size, int num_descriptors) {
	_heapSize = size;
	_numDescriptors = num_descriptors;
	_heapStart = reinterpret_cast<uintptr_t>(start);

	_head = CreateNewBlock(start, sizeof(MemoryBlock));
	MemoryBlock* firstFreeBlock = CreateNewBlock(_head->getBlockEnd(), size - 3 * _head->blockSize());
	_tail = CreateNewBlock(firstFreeBlock->getBlockEnd(), sizeof(MemoryBlock));

	_head->occupy(true);
	_tail->occupy(true);

	_head->nextBlock = firstFreeBlock;
	firstFreeBlock->nextBlock = _tail;

	_tail->prevBlock = firstFreeBlock;
	firstFreeBlock->prevBlock = _head;
}



void* HeapManager::alloc(size_t size) {
	return alloc(size, 4);
}

void* HeapManager::alloc(size_t size, int alignment) {
	MemoryBlock* curr = _tail;
	MemoryBlock* next = _tail->prevBlock;

	while (next != nullptr) {
		if (next->free) {
			size_t availableSize = reinterpret_cast<uintptr_t>(curr) - reinterpret_cast<uintptr_t>(next);
			size_t alignedSize = size;
			alignedSize += CalculateAligned(reinterpret_cast<uintptr_t>(curr) - size, alignment);

			// std::cout << "Available Size = " << availableSize << " ; alignedSize = " << alignedSize << std::endl;
			if (availableSize > (alignedSize + sizeof(MemoryBlock))) {
				size_t remainingSize = availableSize - (alignedSize + sizeof(MemoryBlock));

				// Store this as new block can potentially override the value for next block
				MemoryBlock* prevBlockOfNext = next->prevBlock;

				MemoryBlock* newBlock = CreateNewBlock(reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(curr) - alignedSize - sizeof(MemoryBlock)), alignedSize + sizeof(MemoryBlock));
				newBlock->occupy(true);

				curr->prevBlock = newBlock;
				newBlock->nextBlock = curr;

				if (remainingSize <= sizeof(MemoryBlock)) { // Next variable is potentially overriden 
					newBlock->prevBlock = prevBlockOfNext;
					newBlock->prevBlock->nextBlock = newBlock;
				}
				else {
					newBlock->prevBlock = next;
					next->nextBlock = newBlock;

					size_t newSize = reinterpret_cast<uintptr_t>(newBlock) - reinterpret_cast<uintptr_t>(next) - sizeof(MemoryBlock);
					next->shrink(newSize);
				}
				return newBlock->pBaseAddress;
			}
		}
		curr = next;
		next = next->prevBlock;
	}
	return nullptr;
}

bool HeapManager::free(void* dataPtr) {
	uintptr_t ptr = reinterpret_cast<uintptr_t>(dataPtr) - sizeof(MemoryBlock);
	MemoryBlock* mb = reinterpret_cast<MemoryBlock*>(ptr);
	mb->occupy(false);
	return true;
}

void HeapManager::coalesce() const {
	MemoryBlock* curr = _head;
	MemoryBlock* next = curr->nextBlock;
	while (next != _tail) {
		if (curr->free && next->free) {
			curr->nextBlock = next->nextBlock;
			next->nextBlock->prevBlock = curr;
			uintptr_t difference = reinterpret_cast<uintptr_t>(curr->nextBlock) - reinterpret_cast<uintptr_t>(curr);
			curr->dataSize = difference - sizeof(MemoryBlock);
		}
		else {
			curr = curr->nextBlock;
		}

		next = curr->nextBlock;
	}
}

void HeapManager::debug() const {

	std::cout << "<------ Other Block ----->" << std::endl;
	MemoryBlock* mb = _tail;
	while (mb != nullptr) {
		mb->print();
		mb = mb->prevBlock;
	}
	std::cout << "<------------------------->" << std::endl;
}


MemoryBlock* HeapManager::CreateNewBlock(void* pointer, size_t size) {
	// assert(size > sizeof(MemoryBlock));

	if (size < sizeof(MemoryBlock)) return nullptr;


	MemoryBlock* block = reinterpret_cast<MemoryBlock*>(pointer);

	block->pBaseAddress = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(pointer) + sizeof(MemoryBlock));
	block->dataSize = size - sizeof(MemoryBlock);
	block->nextBlock = nullptr;
	block->prevBlock = nullptr;
	block->free = true;

#if DEBUG_BLOCK
	block->info[0] = 'M';
	block->info[1] = 'B';
	block->info[2] = '0';
	block->info[3] = '\0';
#endif

	// std::cout << "Size and Mem block size: " << block->dataSize << std::endl;
	// block->print();

	return block;
}

char* WriteChars(char* ch, const char* str) {
	int i = 0;
	while (str[i] != '\0') {
		*ch = str[i];
		ch++;
		i++;
	}
	return ch;
}

size_t CalculateAligned(uintptr_t ptr, int alignment) {
	return (ptr + alignment) % alignment;
}

bool HeapManager::contains(void* ptr)
{
	return true;
}

bool HeapManager::isAllocated(void* ptr)
{
	return true;
}

void HeapManager::showFreeBlocks()
{
	std::cout << "<------------- Printing All Blocks 2 ------------>" << std::endl;
	MemoryBlock* mb = _tail;
	while (mb != nullptr) {
		mb->print();
		mb = mb->prevBlock;
	}
	std::cout << "<------------------------------------------------>" << std::endl;
}
