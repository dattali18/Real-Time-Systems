#ifndef CONST25_H
#define CONST25_H

#include <conio.h>
#include <stdio.h>
#include <dos.h>
#include <values.h>
#include <iostream.h>

#define true 1
#define false 0
#define timerInt 0x08
#define userInt 0x60
#define MaxTask 15
#define MaxStack 512

enum taskStatus { READY, NOT_ACTIVE, SUSPENDED, SLEEP, UNDEFINED };

#endif // CONST25_H
