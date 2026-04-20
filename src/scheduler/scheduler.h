#ifndef SCHEDULER_H
#define SCHEDULER_H

void initScheduler();
int  addProcess(char* name, int priority);
void killProcess(int id);
void schedule();
void printProcessStatus();

#endif
