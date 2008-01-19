// $Id$
#ifndef P_MUTEX_H
#define P_MUTEX_H

#include <pthread.h>
#include <dprint.h>

class CPMutex {
 protected:
  pthread_mutex_t Mut;
 public:
  CPMutex(const pthread_mutexattr_t* at=NULL) { pthread_mutex_init(&Mut, at); }
  ~CPMutex() { pthread_mutex_destroy(&Mut); }
  int lock() {
    //DBG(">CPMutex::lock\n");
    return pthread_mutex_lock(&Mut);
  }
  int ltry() {
    //DBG(">CPMutex::ltry\n");
    return pthread_mutex_trylock(&Mut);
  }
  int unlock() {
    //DBG(">CPMutex::unlock\n");
    return pthread_mutex_unlock(&Mut);
  }
  pthread_mutex_t& getMutex() { return Mut; }
};

class CSglLock {
 protected:
  CPMutex& mut;
 public:
  CSglLock(CPMutex& m): mut(m) { mut.lock(); }
  ~CSglLock() { mut.unlock(); }
};

class CPCond: public CPMutex {
 protected:
  pthread_cond_t Cond;
 public:
  CPCond(const pthread_mutexattr_t* at=NULL)
    : CPMutex(at)
  { pthread_cond_init(&Cond, NULL); }
  ~CPCond() { pthread_cond_destroy(&Cond); }
  int signal()    { return pthread_cond_signal(&Cond); }
  int broadcast() { return pthread_cond_broadcast(&Cond); }
  int wait()      { return pthread_cond_wait(&Cond, &Mut); }
  int twait(unsigned long ms);
};

#endif // P_MUTEX_H
