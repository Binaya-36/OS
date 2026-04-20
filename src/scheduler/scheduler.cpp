#include "scheduler.h"
#include "../kernel/screen.h"
#include "../memory/memory.h"

#define MAX_PROCESSES   10
#define PROCESS_FREE     0
#define PROCESS_READY    1
#define PROCESS_RUNNING  2
#define PROCESS_BLOCKED  3

struct Process
{
    int   id;
    int   state;
    int   priority;
    char  name[32];
    int   timeSlice;
    int   ticksLeft;
    int   timeUsed;
    void* memBlock;    // memory block allocated to this process
};

static Process processes[MAX_PROCESSES];
static int currentProcess = 0;
static int activeCount    = 0;

void initScheduler()
{
    for(int i = 0; i < MAX_PROCESSES; i++)
    {
        processes[i].state    = PROCESS_FREE;
        processes[i].id       = -1;
        processes[i].ticksLeft = 0;
        processes[i].timeUsed  = 0;
        processes[i].memBlock  = 0;
    }
    currentProcess = 0;
    activeCount    = 0;
}

int addProcess(char* name, int priority)
{
    for(int i = 0; i < MAX_PROCESSES; i++)
    {
        if(processes[i].state == PROCESS_FREE)
        {
            // Allocate memory for this process (512 bytes = 1 block)
            void* mem = allocateMemory(512);
            if(mem == 0)
            {
                printString("ERROR: Not enough memory to create process!");
                return -1;
            }

            processes[i].id        = i;
            processes[i].state     = PROCESS_READY;
            processes[i].priority  = priority;
            processes[i].timeSlice = priority * 5;
            processes[i].ticksLeft = processes[i].timeSlice;
            processes[i].timeUsed  = 0;
            processes[i].memBlock  = mem;

            int j = 0;
            while(name[j] != '\0' && j < 31)
            {
                processes[i].name[j] = name[j];
                j++;
            }
            processes[i].name[j] = '\0';

            activeCount++;
            return processes[i].id;
        }
    }
    printString("ERROR: Process table full!");
    return -1;
}

void killProcess(int id)
{
    for(int i = 0; i < MAX_PROCESSES; i++)
    {
        if(processes[i].id == id && processes[i].state != PROCESS_FREE)
        {
            // Free the memory block this process was using
            if(processes[i].memBlock != 0)
            {
                freeMemory(processes[i].memBlock);
                processes[i].memBlock = 0;
            }
            processes[i].state = PROCESS_FREE;
            processes[i].id    = -1;
            activeCount--;
            if(activeCount < 0) activeCount = 0;

            // If we just killed the running process, reset to first available
            if(currentProcess == i)
                currentProcess = 0;
            return;
        }
    }
    printString("ERROR: Process not found!");
}

void schedule()
{
    if(activeCount == 0) return;

    if(processes[currentProcess].state == PROCESS_RUNNING)
    {
        processes[currentProcess].ticksLeft--;
        processes[currentProcess].timeUsed++;
        if(processes[currentProcess].ticksLeft > 0)
            return;
        processes[currentProcess].state     = PROCESS_READY;
        processes[currentProcess].ticksLeft = processes[currentProcess].timeSlice;
    }

    int bestIdx      = -1;
    int bestPriority = -1;

    for(int i = 0; i < MAX_PROCESSES; i++)
    {
        if(processes[i].state == PROCESS_READY && processes[i].priority > bestPriority)
        {
            bestPriority = processes[i].priority;
            bestIdx      = i;
        }
    }

    if(bestIdx != -1)
    {
        currentProcess                  = bestIdx;
        processes[currentProcess].state = PROCESS_RUNNING;
    }
}

static void printNum(int n)
{
    if(n == 0) { printChar('0'); return; }
    char buf[12];
    int i = 0;
    while(n > 0) { buf[i++] = '0' + (n % 10); n /= 10; }
    for(int k = i - 1; k >= 0; k--) printChar(buf[k]);
}

void printProcessStatus()
{
    printString("+---------------------------------------+");
    printString("|   NetNinjas OS  -  Process Status     |");
    printString("+---------------------------------------+");
    printString("|  ID  Name          Pri  Mem   State   |");
    printString("+---------------------------------------+");

    int found = 0;
    for(int i = 0; i < MAX_PROCESSES; i++)
    {
        if(processes[i].state != PROCESS_FREE)
        {
            found = 1;
            printStringNoNewLine("|  [");
            printNum(processes[i].id);
            printStringNoNewLine("] ");
            printStringNoNewLine(processes[i].name);
            printStringNoNewLine("  P:");
            printNum(processes[i].priority);
            printStringNoNewLine(" 512B  ");
            if(processes[i].state == PROCESS_RUNNING)
                printString("RUNNING  |");
            else if(processes[i].state == PROCESS_READY)
                printString("READY    |");
            else
                printString("BLOCKED  |");
        }
    }
    if(!found) printString("|  No active processes.                 |");
    printString("+---------------------------------------+");
    printStringNoNewLine("|  Memory used by processes: ");
    printNum(activeCount);
    printString(" x 512B  |");
    printString("+---------------------------------------+");
}
