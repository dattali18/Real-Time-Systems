#ifndef SMARTS25_H
#define SMARTS25_H

#include <conio.h>
#include <stdio.h>
#include <dos.h>
#include <values.h>
#include <iostream.h>

#define true 1
#define false 0
#define timerInt 0x08
#define userInt 0x60
#define MaxTask 15
#define MaxStack 512

enum taskStatus { READY, NOT_ACTIVE, SUSPENDED, SLEEP, UNDEFINED };

/// MARK: PriorityQueue

class PriorityQueue {
    int queue[MaxTask];
    int size;

    void sort();

public:
    PriorityQueue() { size = 0; }

    void push(int taskNum);
    
    int pop();

    int is_empty() { return size == 0; }

    int search(int taskNum);
};

/// MARK: Mutex

class Mutex {
    int owner;
    int s;
    int level;
    PriorityQueue waitingList;
    int inheritanceFlag;

public:
    Mutex();

    Mutex(int inheritance);

    void acquire();

    void release();
};

/// MARK: Event

class Event {
private:
    int flag;         // arrived flag
    void *data;       // the parameter
    char source;      // source task name
    char dest;        // destination task name
    int senderWait;   // tells if the sender has to wait till the reciver receives
    int testAndSet(); // test and set 'flag' in an 'atomic transaction'
public:
    Event();
    // sends an event to 'dest', if 'synch' is true waits until the receiver gets
    // it.
    void send(char dest, void *param, int synch);
    // waits for the event, and returns the parameter.
    void *wait(char &sourceP); //'sourceP' will contain the task sender ID.
    // check for for an arrived event. 'sourceP' will contain the task sender ID.
    int arrived(char &sourceP);
    // resets the event
    void reset();
};

/// MARK: Task


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

/// MARK: Parallelism


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
        return context[currentTask].getCurrentPriority();
    } // get the current task priority

    int getCurrentPriority(int i) {
        return context[i].getCurrentPriority();
    }

    void setPriority(int taskNum, int priority) {
        context[taskNum].setPriority(priority);
    } // set the task priority

    void resetPriority(int taskNum) {
        context[taskNum].resetPriority();
    } // reset the task priority to the original
};
    

// reads the number of the remaining clocks from the timer register
extern unsigned getTimerClocks();
// external functions for 'externalFunctions' function
void far interrupt timerInterruptHandler(...);

void scheduler();

void myTaskEnd();

int roundRobin();

int edf();

int rms();

// main scheduler object
extern Parallelism SMARTS;

#endif // SMARTS25_H