#ifndef PARAL25_H
#define PARAL25_H

#include "CONST25.H"
#include "TASK25.H"
#include "EVENT25.H"

class Parallelism {
private:
  Task context[MaxTask];        // all tasks entries
  Task contextSched;            // scheduler entry
  unsigned schedCopy[MaxStack]; // a copy of the scheduler stack
  int totalTasks;               // total declared tasks
  int currentTask;              // time slice holding task
  int deadlock;                 // deadlock flag
  int error;                    // error flag
  int progInt;                  // 'program interrupt' flag
  int contextSwitchFlag;        // enables/disables context switch
  int endOfTimeSlice; // indicates that has to be a context switch as soon as
                      // possible a pointer to the new timer interrupt handler
  void interrupt (*timerInterruptHandler)(...);
  // a pointer to the original BIOS timer interrupt handler
  void interrupt (*userIntAddress)(...);
  void far *scheduler; // a pointer to the scheduler
  void far
      *userTaskEnd; // a pointer to the function called at the end of each task
public:
  int sleepTasks;  // indicates how many task is sleeping, and it can't be
                   // deadlock
  int activeTasks; // indicates how many task is activate
  int traceInd;    // debugging index
  long TScount;    // counts the clocks of the current task
  unsigned timerClocksBegin; // time start of the current task
  unsigned timerClocksEnd;   // time finish of the current task
  int far (*algorithm)();    // short-term scheduler algorithm function
  Parallelism();
  // sets the addresses of the external functions
  void externalFunctions(void interrupt (*timerInterruptHandler)(...),
                         void far *scheduler, void far *userTaskEnd,
                         int far (*algorithm)());
  // int declareTask(void far *code, char name); // inserts a new task entry
  int declareTask(void far *code, char name, int deadline = 0,
                  int cycles = 0, int priority = 0);  // inserts a new task entry with its
                                    // execution time and deadline time
  void runTheTasks();               // start running the tasks
  void callScheduler();             // return to the control to the scheduler
  void restoreSchedStack();         // restore the 'scheduler' stack
  int getCurrentTask();             // get the current running task entry number
  void setCurrentTask(int taskNum); // set the next running task number
  int getTotalTasks();              // get total declared tasks
  int getDeadlock();                // get deadlock flag
  void setDeadlock();               // set deadlock flag
  int contextSwitchOn();            // enable context switch
  void contextSwitchOff();          // disable context switch
  int getContextSwitch();           // get 'context switch' flag status
  void setProgInt();                // set the 'program interrupt' flag
  void resetProgInt();              // clear the 'program interrupt' flag
  int getProgInt();                 // get the 'program interrupt' flag status
  void setEndOfTimeSlice();         // set the 'end of time slice' flag
  char getName(int taskNum);
  char getCurrentName();
  taskStatus getStatus(int taskNum);
  taskStatus getCurrentStatus();
  void resume(int taskNum);
  void resume(char taskName);
  void setCurrentNotActive();
  void suspended();
  void incrPriority(int taskNum);
  void setOriginalPriority(int taskNum);
  void setCurrentOriginalPriority();
  Event *getExpectedEvent(int taskNum);
  Event *getCurrentExpectedEvent();
  void setCurrentExpectedEvent(Event *expectedEvent);
  void sleep(int t); // sleep current task for 't' milliseconds
  void sleepDecr(int taskNum);
  void getCurrentStack(unsigned &StackSeg, unsigned &StackPtr);
  void setCurrentStack(unsigned StackSeg, unsigned StackPtr);
  void getSchedStack(unsigned &StackSeg, unsigned &StackPtr);
  void handleTimers();
  void taskEnd();

  void updateDeadline(int taskNum) {
    context[taskNum].updateDeadline();
  } // update the deadline of the task

  void updateDeadlines();

  int getOriginalDeadline(int taskNum) {
    // return context[taskNum].getOriginalDeadline();
    return context[taskNum].originalDeadline;
  } // get the original deadline of the task

  int getCycles(int i) {
    return context[i].getCycles();
  } // get the execution time of the task

  void setCycles(int i) { context[i].setCycles(context[i].cycles); }

  int getDeadline(int taskNum) {
    // return context[taskNum].getDeadline();
    return context[taskNum].deadline;
  } // get the deadline of the task

  void setError() { this->error = true; } // set the error flag
  int getError() { return error; }        // get the error flag

  int getCurrentPriority() {
    return context[currentTask].currentPriority;
  } // get the current priority of the current task

  void setCurrentPriority(int priority) {
    context[currentTask].currentPriority = priority;
  } // set the current priority of the current task
};

#endif // PARAL25_H