#include "SCHALG25.H"
#include "SMARTS25.H"

int roundRobin() {
  // Round Robin short-term algorithm
  int count;
  int nextTask = (SMARTS.getCurrentTask() + 1) % SMARTS.getTotalTasks();
  for (count = 0;
       SMARTS.getStatus(nextTask) != READY && count < SMARTS.getTotalTasks();
       count++)
    nextTask = ++nextTask % SMARTS.getTotalTasks();
  if (count == SMARTS.getTotalTasks()) // no found READY task to run
    nextTask = SMARTS.getTotalTasks();
  return nextTask;
}

// EDF - Earliest Deadline First short-term algorithm
int edf() {
  int nextTask = -1; // no task to run
  int minDeadline = MAXINT;              // maximum deadline

  for (int i = 0; i < SMARTS.getTotalTasks(); i++) {
    if (SMARTS.getStatus(i) == READY && SMARTS.getCycles(i) > 0) {
      int temp = SMARTS.getDeadline(i);
      if (temp < minDeadline) {
        minDeadline = temp;
        nextTask = i;
      }
    }
  }

  return (nextTask >= 0) ? nextTask : SMARTS.getTotalTasks();
}

int rms() {
  int nextTask = -1;
	int min = 0xFFFF;

	for (int i = 0; i < SMARTS.getTotalTasks() - 1; i++) {
		if (SMARTS.getStatus(i) == READY && SMARTS.getCycles(i) > 0) {
			int taskPeriod = SMARTS.getOriginalDeadline(i);

			if (taskPeriod < min) {
				min = taskPeriod;
				nextTask = i;
			}
		}
	}

	if (nextTask < 0)	// no found READY task to run
		nextTask = SMARTS.getTotalTasks();	// call main next time.

	return nextTask;
}