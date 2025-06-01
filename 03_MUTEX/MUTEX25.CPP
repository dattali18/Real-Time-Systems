#include "MUTEX25.H"
#include "SMARTS25.H"

void PriorityQueue::enqueue(int value, int priority) {
	queue[size].value = value;
	queue[size].priority = priority;

	size++;
}

int PriorityQueue::peek() {
	int highestPriority = -999;
	int ind = -1;

	for (int i = 0; i < size; i++) {
		if (highestPriority < queue[i].priority) {
			highestPriority = queue[i].priority;
			ind = i;
		}
	}

	int ret = queue[ind].value;
	
	for (i = ind; i < size; i++) 
        queue[i] = queue[i + 1];

	size--;

	return ret;
}

Mutex::Mutex() {
    owner = -1;
    s = 1; // Semaphore value
    numberSuspended = 0;
    level = 0;
}



void Mutex::acquire()
{
	if (s == 1 || owner == SMARTS.getCurrentTask())
		s = 0;
	else {
		this->numberSuspended++;
		waitingList.enqueue(SMARTS.getCurrentTask(), SMARTS.getCurrentPriority());
		SMARTS.suspended();		// suspending the current task.
	}
	owner = SMARTS.getCurrentTask();
	level++;
}

void Mutex::release()
{
	if (owner == SMARTS.getCurrentTask())
		if (--level)
			return;
		else {
			owner = -1;
			if (this->numberSuspended > 0) {
				this->numberSuspended--;
				int task = waitingList.peek();
				SMARTS.resume(task);
			}
			else
				s = 1;
		}
}