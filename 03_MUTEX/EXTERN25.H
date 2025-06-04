#ifndef EXTERN25_H
#define EXTERN25_H

#include "MUTEX25.H"
#include "PARAL25.H"

// reads the number of the remaining clocks from the timer register
extern unsigned getTimerClocks();

// external functions for 'externalFunctions' function
void far interrupt timerInterruptHandler(...);

void scheduler();

void myTaskEnd();

extern Parallelism SMARTS;

#endif // EXTERN25_H