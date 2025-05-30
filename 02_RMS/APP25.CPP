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

void main() {
  clrscr();

  enum RUN { RR, EDF, RMS } run;

  run = RMS;

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
    SMARTS.declareTask(b, 'B', 50, 2);
    SMARTS.declareTask(c, 'C', 30, 1);
    break;
  case RMS:
    SMARTS.externalFunctions(timerInterruptHandler, scheduler, myTaskEnd, rms);
    SMARTS.declareTask(a, 'A', 10, 1);
    SMARTS.declareTask(b, 'B', 50, 1);
    SMARTS.declareTask(c, 'C', 30, 1);
    break;
  default:
    break;
  }

  SMARTS.runTheTasks();

  cout << "\n\nPress any key to exit...";
  char c;
  cin >> c;
}