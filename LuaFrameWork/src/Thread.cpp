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

#include <string.h>
#include <Thread.h>

__thread CThreadObj* _pSelf = NULL;

//pthread_key_t CThreadObj::thrdKey;
//pthread_once_t CThreadObj::thrdKey_once = PTHREAD_ONCE_INIT;

// Wrapper class to destroy thread obj.
class CAutoDel {
 protected:
  CThreadObj* pt;
 public:
  CAutoDel(CThreadObj* p) : pt(p) { _pSelf = p; }
  ~CAutoDel() {
    LOG(L_NOTICE,"threadGo exit 0x%X, destroy thread obj.\n", (int)pt);
    delete pt;
  }
  void* go() { return pt->go(); }
};

extern "C" void* threadGo(void* arg) {
  //(void)pthread_once(&CThreadObj::thrdKey_once,CThreadObj::thrdKey_init);
  //(void)pthread_setspecific(CThreadObj::thrdKey,(static_cast<CThreadObj*>(arg)));
  CAutoDel ad(static_cast<CThreadObj*>(arg));
  (void)sched_yield();  // yield controll to calling thread
  //LOG(L_NOTICE,"threadGo run\n");
  // start thread function from derived class
  return ad.go();
}

//void CThreadObj::thrdKey_init() { (void)pthread_key_create(&thrdKey,NULL); }

//CThreadObj* CThreadObj::getSelf() { return (CThreadObj*)pthread_getspecific(thrdKey); }
CThreadObj* CThreadObj::getSelf() { return _pSelf; }

CThreadObj::CThreadObj(char flg,uInt st_size,int prio) {
  (void)pthread_attr_init(&attr);
  if(st_size)
    (void)pthread_attr_setstacksize(&attr,st_size);
  {
    // SCHED_OTHER, SCHED_BATCH, SCHED_IDLE - default
    // SCHED_FIFO, SCHED_RR - realtime

    int policy = !flg ? SCHED_OTHER :
      ((flg == 'f') ? SCHED_FIFO :
       ((flg == 'r') ? SCHED_RR :
	((flg == 'b') ?  SCHED_BATCH : /*SCHED_IDLE*/5)));
    (void)pthread_attr_setschedpolicy(&attr,policy);
  }
  if(strchr("fr", flg) && prio) {
    // Set PRIO for SCHED_SPORADIC
    sched_param sParam;
    sParam.sched_priority = prio;
    (void)pthread_attr_setschedparam(&attr, &sParam);
  }
}
void CThreadObj::start() {
  (void)pthread_create(&tid,&attr,threadGo,this);
  int rr = pthread_detach(tid);
  if(rr)
    LOG(L_ERR,"<CThreadObj::CThreadObj Error pthread_detach %d\n",rr);
}

// $Id: Thread.cpp 301 2010-01-11 12:08:00Z asus $
