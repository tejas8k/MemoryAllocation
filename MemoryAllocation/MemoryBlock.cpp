#include "MemoryBlock.h"
#include <iostream>

std::ostream& operator<<(std::ostream& os, const MemoryBlock& mb)
{
	os << "MEM BLOCK " << &mb << " -- " << mb.baseAddress << " -- ";
	if (mb.free) os << " STATUS --> Free -- ";
	else os << " STATUS --> Used -- ";
	os << "DataSize = " << mb.dataSize << "; BlockSize=" << mb.dataSize + sizeof(MemoryBlock);
	return os;
}
