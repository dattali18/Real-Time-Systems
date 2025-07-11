#include "SMARTS25.H"

/// MARK: timer

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

/// MARK: Parallelism

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

  contextSched.declare(scheduler, userTaskEnd, 'S');
  // prepare the stack of the scheduler task

  for (int i = MaxStack - 1; i >= (MaxStack - 14); i--)
    schedCopy[i] = contextSched.stack[i];
}

int Parallelism::declareTask(void far *code, char name, int deadline,
           int cycles, int priority) {
  // Insert a new task entry in SMARTS context array [ ] with its execution time
  // and deadline time

  if (totalTasks < MaxTask - 1) {
    context[totalTasks++].declare(code, userTaskEnd, name, deadline, cycles, priority);
    ++activeTasks;
    return true;
  } else
    return false;
}

void Parallelism::updateDeadlines() {
  int i;
  for (i = 0; i < totalTasks - 1; ++i) {
    context[i].updateDeadline();
  }

  for (i = 0; i < totalTasks - 1; --i) {
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
    context[currentTask].contextSwitchOn();
    // contextSwitchFlag = true;
    callScheduler(); // return control to the scheduler
    return 1;
  }
  // contextSwitchFlag = true;
  context[currentTask].contextSwitchOn();
  return 0;
}

void Parallelism::contextSwitchOff() {
  // Disable context switch
  // contextSwitchFlag = false;
  context[currentTask].contextSwitchOff();
}

int Parallelism::getContextSwitch() { return context[currentTask].contextSwitchFlag; }

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

/// MARK: Task

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

void Task::declare(void far *code, void far *taskEnd, char name, int deadline, int cycles, int priority) {
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
  this->priority = priority;

  // contextSwitchFlag = 1;

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
    SMARTS.setError();
    SMARTS.callScheduler();
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

/// MARK: PriorityQueue

#include "MUTEX25.H"

#include "EXTERN25.H"

void PriorityQueue::push(int taskNum) {
	if(size < MaxTask) {
		queue[size] = taskNum;
		size++;
		sort();
		return;
	}
}

int PriorityQueue::pop() {
	return queue[--size];
}

void PriorityQueue::sort() {
	// insertion sort
	for (int i = 1; i < size; ++i) {
		int key = queue[i];
		int j = i - 1;

		while (j >= 0 && SMARTS.getCurrentPriority(queue[j]) > SMARTS.getCurrentPriority(key)) {
			queue[j + 1] = queue[j];
			--j;
		}

		queue[j + 1] = key;
	}
}

int PriorityQueue::search(int taskNum)
{
	for (int i = 0; i < size; i++)
	{
		if (queue[i] == taskNum)
		{
			return 1;
		}
	}
	return 0;
}

/// MARK: Mutex

Mutex::Mutex() {
    owner = -1;
    s = 1; // Semaphore value
    level = 0;
	waitingList = PriorityQueue();
	inheritanceFlag = 0; // Default to no priority inheritance
}

Mutex::Mutex(int inheritance) {
	owner = -1;
	s = 1; // Semaphore value
	level = 0;
	waitingList = PriorityQueue();
	inheritanceFlag = inheritance; // Set priority inheritance flag
}

void Mutex::acquire() {
	SMARTS.contextSwitchOff();

	int currentTask = SMARTS.getCurrentTask();
	if (s == 1 || owner == currentTask) // prevent deadlock
		s = 0;
	else {
		if (inheritanceFlag) {
			if (SMARTS.getCurrentPriority(owner) > SMARTS.getCurrentPriority(currentTask)) {
				SMARTS.setPriority(owner, SMARTS.getCurrentPriority(currentTask));
				printf("\ncurrent task %d inherit to owner %d priority %d\n", currentTask, owner, SMARTS.getCurrentPriority(currentTask));
				// fprintf(SMARTS.file, "currrent task %d inherite to owner %d priority %d \n", currentTask, owner, SMARTS.getCurrentPriority(currentTask));
			}
		}
		waitingList.push(currentTask);
		SMARTS.suspended();
	}

	owner = currentTask;
	level++;

	SMARTS.contextSwitchOn();
}

void Mutex::release() {
		SMARTS.contextSwitchOff();
		int currentTask = SMARTS.getCurrentTask();
		if (owner == currentTask) {
			if (--level)
				return;
	
			else {
				owner = -1;
				if (!waitingList.is_empty()) {
					int task = waitingList.pop();
					SMARTS.resume(task);
				}
				else
					s = 1;
			}
		}
		if (inheritanceFlag == 1) {
			SMARTS.resetPriority(currentTask);
			printf("restore owner %d priority to original priority %d\n", currentTask, SMARTS.getCurrentPriority(currentTask));
			// fprintf(SMARTS.file, "restore owner %d priority to original priority %d  \n", currentTask, SMARTS.getCurrentPriority(currentTask));
		}
	
		SMARTS.contextSwitchOn();
}