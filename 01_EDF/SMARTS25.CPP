#include "PARAL25.H"
#include "TASK25.H"
#include "SMARTS25.H"

unsigned getTimerClocks() {
  // Gets the remaining clocks of the timer register

  unsigned clocks;
  /* latch counter #0 */
  outportb(0x43, 0x00);
  /* read counter #0 low byte */
  clocks = inportb(0x40);
  /* read counter #0 high byte */
  clocks += inportb(0x40) << 8;
  return clocks;
}

Parallelism::Parallelism() {
  currentTask = 0;
  sleepTasks = 0;
  activeTasks = 0;
  totalTasks = 0;
  deadlock = false;
  contextSwitchFlag = true;
  endOfTimeSlice = true;
}

void Parallelism::externalFunctions(
    void interrupt (*timerInterruptHandler)(...), void far *scheduler,
    void far *userTaskEnd, int far (*algorithm)()) {
  // Sets the external functions

  this->timerInterruptHandler = timerInterruptHandler;
  this->scheduler = scheduler;
  this->userTaskEnd = userTaskEnd;
  this->algorithm = algorithm;

  contextSched.declare(scheduler, userTaskEnd,
                       'S'); // prepare the stack of the scheduler task

  for (int i = MaxStack - 1; i >= (MaxStack - 14); i--)
    schedCopy[i] = contextSched.stack[i];
}

int Parallelism::declareTask(void far *code, char name, int deadline,
                             int cycles) {
  // Insert a new task entry in SMARTS context array [ ] with its execution time
  // and deadline time

  if (totalTasks < MaxTask - 1) {
    context[totalTasks++].declare(code, userTaskEnd, name, deadline, cycles);
    ++activeTasks;
    return true;
  } else
    return false;
}

void Parallelism::updateDeadlines() {
  for (int i = 0; i < totalTasks - 1; ++i) {
    context[i].updateDeadline();

    if (context[i].deadline == 0 && context[i].cycles > 0) {
      context[i].redeclare();
    }
  }
} // update the deadlines of all tasks

void Parallelism::runTheTasks() {
  // Start running all tasks declared in SMARTS.

  context[totalTasks].status = READY; // for task 'runTheTasks' (IDLE task)
  context[totalTasks].priority = MAXINT;
  context[totalTasks].currentPriority = MAXINT;

  currentTask = totalTasks;

  asm cli; // forbids interrupts (clear interrupts) while changing the interrupt
           // vect saves the original BIOS userInt in our variable
           // 'userIntAddress' to be restored at the end
  userIntAddress = getvect(
      userInt); // BIOS userInt 0x60  (unused by PC)
                // puts the normal BIOS timerInt into the unused userInt address
  setvect(userInt, getvect(timerInt)); // timerInt 0x08

  // sets our SMARTS external function 'timerInterruptHandler' as the new PC
  // hard interrupt time handler
  setvect(timerInt, timerInterruptHandler);
  asm sti; // allows back interrupts (set interrupts)

  // waits for end of runTheTasks (end of all the tasks)
  while (true) {
    if (error) {
      textcolor(RED);
      cprintf("\n\n\rExit : error");
      break;
    }
    if (deadlock) {
      textcolor(RED);
      cprintf("\n\n\rExit : deadlock");
      break;
    }
    if (activeTasks == 0) {
      cprintf("\n\n\rExit : finish");
      break;
    }
  }

  // restore the original BIOS 'interrupt vector' at the end before returning to
  // regular DOS
  asm cli;                             // no interrupts
  setvect(timerInt, getvect(userInt)); // restore original BIOS time handler
  setvect(userInt, userIntAddress);    // restore original BIOS userInt
  asm sti;                             // yes interrupts
}

void Parallelism::callScheduler() {
  // Return the control to the scheduler, this sets ON the software interrupt
  // ProgInt flag
  setProgInt();
  asm int timerInt;
}

void Parallelism::restoreSchedStack() {
  // Restore the scheduler stack
  for (int i = MaxStack - 1; i >= (MaxStack - 14); i--)
    contextSched.stack[i] = schedCopy[i];
}

int Parallelism::getCurrentTask() { return currentTask; }

void Parallelism::setCurrentTask(int taskNum) {
  // Sets the next task to be run

  if (taskNum <= totalTasks)
    currentTask = taskNum;
}

int Parallelism::getTotalTasks() {
  // Gets total tasks declared
  return totalTasks;
}

int Parallelism::getDeadlock() { return deadlock; }

void Parallelism::setDeadlock() { deadlock = true; }

int Parallelism::contextSwitchOn() {
  // flag which enables context switch

  if (endOfTimeSlice) // is current time slice finished ?
  {
    endOfTimeSlice = false;
    contextSwitchFlag = true;
    callScheduler(); // return control to the scheduler
    return 1;
  }
  contextSwitchFlag = true;
  return 0;
}

