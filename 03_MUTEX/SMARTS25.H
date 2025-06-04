#ifndef SMARTS25_H
#define SMARTS25_H

#include "CONST25.H"
#include "PARAL25.H"
#include "TASK25.H"
#include "EVENT25.H"
#include "SCHALG25.H"
#include "MUTEX25.H"

// reads the number of the remaining clocks from the timer register
extern unsigned getTimerClocks();
// external functions for 'externalFunctions' function
void far interrupt timerInterruptHandler(...);

void scheduler();
void myTaskEnd();

// main scheduler object
extern Parallelism SMARTS;

#endif // SMARTS25_H