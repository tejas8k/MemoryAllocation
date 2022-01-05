#pragma once
#include <cstdint>
#include <ostream>

struct MemoryBlock
{
	void* baseAddress;
	MemoryBlock* nextBlock;
	MemoryBlock* prevBlock;
	size_t dataSize;
	bool free;

	MemoryBlock(void* basePtr) : baseAddress(basePtr), nextBlock(nullptr), prevBlock(nullptr), dataSize(0), free(true) {};
	friend std::ostream& operator<<(std::ostream& os, const MemoryBlock& mb);
};