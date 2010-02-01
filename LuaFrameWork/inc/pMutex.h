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
  // disable copy constructor.
  CSglLock(const CSglLock&);
  CSglLock operator = (const CSglLock&);

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
  CPCond(const pthread_mutexattr_t* at=NULL) : CPMutex(at)
  { pthread_cond_init(&Cond, NULL); }
  ~CPCond() { pthread_cond_destroy(&Cond); }
  int signal()    { return pthread_cond_signal   (&Cond); }
  int broadcast() { return pthread_cond_broadcast(&Cond); }
  int wait()      { return pthread_cond_wait(&Cond, &Mut); }
  int twait(unsigned long ms);
};

class CRwLock {
  // disable copy constructor.
  CRwLock(const CRwLock&);
  CRwLock &operator=( const CRwLock&);

 protected:
  pthread_rwlock_t rwlock;
 public:
  CRwLock() {
    pthread_rwlockattr_t attr;
    int retVal = pthread_rwlockattr_init( &attr );
    retVal = pthread_rwlockattr_setkind_np(&attr, PTHREAD_RWLOCK_PREFER_WRITER_NONRECURSIVE_NP);
    retVal = pthread_rwlock_init( &rwlock, &attr );
    retVal = pthread_rwlockattr_destroy( &attr );
  }
  ~CRwLock()     { pthread_rwlock_destroy( &rwlock ); }
  void lock()    { pthread_rwlock_rdlock ( &rwlock ); }
  void unlock()  { pthread_rwlock_unlock ( &rwlock ); }
  void wLock()   { pthread_rwlock_wrlock ( &rwlock ); }
  void wUnlock() { pthread_rwlock_unlock ( &rwlock ); }
};

class CSWLock {
  // disable copy constructor.
  CSWLock(const CSWLock&);
  CSWLock operator = (const CSWLock&);

 protected:
  CRwLock& rw;
  char flag;
 public:
 CSWLock(CRwLock& m, char x='\0') : rw(m), flag(x) { (!flag) ? rw.lock() : rw.wLock(); }
  ~CSWLock() { (!flag) ? rw.unlock() : rw.wUnlock(); }
};

#endif // P_MUTEX_H

// $Id: pMutex.h 299 2010-01-09 22:19:52Z asus $