void Parallelism::contextSwitchOff() {
  // Disable context switch
  contextSwitchFlag = false;
}

int Parallelism::getContextSwitch() { return contextSwitchFlag; }

void Parallelism::setProgInt() {
  // flag indicates to the extern function 'timerInterruptHandler'
  // that this is an internal SMARTS software interrupt call,
  // and the original BIOS function has no to be called.
  progInt = true;
}

void Parallelism::resetProgInt() { progInt = false; }

int Parallelism::getProgInt() { return progInt; }

void Parallelism::setEndOfTimeSlice() {
  // flag indicates that when 'context switch' will be enabled,
  // it must also return the control to the scheduler.
  endOfTimeSlice = true;
}

char Parallelism::getName(int taskNum) // returns name found or ' ' if not
{
  return (taskNum <= totalTasks) ? context[taskNum].name : ' ';
}

char Parallelism::getCurrentName() { return context[currentTask].name; }

taskStatus Parallelism::getStatus(int taskNum) {
  // returns status or undefined if not found

  return (taskNum <= totalTasks) ? context[taskNum].status : UNDEFINED;
}

taskStatus Parallelism::getCurrentStatus() {
  return context[currentTask].status;
}

void Parallelism::resume(int taskNum) {
  if (taskNum < totalTasks)
    context[taskNum].status = READY;
}

void Parallelism::resume(char taskName) {
  for (int i = 0; i < totalTasks; ++i)
    if (context[i].name == taskName)
      context[i].status = READY;
}

void Parallelism::setCurrentNotActive() {
  context[currentTask].status = NOT_ACTIVE;
  if (context[currentTask].cycles <= 1)
    --activeTasks;
}
void Parallelism::suspended() {
  context[currentTask].status = SUSPENDED;
  callScheduler();
}

void Parallelism::incrPriority(int taskNum) {
  if (taskNum < totalTasks)
    context[taskNum].incrPriority();
}
void Parallelism::setOriginalPriority(int taskNum) {
  if (taskNum < totalTasks)
    context[taskNum].setOriginalPriority();
}

void Parallelism::setCurrentOriginalPriority() {
  context[currentTask].setOriginalPriority();
}

Event *Parallelism::getExpectedEvent(int taskNum) {
  // returns *Event  or  NULL  if not found

  return (taskNum <= totalTasks) ? context[taskNum].expectedEvent : NULL;
}

Event *Parallelism::getCurrentExpectedEvent() {
  return context[currentTask].expectedEvent;
}

void Parallelism::setCurrentExpectedEvent(Event *expectedEvent) {
  context[currentTask].expectedEvent = expectedEvent;
}

void Parallelism::sleep(int t) {
  // Current task sleeps for 't' milliseconds

  if (t < MAXINT) {
    context[currentTask].sleepCount = t / 55 + 1;
    context[currentTask].status = SLEEP;
    ++sleepTasks;
    callScheduler(); // return control to scheduler
  }
}

void Parallelism::sleepDecr(int taskNum) {
  if (taskNum < totalTasks)
    context[taskNum].sleepDecr();
}

void Parallelism::getCurrentStack(unsigned &StackSeg, unsigned &StackPtr) {
  // Load current task stack pointer
  if (context[currentTask].is_restore) {
    for (int i = MaxStack - 1; i >= (MaxStack - 14); i--)
      context[currentTask].stack[i] = context[currentTask].stackCopy[i];

    StackSeg = context[currentTask].stackSegCopy;
    StackPtr = context[currentTask].stackPtrCopy;

    context[currentTask].is_restore = false;
  } else {
    StackSeg = context[currentTask].stackSeg;
    StackPtr = context[currentTask].stackPtr;
  }
}

void Parallelism::setCurrentStack(unsigned StackSeg, unsigned StackPtr) {
  // Save current task stack pointer

  context[currentTask].stackSeg = StackSeg;
  context[currentTask].stackPtr = StackPtr;
}

void Parallelism::getSchedStack(unsigned &StackSeg, unsigned &StackPtr) {
  // Load scheduler  stack pointer

  StackSeg = contextSched.stackSeg;
  StackPtr = contextSched.stackPtr;
}

void Parallelism::handleTimers() {
  // handling of the sleep status mode
  /// MARK: update deadline

  this->updateDeadlines(); // update the deadlines of all tasks

  for (int i = totalTasks - 1; i >= 0; --i) {
    if (getStatus(i) == SLEEP) {
      sleepDecr(i);
      if (getStatus(i) == READY)
        --sleepTasks;
    }
  }
}

void Parallelism::taskEnd() {
  // This function is called after the last operation of a task, instead of
  // function return
  SMARTS.setCurrentNotActive();
  SMARTS.callScheduler();
}