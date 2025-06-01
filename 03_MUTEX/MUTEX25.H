#define MUTEX25_H
#ifndef MUTEX25_H

#include "CONST25.H"

struct QueueItem {
    int value;
    int priority;
};

class PriorityQueue {
    struct QueueItem queue[MaxTask];
    int size;

public:
    PriorityQueue() { size = 0;}

    void enqueue(int value, int priority);
    
    int peek();
};

class Mutex {
    int owner;
    int s;
    int numberSuspended;
    int level;
    PriorityQueue waitingList;

public:
    Mutex();

    void acquire();

    void release();
};

#endif // MUTEX25_H