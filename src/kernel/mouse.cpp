#include "mouse.h"
#include "screen.h"

static inline void outb(unsigned short port, unsigned char val)
{
    asm volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline unsigned char inb(unsigned short port)
{
    unsigned char val;
    asm volatile("inb %1, %0" : "=a"(val) : "Nd"(port));
    return val;
}

static void ps2WaitWrite()
{
    int t = 100000;
    while(t-- > 0)
        if((inb(0x64) & 0x02) == 0) return;
}

static void ps2WaitRead()
{
    int t = 100000;
    while(t-- > 0)
        if(inb(0x64) & 0x01) return;
}

static void mouseSendCmd(unsigned char cmd)
{
    ps2WaitWrite();
    outb(0x64, 0xD4);
    ps2WaitWrite();
    outb(0x60, cmd);
    ps2WaitRead();
    inb(0x60);
}

static int mouseX     = 40;
static int mouseY     = 12;
static int mouseLeft  = 0;
static int mouseRight = 0;

static unsigned char packetBuf[3];
static int           packetIdx = 0;

void initMouse()
{
    ps2WaitWrite();
    outb(0x64, 0xA8);

    ps2WaitWrite();
    outb(0x64, 0x20);
    ps2WaitRead();
    unsigned char config = inb(0x60);
    config |= 0x02;
    config &= (unsigned char)(~0x20);
    ps2WaitWrite();
    outb(0x64, 0x60);
    ps2WaitWrite();
    outb(0x60, config);

    mouseSendCmd(0xF6);
    mouseSendCmd(0xF4);

    drawMouseStatus(mouseX, mouseY, mouseLeft, mouseRight);
}

void handleMouse(unsigned char data)
{
    if(packetIdx == 0 && !(data & 0x08))
        return;

    packetBuf[packetIdx++] = data;

    if(packetIdx < 3)
        return;

    packetIdx = 0;

    unsigned char flags = packetBuf[0];
    int dx = (int)(unsigned int)packetBuf[1];
    int dy = (int)(unsigned int)packetBuf[2];

    if(flags & 0x10) dx |= 0xFFFFFF00;
    if(flags & 0x20) dy |= 0xFFFFFF00;
    if(flags & 0xC0) return;

    mouseLeft  = (flags & 0x01) ? 1 : 0;
    mouseRight = (flags & 0x02) ? 1 : 0;

    mouseX += dx / 8;
    mouseY -= dy / 8;

    if(mouseX <  0) mouseX =  0;
    if(mouseX > 79) mouseX = 79;
    if(mouseY <  0) mouseY =  0;
    if(mouseY > 24) mouseY = 24;

    drawMouseStatus(mouseX, mouseY, mouseLeft, mouseRight);
}

int getMouseX()     { return mouseX;     }
int getMouseY()     { return mouseY;     }
int getMouseLeft()  { return mouseLeft;  }
int getMouseRight() { return mouseRight; }
