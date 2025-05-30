#ifndef EVENT25_H
#define EVENT25_H

class Event {
private:
  int flag;         // arrived flag
  void *data;       // the parameter
  char source;      // source task name
  char dest;        // destination task name
  int senderWait;   // tells if the sender has to wait till the reciver receives
  int testAndSet(); // test and set 'flag' in an 'atomic transaction'
public:
  Event();
  // sends an event to 'dest', if 'synch' is true waits until the receiver gets
  // it.
  void send(char dest, void *param, int synch);
  // waits for the event, and returns the parameter.
  void *wait(char &sourceP); //'sourceP' will contain the task sender ID.
  // check for for an arrived event. 'sourceP' will contain the task sender ID.
  int arrived(char &sourceP);
  // resets the event
  void reset();
};

#endif // EVENT25_H