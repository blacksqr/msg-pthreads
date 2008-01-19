// $Id$
#ifndef ITSTIME_H
#define ITSTIME_H
#include <sys/time.h>

class CAppTimer {
 private:
  struct timeval T;
 public:
  CAppTimer();
  ~CAppTimer() {}
  unsigned long now(int s=0, char ms='\0');
  void tmOut(unsigned long ms, struct timeval* pT, char flag='\0');
};

extern CAppTimer appTimer;
#define tNow() appTimer.now()
// with delta timeout
#define tOut(ms,pT) appTimer.tmOut(ms,pT)
// Absolut timeout
#define tOutAbs(ms,pT) appTimer.tmOut(ms,pT,'x')

#endif // ITSTIME_H
// $Log$
