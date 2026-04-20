#include "interrupts.h"
#include "screen.h"
#include "keyboard.h"
#include "mouse.h"
#include "../scheduler/scheduler.h"

struct IDTEntry
{
    unsigned short handlerAddressLow;
    unsigned short selector;
    unsigned char  zero;
    unsigned char  attributes;
    unsigned short handlerAddressHigh;
} __attribute__((packed));

struct IDTPointer
{
    unsigned short size;
    unsigned int   address;
} __attribute__((packed));

struct InterruptFrame
{
    unsigned int ip;
    unsigned int cs;
    unsigned int flags;
};

static IDTEntry   idt[256];
static IDTPointer idtPointer;

static void setIDTEntry(int interrupt, void (*handler)())
{
    unsigned int addr = (unsigned int)handler;
    idt[interrupt].handlerAddressLow  = (unsigned short)(addr & 0xFFFF);
    idt[interrupt].selector           = 0x08;
    idt[interrupt].zero               = 0;
    idt[interrupt].attributes         = 0x8E;
    idt[interrupt].handlerAddressHigh = (unsigned short)((addr >> 16) & 0xFFFF);
}

void __attribute__((interrupt)) timerHandler(struct InterruptFrame* frame)
{
    (void)frame;
    schedule();
    asm volatile("outb %0, %1" : : "a"((unsigned char)0x20), "Nd"((unsigned short)0x20));
}

void __attribute__((interrupt)) keyboardHandler(struct InterruptFrame* frame)
{
    (void)frame;
    unsigned char scanCode;
    asm volatile("inb %1, %0" : "=a"(scanCode) : "Nd"((unsigned short)0x60));
    handleKeyboard(scanCode);
    asm volatile("outb %0, %1" : : "a"((unsigned char)0x20), "Nd"((unsigned short)0x20));
}

void __attribute__((interrupt)) mouseHandler(struct InterruptFrame* frame)
{
    (void)frame;
    unsigned char data;
    asm volatile("inb %1, %0" : "=a"(data) : "Nd"((unsigned short)0x60));
    handleMouse(data);
    asm volatile("outb %0, %1" : : "a"((unsigned char)0x20), "Nd"((unsigned short)0xA0));
    asm volatile("outb %0, %1" : : "a"((unsigned char)0x20), "Nd"((unsigned short)0x20));
}

void __attribute__((interrupt)) ignoreInterruptHandler(struct InterruptFrame* frame)
{
    (void)frame;
    asm volatile("outb %0, %1" : : "a"((unsigned char)0x20), "Nd"((unsigned short)0x20));
}

InterruptManager::InterruptManager()
{
    asm volatile("outb %0, %1"::"a"((unsigned char)0x11),"Nd"((unsigned short)0x20));
    asm volatile("outb %0, %1"::"a"((unsigned char)0x11),"Nd"((unsigned short)0xA0));
    asm volatile("outb %0, %1"::"a"((unsigned char)0x20),"Nd"((unsigned short)0x21));
    asm volatile("outb %0, %1"::"a"((unsigned char)0x28),"Nd"((unsigned short)0xA1));
    asm volatile("outb %0, %1"::"a"((unsigned char)0x04),"Nd"((unsigned short)0x21));
    asm volatile("outb %0, %1"::"a"((unsigned char)0x02),"Nd"((unsigned short)0xA1));
    asm volatile("outb %0, %1"::"a"((unsigned char)0x01),"Nd"((unsigned short)0x21));
    asm volatile("outb %0, %1"::"a"((unsigned char)0x01),"Nd"((unsigned short)0xA1));
    asm volatile("outb %0, %1"::"a"((unsigned char)0x00),"Nd"((unsigned short)0x21));
    asm volatile("outb %0, %1"::"a"((unsigned char)0x00),"Nd"((unsigned short)0xA1));

    for(int i = 0; i < 256; i++)
        setIDTEntry(i, (void(*)())&ignoreInterruptHandler);

    setIDTEntry(0x20, (void(*)())&timerHandler);
    setIDTEntry(0x21, (void(*)())&keyboardHandler);
    setIDTEntry(0x2C, (void(*)())&mouseHandler);

    idtPointer.size    = (unsigned short)(256 * sizeof(IDTEntry) - 1);
    idtPointer.address = (unsigned int)idt;
    asm volatile("lidt %0" : : "m"(idtPointer));
    asm volatile("sti");
}

void InterruptManager::ignoreInterrupt() {}
