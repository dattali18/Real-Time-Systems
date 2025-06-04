#include "SMARTS25.H"

#define N 10
#define M 200000

#define useInheritance 1

Mutex ioMutex(0);
Mutex cenMutex(useInheritance);
Mutex amiMutex(useInheritance);

Event evCtoA, evCtoB, evFtoD, evFtoE;
// FILE* SMARTS.file = fopen("withoutP.txt", "w");  //a  == append, w == write
//FILE* SMARTS.file = fopen("withP.txt", "w");  //a  == append, w == write

void a() {
	int j;
	ioMutex.acquire();
	cout << "\n *************   A Start    *********************\n";
	// fprintf(SMARTS.file, "\n *************   A Start    *********************\n");
	ioMutex.release();

	for (j = 0; j < N; j++) {
		for (long i = 0; i < M; i++);
		ioMutex.acquire();
		cout << "A";
		// fprintf(SMARTS.file, "A");
		ioMutex.release();
	}

	ioMutex.acquire();
	cout << "\n *************   A wait event from C    *********************\n";
	// fprintf(SMARTS.file, "\n *************   A wait event from C    *********************\n");
	ioMutex.release();

	char w;
	evCtoA.wait(w);

	for (j = 0; j < N; j++) {
		for (long i = 0; i < M; i++);
		ioMutex.acquire();
		cout << "A";
		// fprintf(SMARTS.file, "A");
		ioMutex.release();
	}

	ioMutex.acquire();
	cout << "\n *************   A acquire   *********************\n";
	// fprintf(SMARTS.file, "\n *************   A acquire    *********************\n");
	ioMutex.release();

	cenMutex.acquire();

	for (j = 0; j < N; j++) {
		for (long i = 0; i < M; i++);
		ioMutex.acquire();
		cout << "A";
		// fprintf(SMARTS.file, "A");
		ioMutex.release();
	}

	ioMutex.acquire();
	cout << "\n *************   A release   *********************\n";
	// fprintf(SMARTS.file, "\n *************   A release    *********************\n");
	ioMutex.release();

	cenMutex.release();

	ioMutex.acquire();
	cout << "\n *************   A Finish   *********************\n";
	// fprintf(SMARTS.file, "\n *************   A Finish    *********************\n");
	ioMutex.release();
}

void b() {
	int j;
	ioMutex.acquire();
	cout << "\n *************   B Start    *********************\n";
	// fprintf(SMARTS.file, "\n *************   B Start    *********************\n");
	ioMutex.release();

	for (j = 0; j < N; j++) {
		for (long i = 0; i < M; i++);
		ioMutex.acquire();
		cout << "B";
		// fprintf(SMARTS.file, "B");
		ioMutex.release();
	}

	ioMutex.acquire();
	cout << "\n *************   B wait event from C    *********************\n";
	// fprintf(SMARTS.file, "\n *************   B wait event from C    *********************\n");
	ioMutex.release();

	char w;
	evCtoB.wait(w);

	for (j = 0; j < N; j++) {
		for (long i = 0; i < M; i++);
		ioMutex.acquire();
		cout << "B";
		// fprintf(SMARTS.file, "B");
		ioMutex.release();
	}

	ioMutex.acquire();
	cout << "\n *************   B Finish   *********************\n";
	// fprintf(SMARTS.file, "\n *************   B Finish    *********************\n");
	ioMutex.release();
}

void c() {
	int j;
	ioMutex.acquire();
	cout << "\n *************   C Start    *********************\n";
	// fprintf(SMARTS.file, "\n *************   C Start    *********************\n");
	ioMutex.release();

	for (j = 0; j < N; j++) {
		for (long i = 0; i < M; i++);
		ioMutex.acquire();
		cout << "C";
		// fprintf(SMARTS.file, "C");
		ioMutex.release();
	}

	ioMutex.acquire();
	cout << "\n *************   C acquire   *********************\n";
	// fprintf(SMARTS.file, "\n *************   C acquire    *********************\n");
	ioMutex.release();

	cenMutex.acquire();

	for (j = 0; j < N; j++) {
		for (long i = 0; i < M; i++);
		ioMutex.acquire();
		cout << "C";
		// fprintf(SMARTS.file, "C");
		ioMutex.release();
	}

	ioMutex.acquire();
	cout << "\n *************   C send event to A    *********************\n";
	// fprintf(SMARTS.file, "\n *************   C send event to A    *********************\n");
	ioMutex.release();

	evCtoA.send('A', NULL, 0);

	ioMutex.acquire();
	cout << "\n *************   C send event to B    *********************\n";
	// fprintf(SMARTS.file, "\n *************   C send event to B    *********************\n");
	ioMutex.release();

	evCtoB.send('B', NULL, 0);

	for (j = 0; j < N; j++) {
		for (long i = 0; i < M; i++);
		ioMutex.acquire();
		cout << "C";
		// fprintf(SMARTS.file, "C");
		ioMutex.release();
	}

	ioMutex.acquire();
	cout << "\n *************   C release   *********************\n";
	// fprintf(SMARTS.file, "\n *************   C release    *********************\n");
	ioMutex.release();

	cenMutex.release();

	for (j = 0; j < N; j++) {
		for (long i = 0; i < M; i++);
		ioMutex.acquire();
		cout << "C";
		// fprintf(SMARTS.file, "C");
		ioMutex.release();
	}

	ioMutex.acquire();
	cout << "\n *************   C Finish   *********************\n";
	// fprintf(SMARTS.file, "\n *************   C Finish    *********************\n");
	ioMutex.release();
}

void main() {
  clrscr();

  enum RUN { RR, EDF, RMS } run;

  run = RMS;

  switch (run) {
  case RR:
    cout << "Round Robin:\n";
    cout << "*************************************************\n";
    SMARTS.externalFunctions(timerInterruptHandler, scheduler, myTaskEnd, roundRobin);
    break;
  case EDF:
    cout << "EDF:\n";
    cout << "*************************************************\n";
    SMARTS.externalFunctions(timerInterruptHandler, scheduler, myTaskEnd, edf);
    break;
  case RMS:
    cout << "RMS:\n";
    cout << "*************************************************\n";
    SMARTS.externalFunctions(timerInterruptHandler, scheduler, myTaskEnd, rms);
  break;
  default:
    break;
  }

  int cycles = 2;

  SMARTS.declareTask(a, 'A', 400, cycles);
  SMARTS.declareTask(b, 'B', 401, cycles);
  SMARTS.declareTask(c, 'C', 402, cycles);

  SMARTS.runTheTasks();

  cout << "\n\nPress any key to exit...";
  char c;
  cin >> c;
}