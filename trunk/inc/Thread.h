// $Id$
#ifndef ITS_MUTEX_H
#define ITS_MUTEX_H
#include <Global.h>
#include <pMutex.h>

// common thread function
extern "C" void* threadGo(void* arg);

//static function
#define yield() sched_yield()

// base class for all thread
class CThreadObj {
  friend void* threadGo(void* arg);
  // disable copy constructor.
  CThreadObj(const CThreadObj&);
  void operator = (const CThreadObj&);
 protected:
  static pthread_key_t thrdKey;
  static pthread_once_t thrdKey_once;
  static void thrdKey_init();
  pthread_t tid;
  virtual ~CThreadObj() {}
 public:
  CThreadObj(char flg=0x0);
  pthread_t getTId() const { return tid; }
  virtual void* go() { return NULL; }
  static CThreadObj* getSelf();
};

#endif // ITS_MUTEX_H
// $Log$
