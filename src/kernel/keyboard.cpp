#include "keyboard.h"
#include "screen.h"

static char command[80];
static int commandLength = 0;

static char scanCodeTable[] =
{
    0,     // 0
    0,     // 1  Escape
    '1',   // 2
    '2',   // 3
    '3',   // 4
    '4',   // 5
    '5',   // 6
    '6',   // 7
    '7',   // 8
    '8',   // 9
    '9',   // 10
    '0',   // 11
    '-',   // 12
    '=',   // 13
    '\b',  // 14 Backspace
    0,     // 15 Tab
    'q',   // 16
    'w',   // 17
    'e',   // 18
    'r',   // 19
    't',   // 20
    'y',   // 21
    'u',   // 22
    'i',   // 23
    'o',   // 24
    'p',   // 25
    '[',   // 26
    ']',   // 27
    '\n',  // 28 Enter
    0,     // 29 Left Control
    'a',   // 30
    's',   // 31
    'd',   // 32
    'f',   // 33
    'g',   // 34
    'h',   // 35
    'j',   // 36
    'k',   // 37
    'l',   // 38
    ';',   // 39
    '\'',  // 40
    '`',   // 41
    0,     // 42 Left Shift
    '\\',  // 43
    'z',   // 44
    'x',   // 45
    'c',   // 46
    'v',   // 47
    'b',   // 48
    'n',   // 49
    'm',   // 50
    ',',   // 51
    '.',   // 52
    '/',   // 53
    0,     // 54 Right Shift
    0,     // 55
    0,     // 56 Left Alt
    ' ',   // 57 Space
};

static void (*commandHandler)(char*) = 0;

void setCommandHandler(void (*handler)(char*))
{
    commandHandler = handler;
}

void handleKeyboard(unsigned char scanCode)
{
    if(scanCode & 0x80) return;

    if(scanCode < sizeof(scanCodeTable))
    {
        char c = scanCodeTable[scanCode];

        if(c == '\n')
        {
            printChar('\n');
            command[commandLength] = '\0';
            if(commandHandler != 0 && commandLength > 0)
                commandHandler(command);
            commandLength = 0;
            command[0] = '\0';
            printStringNoNewLine("> ");
        }
        else if(c == '\b')
        {
            if(commandLength > 0)
            {
                commandLength--;
                command[commandLength] = '\0';
                printChar('\b');
            }
        }
        else if(c != 0)
        {
            if(commandLength < 79)
            {
                command[commandLength++] = c;
                command[commandLength]   = '\0';
                printChar(c);
            }
        }
    }
}
