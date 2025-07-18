#ifndef TASK25_H
#define TASK25_H

#include "CONST25.H"
#include "EVENT25.H"

class Task {
public:
  char name;                   // task name
  unsigned stack[MaxStack];    // the stack
  unsigned stackSeg, stackPtr; // the stack pointer
  int currentPriority;         // the priority of the task
  int priority;                // the original priority
  taskStatus status;           // the status of the task
  Event *expectedEvent;        // the event that this task is waiting for
  int sleepCount;              // the sleep remainder time
  Task();
  // void declare(void far *code, void far *taskEnd, char name);
  void sleepDecr();
  void incrPriority();
  void setOriginalPriority();

  int deadline;         // the deadline
  int originalDeadline; // the original deadline time
  int cycles;           // the execution time
  int is_restore;

  unsigned stackCopy[MaxStack];        // a copy of the stack
  unsigned stackSegCopy, stackPtrCopy; // a copy of the stack pointer

  int getDeadline() { return deadline; }
  void updateDeadline() { deadline--; }

  int getOriginalDeadline() { return originalDeadline; }

  int getCycles() { return cycles; }    // get the execution time
  void setCycles(int c) { cycles = c; } // set the execution time

  void declare(void far *code, void far *taskEnd, char name, int deadline = 0,
    int cycles = 0, int priority = 0);

  void redeclare();

  int getCurrentPriority() { return priority; }
  void setCurrentPriority(int p) {
    priority = p;
    currentPriority = p;
  } // set the priority of the task

  int contextSwitchFlag;

  void contextSwitchOff() {
    contextSwitchFlag = false; // disable context switch
  }

  void contextSwitchOn() {
    contextSwitchFlag = true; // enable context switch
  }

  void setPriority(int p) {
    currentPriority = p;
  }

  void resetPriority() {
    currentPriority = priority; // reset the priority to the original
  }
};


#endif // TASK25_H