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
  int min = MAXINT; // maximum deadline

  for (int i = 0; i < SMARTS.getTotalTasks(); i++) {
		if (SMARTS.getStatus(i) != READY) 
      continue; // skip tasks that are not READY

    if(SMARTS.getCycles(i) <= 0)
      continue; // skip tasks with no cycles left

    int temp = SMARTS.getDeadline(i);
    if (temp < min) {
      min = temp;
      nextTask = i;
    }
  }

  return (nextTask >= 0) ? nextTask : SMARTS.getTotalTasks();
}

/// RMS: Rate Monotonic Scheduling short-term algorithm
int rms() {
  int nextTask = -1;
	int min = MAXINT;

	for (int i = 0; i < SMARTS.getTotalTasks(); i++) {
		if (SMARTS.getStatus(i) != READY) 
      continue; // skip tasks that are not READY

    if(SMARTS.getCycles(i) <= 0)
      continue; // skip tasks with no cycles left

    int temp = SMARTS.getOriginalDeadline(i);

    if (temp < min) {
      min = temp;
      nextTask = i;
    }
	}

	return (nextTask >= 0) ? nextTask : SMARTS.getTotalTasks();
}