#include "SMARTS25.H"

#define N 20
#define M 100000

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

  enum RUN { RR, EDF } run;

  run = RR;

  switch (run) {
  case RR:
    SMARTS.externalFunctions(timerInterruptHandler, scheduler, myTaskEnd,
                             roundRobin);
    SMARTS.declareTask(a, 'A');
    SMARTS.declareTask(b, 'B');
    SMARTS.declareTask(c, 'C');
    SMARTS.runTheTasks();
    break;
  case EDF:
    /// after adding edf
    break;
  default:
    break;
  }

  cout << "\n\nPress any key to exit...";
  char c;
  cin >> c;
}