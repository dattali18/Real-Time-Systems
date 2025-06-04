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