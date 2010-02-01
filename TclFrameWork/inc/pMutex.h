/* The contents of this file are subject to the Mozilla Public License
 * Version 1.1 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
 * License for the specific language governing rights and limitations
 * under the License.
 * The Initial Developer of the Original Code is Alex Goldenstein.
 * All Rights Reserved.
 * Contributor(s): Alex Goldenstein.<goldale.de@googlemail.com>
 */
#ifndef P_MUTEX_H
#define P_MUTEX_H

#include <pthread.h>
#include <dprint.h>

const unsigned char _tmFlg_[] = {
  0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80
};

class CPMutex {
  // disable copy constructor.
  CPMutex(const CPMutex&);
  void operator = (const CPMutex&);
 protected:
  pthread_mutex_t Mut;
 public:
  CPMutex(const pthread_mutexattr_t* at=NULL) { pthread_mutex_init(&Mut, at); }
  virtual ~CPMutex() { pthread_mutex_destroy(&Mut); }
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

typedef unsigned int uInt;

class CRWLock {
  // disable copy constructor.
  CRWLock(const CRWLock&);
  void operator = (const CRWLock&);
 protected:
  pthread_rwlock_t rwl;
 public:
  CRWLock(const pthread_rwlockattr_t* attr=NULL) {
    (void)pthread_rwlock_init(&rwl, attr);
  }
  virtual ~CRWLock() {
    (void)pthread_rwlock_destroy(&rwl);
  }
  int rLock() { return pthread_rwlock_rdlock(&rwl); }
  int rLtry() { return pthread_rwlock_tryrdlock(&rwl); }
  int timedRLock(uInt tm) {
    struct timespec to;
    to.tv_sec  = tm / 100;
    //           1000000000
    to.tv_nsec = 10000000 * (tm % 100);
    return pthread_rwlock_timedrdlock(&rwl, &to);
  }
  int wLock() { return pthread_rwlock_wrlock(&rwl); }
  int wLtry() { return pthread_rwlock_trywrlock(&rwl); }
  int timedWLock(uInt tm) {
    struct timespec to;
    to.tv_sec  = tm / 100;
    //           1000000000
    to.tv_nsec = 10000000 * (tm % 100);
    return pthread_rwlock_timedwrlock(&rwl, &to);
  }
};

class CSglWLock {
 protected:
  CPMutex& mut;
 public:
  CSglWLock(CPMutex& m): mut(m) { mut.lock(); }
  ~CSglWLock() { mut.unlock(); }
};

class CSglRLock {
 protected:
  CPMutex& mut;
 public:
  CSglRLock(CPMutex& m): mut(m) { mut.lock(); }
  ~CSglRLock() { mut.unlock(); }
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
