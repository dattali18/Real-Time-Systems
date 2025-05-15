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