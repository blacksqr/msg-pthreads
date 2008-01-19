#include <time.h>
#include <pMutex.h>
#include <getTime.h>
#include <dprint.h>

typedef unsigned int  uInt;
typedef unsigned long uLong;

int CPCond::twait(uLong ms) {
  struct timeval x;
  struct timespec to;
  tOutAbs(ms,&x);
  to.tv_nsec = 1000 * x.tv_usec; // nano-sec
  to.tv_sec = x.tv_sec;
  return pthread_cond_timedwait(&Cond, &Mut, &to);
}
