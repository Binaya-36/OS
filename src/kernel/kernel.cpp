#include "screen.h"
#include "interrupts.h"
#include "keyboard.h"
#include "mouse.h"
#include "../memory/memory.h"
#include "../scheduler/scheduler.h"

static int stringEquals(const char* a, const char* b)
{
    int i = 0;
    while(a[i] != '\0' && b[i] != '\0')
    {
        if(a[i] != b[i]) return 0;
        i++;
    }
    return a[i] == b[i];
}

static int charToDigit(char c)
{
    if(c >= '0' && c <= '9') return c - '0';
    return -1;
}

static void showMenu()
{
    printString("+---------------------------------------+");
    printString("|   NetNinjas OS  -  Commands           |");
    printString("+---------------------------------------+");
    printString("|  help         - Show this menu        |");
    printString("|  memory       - Memory status         |");
    printString("|  procs        - Process status        |");
    printString("|  run <name>   - Start a new process   |");
    printString("|  kill <id>    - Kill a process        |");
    printString("|  color <name> - Change background     |");
    printString("|    black / blue / cyan / magenta      |");
    printString("|  mouse        - Mouse position        |");
    printString("|  team         - Team info             |");
    printString("|  clear        - Clear screen          |");
    printString("|  alloc        - Test memory alloc     |");
    printString("+---------------------------------------+");
}

static char* parseRunCmd(char* cmd)
{
    if(cmd[0]=='r' && cmd[1]=='u' && cmd[2]=='n' && cmd[3]==' ')
        return cmd + 4;
    return 0;
}

static int parseKillCmd(char* cmd)
{
    if(cmd[0]=='k' && cmd[1]=='i' && cmd[2]=='l' && cmd[3]=='l' && cmd[4]==' ')
        return charToDigit(cmd[5]);
    return -1;
}

// Returns bg color index or -1 if not a color command or -2 if bad color name
static int parseColorCmd(char* cmd)
{
    if(!(cmd[0]=='c' && cmd[1]=='o' && cmd[2]=='l' &&
         cmd[3]=='o' && cmd[4]=='r' && cmd[5]==' '))
        return -1;

    char* name = cmd + 6;

    if(stringEquals(name, "black"))   return 0;
    if(stringEquals(name, "blue"))    return 1;
    if(stringEquals(name, "cyan"))    return 3;
    if(stringEquals(name, "magenta")) return 5;

    return -2;
}

void handleCommand(char* command)
{
    if(stringEquals(command, "help"))
    {
        showMenu();
    }
    else if(stringEquals(command, "memory"))
    {
        printMemoryStatus();
    }
    else if(stringEquals(command, "procs"))
    {
        printProcessStatus();
    }
    else if(parseRunCmd(command) != 0)
    {
        char* name = parseRunCmd(command);
        int id = addProcess(name, 2);
        if(id >= 0)
        {
            printStringNoNewLine("Process '");
            printStringNoNewLine(name);
            printString("' started. Memory allocated: 512 bytes.");
            printMemoryStatus();
        }
    }
    else if(parseKillCmd(command) >= 0)
    {
        int id = parseKillCmd(command);
        killProcess(id);
        printStringNoNewLine("Process ");
        printChar((char)('0' + id));
        printString(" terminated. Memory freed.");
        printMemoryStatus();
    }
    else if(parseColorCmd(command) != -1)
    {
        int bg = parseColorCmd(command);
        if(bg == -2)
        {
            printString("Invalid color. Available: black  blue  cyan  magenta");
        }
        else
        {
            setColor((unsigned char)bg);
            clearScreen();
            printStringNoNewLine("> ");
        }
    }
    else if(stringEquals(command, "mouse"))
    {
        int mx = getMouseX();
        int my = getMouseY();
        printString("+---------------------------------------+");
        printString("|   NetNinjas OS  -  Mouse Status       |");
        printString("+---------------------------------------+");
        printStringNoNewLine("|  X position : ");
        if(mx >= 10) printChar((char)('0' + mx / 10));
        else          printChar('0');
        printChar((char)('0' + mx % 10));
        printString("                         |");
        printStringNoNewLine("|  Y position : ");
        if(my >= 10) printChar((char)('0' + my / 10));
        else          printChar('0');
        printChar((char)('0' + my % 10));
        printString("                         |");
        printStringNoNewLine("|  Left btn   : ");
        printString(getMouseLeft()  ? "pressed                |" : "not pressed            |");
        printStringNoNewLine("|  Right btn  : ");
        printString(getMouseRight() ? "pressed                |" : "not pressed            |");
        printString("+---------------------------------------+");
    }
    else if(stringEquals(command, "team"))
    {
        printString("+---------------------------------------+");
        printString("|   NetNinjas OS  -  Team Info          |");
        printString("+---------------------------------------+");
        printString("|  Team  : Net Ninjas                   |");
        printString("|  Members:                             |");
        printString("|    >> Ayusha                          |");
        printString("|    >> Binaya                          |");
        printString("|    >> Prijina                         |");
        printString("+---------------------------------------+");
    }
    else if(stringEquals(command, "clear"))
    {
        clearScreen();
        printStringNoNewLine("> ");
    }
    else if(stringEquals(command, "alloc"))
    {
        printString("Allocating 512 bytes...");
        void* mem = allocateMemory(512);
        if(mem != 0)
        {
            printString("Allocation successful!");
            printMemoryStatus();
            freeMemory(mem);
            printString("Memory freed.");
        }
        else
        {
            printString("ERROR: allocation failed!");
        }
    }
    else
    {
        printStringNoNewLine("Unknown command: '");
        printStringNoNewLine(command);
        printString("'  (type 'help')");
    }
}

extern "C" void kernelMain(void* multiboot_structure, unsigned int magicNumber)
{
    (void)multiboot_structure;
    (void)magicNumber;

    clearScreen();

    printString("+===========================================+");
    printString("|                                           |");
    printString("|    WELCOME TO NET NINJAS OS               |");
    printString("|    Team: Ayusha  Binaya  Prijina          |");
    printString("|                                           |");
    printString("+===========================================+");
    printString("");

    InterruptManager interrupts;
    printString("  [OK] Interrupts ready.");

    initMemory();
    printString("  [OK] Memory ready. 256 x 512B blocks.");

    initScheduler();
    char p1[] = "Music Player";
    char p2[] = "File Manager";
    char p3[] = "Text Editor";
    addProcess(p1, 1);
    addProcess(p2, 2);
    addProcess(p3, 3);
    printString("  [OK] Scheduler ready. 3 processes loaded.");

    setCommandHandler(&handleCommand);
    printString("  [OK] Keyboard driver ready.");

    initMouse();
    printString("  [OK] Mouse driver ready.");

    printString("");
    printString("+===========================================+");
    printString("|   NetNinjas OS is ready!                 |");
    printString("|   Type 'help' to see all commands.       |");
    printString("+===========================================+");
    printString("");
    printStringNoNewLine("> ");

    while(1) {}
}
