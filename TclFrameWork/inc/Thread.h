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
