#include "SMARTS25.H"
#include "TASK25.H"

Task::Task() {
  stack[MaxStack - 14] = _BP;
  stack[MaxStack - 13] = _DI;
  stack[MaxStack - 12] = _SI;
  stack[MaxStack - 11] = _DS;
  stack[MaxStack - 10] = _ES;
  stack[MaxStack - 9] = _DX;
  stack[MaxStack - 8] = _CX;
  stack[MaxStack - 7] = _BX;
  stack[MaxStack - 6] = _AX;
  stackSeg = FP_SEG(&stack[MaxStack - 14]);
  stackPtr = FP_OFF(&stack[MaxStack - 14]);
  status = NOT_ACTIVE;
  sleepCount = 0;
  currentPriority = priority = 0;
}

void Task::declare(void far *code, void far *taskEnd, char name, int deadline, int cycles) {
  // Declare a task with its execution time and deadline time

  stack[MaxStack - 5] = FP_OFF(code);
  stack[MaxStack - 4] = FP_SEG(code);
  stack[MaxStack - 3] = _FLAGS;
  stack[MaxStack - 2] = FP_OFF(taskEnd);
  stack[MaxStack - 1] = FP_SEG(taskEnd);

  // copy the stack to the stackCopy array
  for (int i = MaxStack - 1; i >= (MaxStack - 14); i--)
    stackCopy[i] = stack[i];

  stackSegCopy = stackSeg;
  stackPtrCopy = stackPtr;

  is_restore = false;

  this->name = name;
  this->cycles = cycles; // set the execution time
  this->deadline = deadline;
  this->originalDeadline = deadline;

  status = READY;
}

void Task::redeclare() {
  deadline = originalDeadline;
  cycles--;

  if (status == NOT_ACTIVE) {
    if (cycles > 0) {
        is_restore = true;
        status = READY;
    }
    else {
      status = NOT_ACTIVE;
    }
  }
  else {
    SMARTS.contextSwitchOff();
    cprintf("Time error %c \n", name);
    SMARTS.setError();
    SMARTS.contextSwitchOn();
  }
}

void Task::incrPriority() { --currentPriority; }

void Task::setOriginalPriority() { currentPriority = priority; }

void Task::sleepDecr() {
  // Decrements the sleep counter and update the task status accordingly

  if (status == SLEEP) {
    if (sleepCount > 0)
      --sleepCount;
    if (!sleepCount)
      status = READY;
  }
}
