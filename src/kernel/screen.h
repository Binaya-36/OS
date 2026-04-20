#ifndef SCREEN_H
#define SCREEN_H

void printChar(char c);
void printString(const char* str);
void printStringNoNewLine(const char* str);
void clearScreen();
void updateCursor();
void hideCursor();
void setColor(unsigned char color);
void drawMouseStatus(int x, int y, int leftBtn, int rightBtn);

#endif
