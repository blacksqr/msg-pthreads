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
#ifndef ITS_PTHREADOBJ_H
#define ITS_PTHREADOBJ_H

#include <Global.h>
#include <pMutex.h>
#include <sched.h>

class CAutoDel;

//static function
#define yield() sched_yield()

// base class for all thread
class CThreadObj {
  friend class CAutoDel;
  // disable copy constructor.
  CThreadObj(const CThreadObj&);
  void operator = (const CThreadObj&);
 protected:
  //static pthread_key_t thrdKey;
  //static pthread_once_t thrdKey_once;
  //static void thrdKey_init();
  pthread_t tid;
  pthread_attr_t attr;
  virtual ~CThreadObj() { pthread_attr_destroy(&attr); }
 public:
  CThreadObj(char flg=0x0,uInt st_size=0u,int prio=0);
  pthread_t getTId() const { return tid; }
  virtual void* go() { return NULL; }
  static CThreadObj* getSelf();
  void start();
};

#endif // ITS_PTHREADOBJ_H

// $Id: Thread.h 301 2010-01-11 12:08:00Z asus $
