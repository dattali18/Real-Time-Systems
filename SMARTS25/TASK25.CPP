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

void Task::declare(void far *code, void far *taskEnd, char name) {
  // Declare a task with its execution time and deadline time

  stack[MaxStack - 5] = FP_OFF(code);
  stack[MaxStack - 4] = FP_SEG(code);
  stack[MaxStack - 3] = _FLAGS;
  stack[MaxStack - 2] = FP_OFF(taskEnd);
  stack[MaxStack - 1] = FP_SEG(taskEnd);

  this->name = name;

  status = READY;
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
