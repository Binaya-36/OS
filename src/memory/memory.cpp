#include "memory.h"
#include "../kernel/screen.h"

// Block size reduced from 4096 to 512 bytes.
// This means each process only needs 512 bytes instead of 4KB,
// so multitasking uses far less memory.
// 256 blocks x 512 bytes = 128KB total pool.
#define MEMORY_SIZE  (128 * 1024)
#define BLOCK_SIZE   512
#define NUM_BLOCKS   (MEMORY_SIZE / BLOCK_SIZE)
#define BLOCK_CONT   255

static unsigned char memoryPool[MEMORY_SIZE];
static unsigned char blockMap[NUM_BLOCKS];
static int usedBlocks = 0;

void initMemory()
{
    for(int i = 0; i < NUM_BLOCKS; i++)
        blockMap[i] = 0;
    usedBlocks = 0;
}

void* allocateMemory(int size)
{
    if(size <= 0) return 0;

    int blocksNeeded = (size + BLOCK_SIZE - 1) / BLOCK_SIZE;
    int freeCount    = 0;
    int startBlock   = -1;

    for(int i = 0; i < NUM_BLOCKS; i++)
    {
        if(blockMap[i] == 0)
        {
            if(freeCount == 0) startBlock = i;
            freeCount++;
            if(freeCount == blocksNeeded)
            {
                blockMap[startBlock] = (unsigned char)blocksNeeded;
                for(int j = startBlock + 1; j < startBlock + blocksNeeded; j++)
                    blockMap[j] = BLOCK_CONT;
                usedBlocks += blocksNeeded;
                return (void*)(memoryPool + startBlock * BLOCK_SIZE);
            }
        }
        else
        {
            freeCount  = 0;
            startBlock = -1;
        }
    }

    printString("ERROR: Out of memory!");
    return 0;
}

void freeMemory(void* ptr)
{
    if(ptr == 0) return;
    int blockNumber = ((unsigned char*)ptr - memoryPool) / BLOCK_SIZE;
    if(blockNumber < 0 || blockNumber >= NUM_BLOCKS) return;
    int blocksToFree = (int)blockMap[blockNumber];
    if(blocksToFree == 0 || blocksToFree == BLOCK_CONT) return;
    for(int i = blockNumber; i < blockNumber + blocksToFree && i < NUM_BLOCKS; i++)
        blockMap[i] = 0;
    usedBlocks -= blocksToFree;
    if(usedBlocks < 0) usedBlocks = 0;
}

int getUsedBlocks()  { return usedBlocks; }
int getTotalBlocks() { return NUM_BLOCKS; }

static void printNumber(int num)
{
    if(num == 0) { printChar('0'); return; }
    char buf[12];
    int i = 0;
    while(num > 0) { buf[i++] = '0' + (num % 10); num /= 10; }
    for(int k = i - 1; k >= 0; k--) printChar(buf[k]);
}

void printMemoryStatus()
{
    printString("+---------------------------------------+");
    printString("|   NetNinjas OS  -  Memory Status      |");
    printString("+---------------------------------------+");
    printStringNoNewLine("|  Block size : 512 bytes               |");
    printString("");
    printStringNoNewLine("|  Total  : ");
    printNumber(NUM_BLOCKS);
    printString(" blocks (128KB)            |");
    printStringNoNewLine("|  Used   : ");
    printNumber(usedBlocks);
    printStringNoNewLine(" blocks (");
    printNumber(usedBlocks * BLOCK_SIZE);
    printString(" bytes)         |");
    printStringNoNewLine("|  Free   : ");
    printNumber(NUM_BLOCKS - usedBlocks);
    printString(" blocks                    |");
    printString("+---------------------------------------+");
}
