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

  /// MARK: addition for the class assignment
  int deadline;         // the deadline
  int originalDeadline; // the original deadline time
  int cycles;           // the execution time
  int is_restore;

  unsigned stackCopy[MaxStack];        // a copy of the stack
  unsigned stackSegCopy, stackPtrCopy; // a copy of the stack pointer

  int getDeadline() { return deadline; }
  void updateDeadline() { deadline--; }

  int getCycles() { return cycles; }    // get the execution time
  void setCycles(int c) { cycles = c; } // set the execution time

  void declare(void far *code, void far *taskEnd, char name, int deadline = 0,
               int cycles = 0);

  void redeclare();
};


#endif // TASK25_H