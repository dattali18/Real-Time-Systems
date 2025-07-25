#include "SMARTS25.H"

Parallelism SMARTS = Parallelism();
static unsigned StackSegAct, StackPtrAct;

void far interrupt timerInterruptHandler(...) {
  // This function is called 18.2 times per second ( every ~~ 55 millisec).

  asm mov StackPtrAct, sp; // get the stack addr of the task interrupted
  asm mov ax, ss;          // by the clock or by a Program interrupt
  asm mov StackSegAct, ax;

  SMARTS.timerClocksEnd = getTimerClocks();

  // if it was called by the timer interrupt (hardware source),
  // it calls the original BIOS routine. not to change the PC work.
  if (!SMARTS.getProgInt()) {
    asm int userInt;
    SMARTS.handleTimers();
  }
  
  // if the 'context switch' flag=true, save the current task variables
  // and call the 'scheduler'
  if (SMARTS.getContextSwitch() || SMARTS.getProgInt()) {
    // save stack addr of interrupted task in
    SMARTS.resetProgInt(); // reset the 'program interrupt' flag
    // current stack
    SMARTS.setCurrentStack(StackSegAct, StackPtrAct);
    SMARTS.restoreSchedStack(); // prepare activation of the task "scheduler"
    SMARTS.getSchedStack(StackSegAct, StackPtrAct);
    asm mov ax, StackSegAct;
    asm mov ss, ax;
    asm mov sp, StackPtrAct;
  }
}

void scheduler() {
  // this also a parallel task
  // This function is called by hardware called
  // 'timerInterruptHandler' function or by software SMARTS
  // system's functions It selects & switches the next task to be
  // run.
  int nextTask, i;

  // calling to the 'short term scheduler' algorithm
  nextTask = SMARTS.algorithm();

  // checking for 'deadlock' : not found task to run and tasks remain suspended
  if (nextTask == SMARTS.getTotalTasks() && SMARTS.sleepTasks == 0 &&
      SMARTS.activeTasks > 0) {
    for (i = SMARTS.getTotalTasks() - 1; i >= 0; --i)
      if (SMARTS.getStatus(i) == SUSPENDED) {
        cprintf("\ntask %c  is suspended", SMARTS.getName(i));
        SMARTS.setDeadlock();
      }
  }
  // ok to run a next task
  SMARTS.setCurrentTask(nextTask);

  //  'dispatcher' to restore the next task stack addr and registers
  SMARTS.getCurrentStack(StackSegAct, StackPtrAct);

  asm mov ax, StackSegAct;
  asm mov ss, ax;
  asm mov sp, StackPtrAct;

  asm pop bp;
  asm pop di;
  asm pop si;
  asm pop ds;
  asm pop es;
  asm pop dx;
  asm pop cx;
  asm pop bx;
  asm pop ax;
  asm iret; // return from interrupt
}

void myTaskEnd() { SMARTS.taskEnd(); }
