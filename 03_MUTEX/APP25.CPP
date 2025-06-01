#include "SMARTS25.H"

#define N 10
#define M 200000

/// MARK: macros

void a() {
  SMARTS.contextSwitchOff();
  cout << "\n *************   A Start    *********************\n";
  SMARTS.contextSwitchOn();
  for (int j = 0; j < N; j++) {
    for (int long i = 0; i < M; ++i)
      ;

    SMARTS.contextSwitchOff();
    cout << "A";
    SMARTS.contextSwitchOn();
  }
  SMARTS.contextSwitchOff();
  cout << "\n *************   A Finish   *********************\n";
  SMARTS.contextSwitchOn();
}

void b() {
  SMARTS.contextSwitchOff();
  cout << "\n *************   B Start    *********************\n";
  SMARTS.contextSwitchOn();
  for (int j = 0; j < N; j++) {
    for (int long i = 0; i < M; ++i)
      ;

    SMARTS.contextSwitchOff();
    cout << "B";
    SMARTS.contextSwitchOn();
  }
  SMARTS.contextSwitchOff();
  cout << "\n *************   B Finish   *********************\n";
  SMARTS.contextSwitchOn();
}

void c() {
  SMARTS.contextSwitchOff();
  cout << "\n *************   C Start    *********************\n";
  SMARTS.contextSwitchOn();
  for (int j = 0; j < N; j++) {
    for (int long i = 0; i < M; ++i)
      ;

    SMARTS.contextSwitchOff();
    cout << "C";
    SMARTS.contextSwitchOn();
  }
  SMARTS.contextSwitchOff();
  cout << "\n *************   C Finish   *********************\n";
  SMARTS.contextSwitchOn();
}

void a1() {
  MUTEX.acquire();
  cout << "\n *************   A Start    *********************\n";
  MUTEX.release();
  for (int j = 0; j < N; j++) {
    for (int long i = 0; i < M; ++i)
      ;

    MUTEX.acquire();
    cout << "A";
    MUTEX.release();
  }
  MUTEX.acquire();
  cout << "\n *************   A Finish   *********************\n";
  MUTEX.release();
}

void b1() {
  MUTEX.acquire();
  cout << "\n *************   B Start    *********************\n";
  MUTEX.release();
  for (int j = 0; j < N; j++) {
    for (int long i = 0; i < M; ++i)
      ;

    MUTEX.acquire();
    cout << "B";
    MUTEX.release();
  }
  MUTEX.acquire();
  cout << "\n *************   B Finish   *********************\n";
  MUTEX.release();
}

void c1() {
  MUTEX.acquire();
  cout << "\n *************   C Start    *********************\n";
  MUTEX.release();
  for (int j = 0; j < N; j++) {
    for (int long i = 0; i < M; ++i)
      ;

    MUTEX.acquire();
    cout << "C";
    MUTEX.release();
  }
  MUTEX.acquire();
  cout << "\n *************   C Finish   *********************\n";
  MUTEX.release();
}

void main() {
  clrscr();

  enum SCHED { MUTEX, CONTEXT_SWITCH } sched;

  enum RUN { RR, EDF, RMS } run;

  run = EDF;

  sched = CONTEXT_SWITCH;

  switch (sched) {
    case CONTEXT_SWITCH:
     switch (run) {
      case RR:
        SMARTS.externalFunctions(timerInterruptHandler, scheduler, myTaskEnd,
                                 roundRobin);
        SMARTS.declareTask(a, 'A');
        SMARTS.declareTask(b, 'B');
        SMARTS.declareTask(c, 'C');
        break;
      case EDF:
        SMARTS.externalFunctions(timerInterruptHandler, scheduler, myTaskEnd, edf);
        SMARTS.declareTask(a, 'A', 10, 3);
        SMARTS.declareTask(b, 'B', 60, 2);
        SMARTS.declareTask(c, 'C', 30, 1);
        break;
      case RMS:
        SMARTS.externalFunctions(timerInterruptHandler, scheduler, myTaskEnd, rms);
        SMARTS.declareTask(a, 'A', 10, 3);
        SMARTS.declareTask(b, 'B', 60, 2);
        SMARTS.declareTask(c, 'C', 30, 1);
        break;
      default:
        break;
      }
      break;
    case MUTEX:
    switch (run) {
      case RR:
        SMARTS.externalFunctions(timerInterruptHandler, scheduler, myTaskEnd,
                                 roundRobin);
        SMARTS.declareTask(a1, 'A');
        SMARTS.declareTask(b1, 'B');
        SMARTS.declareTask(c1, 'C');
        break;
      case EDF:
        SMARTS.externalFunctions(timerInterruptHandler, scheduler, myTaskEnd, edf);
        SMARTS.declareTask(a1, 'A', 10, 3);
        SMARTS.declareTask(b1, 'B', 60, 2);
        SMARTS.declareTask(c1, 'C', 30, 1);
        break;
      case RMS:
        SMARTS.externalFunctions(timerInterruptHandler, scheduler, myTaskEnd, rms);
        SMARTS.declareTask(a1, 'A', 10, 3);
        SMARTS.declareTask(b1, 'B', 60, 2);
        SMARTS.declareTask(c1, 'C', 30, 1);
        break;
      default:
        break;
      }
      break;
  }

  SMARTS.runTheTasks();

  cout << "\n\nPress any key to exit...";
  char c;
  cin >> c;
}