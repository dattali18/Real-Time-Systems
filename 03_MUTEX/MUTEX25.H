#ifndef MUTEX25_H
#define MUTEX25_H

#include "CONST25.H"


class PriorityQueue {
    int queue[MaxTask];
    int size;

    void sort();

public:
    PriorityQueue() { size = 0; }

    void push(int taskNum);
    
    int pop();

    int is_empty() { return size == 0; }

    int search(int taskNum);
};

class Mutex {
    int owner;
    int s;
    int level;
    PriorityQueue waitingList;
    int inheritanceFlag;

public:
    Mutex();

    Mutex(int inheritance);

    void acquire();

    void release();
};

#endif // MUTEX25_H