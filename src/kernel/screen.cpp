#include "screen.h"

static unsigned short* videoMemory = (unsigned short*)0xB8000;
static int cursorX = 0;
static int cursorY = 0;

// Current color: high byte of VGA cell
// Format: bits 7-4 = background, bits 3-0 = foreground
// Default: 0x0F = black background, white text
static unsigned char currentColor = 0x0F;

static inline void outb(unsigned short port, unsigned char value)
{
    asm volatile("outb %0, %1" : : "a"(value), "Nd"(port));
}

static inline unsigned char inb(unsigned short port)
{
    unsigned char value;
    asm volatile("inb %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

void updateCursor()
{
    unsigned short pos = (unsigned short)(cursorY * 80 + cursorX);
    outb(0x3D4, 0x0F);
    outb(0x3D5, (unsigned char)(pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (unsigned char)((pos >> 8) & 0xFF));
}

void hideCursor()
{
    outb(0x3D4, 0x0A);
    outb(0x3D5, 0x20);
}

// Change background color and repaint the whole screen
// bg: 0=black 1=blue 2=green 3=cyan 4=red 5=magenta 6=brown 7=grey
void setColor(unsigned char bg)
{
    if(bg > 7) bg = 0;
    // foreground always white (15)
    currentColor = (unsigned char)((bg << 4) | 0x0F);

    // Repaint every existing character with the new color
    for(int i = 0; i < 80 * 25; i++)
    {
        unsigned char ch = (unsigned char)(videoMemory[i] & 0xFF);
        videoMemory[i] = (unsigned short)((currentColor << 8) | ch);
    }
}

void printChar(char c)
{
    if(c == '\n')
    {
        cursorX = 0;
        cursorY++;
    }
    else if(c == '\b')
    {
        if(cursorX > 0)
        {
            cursorX--;
            videoMemory[cursorY * 80 + cursorX] =
                (unsigned short)((currentColor << 8) | ' ');
        }
    }
    else
    {
        videoMemory[cursorY * 80 + cursorX] =
            (unsigned short)((currentColor << 8) | (unsigned char)c);
        cursorX++;
        if(cursorX >= 80)
        {
            cursorX = 0;
            cursorY++;
        }
    }

    if(cursorY >= 25)
    {
        for(int i = 0; i < 24 * 80; i++)
            videoMemory[i] = videoMemory[i + 80];
        for(int i = 24 * 80; i < 25 * 80; i++)
            videoMemory[i] = (unsigned short)((currentColor << 8) | ' ');
        cursorY = 24;
    }

    updateCursor();
}

void printString(const char* str)
{
    for(int i = 0; str[i] != '\0'; i++)
        printChar(str[i]);
    printChar('\n');
}

void printStringNoNewLine(const char* str)
{
    for(int i = 0; str[i] != '\0'; i++)
        printChar(str[i]);
}

void clearScreen()
{
    for(int i = 0; i < 80 * 25; i++)
        videoMemory[i] = (unsigned short)((currentColor << 8) | ' ');
    cursorX = 0;
    cursorY = 0;
    updateCursor();
}

void drawMouseStatus(int x, int y, int leftBtn, int rightBtn)
{
    int savedX = cursorX;
    int savedY = cursorY;

    char buf[25];
    int i = 0;
    buf[i++] = 'M'; buf[i++] = 'O'; buf[i++] = 'U';
    buf[i++] = 'S'; buf[i++] = 'E'; buf[i++] = ' ';
    buf[i++] = 'X'; buf[i++] = ':';
    buf[i++] = (x >= 10) ? (char)('0' + x / 10) : '0';
    buf[i++] = (char)('0' + x % 10);
    buf[i++] = ' '; buf[i++] = 'Y'; buf[i++] = ':';
    buf[i++] = (y >= 10) ? (char)('0' + y / 10) : '0';
    buf[i++] = (char)('0' + y % 10);
    buf[i++] = ' '; buf[i++] = 'L'; buf[i++] = ':';
    buf[i++] = (char)('0' + (leftBtn  ? 1 : 0));
    buf[i++] = ' '; buf[i++] = 'R'; buf[i++] = ':';
    buf[i++] = (char)('0' + (rightBtn ? 1 : 0));
    buf[i]   = '\0';

    // Mouse bar always green bg, white text (0x2F)
    for(int k = 0; buf[k] != '\0'; k++)
        videoMemory[56 + k] = (unsigned short)(0x2F00 | (unsigned char)buf[k]);

    cursorX = savedX;
    cursorY = savedY;
    updateCursor();
}
