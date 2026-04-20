#ifndef KEYBOARD_H
#define KEYBOARD_H

void handleKeyboard(unsigned char scanCode);
void setCommandHandler(void (*handler)(char*));

#endif
