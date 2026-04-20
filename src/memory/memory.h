#ifndef MEMORY_H
#define MEMORY_H

void  initMemory();
void* allocateMemory(int size);
void  freeMemory(void* ptr);
void  printMemoryStatus();
int   getUsedBlocks();
int   getTotalBlocks();

#endif
