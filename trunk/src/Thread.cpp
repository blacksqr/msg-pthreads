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
#include <Thread.h>

pthread_key_t CThreadObj::thrdKey;
pthread_once_t CThreadObj::thrdKey_once = PTHREAD_ONCE_INIT;

extern "C" void* threadGo(void* arg) {
  (void)pthread_once(&CThreadObj::thrdKey_once,CThreadObj::thrdKey_init);
  (void)pthread_setspecific(CThreadObj::thrdKey,(static_cast<CThreadObj*>(arg)));
  // yield controll to calling thread
  (void)pthread_yield();
  // start thread in derived class
  void* pr = (static_cast<CThreadObj*>(arg))->go();
  (void)(static_cast<CThreadObj*>(arg))->go();
  delete (static_cast<CThreadObj*>(arg)); // run virtual destructor
  return pr;
}

void CThreadObj::thrdKey_init() { (void)pthread_key_create(&thrdKey,NULL); }
CThreadObj* CThreadObj::getSelf() { return (CThreadObj*)pthread_getspecific(thrdKey); }

CThreadObj::CThreadObj(char flg) {
  pthread_attr_t attr;
  (void)pthread_attr_init(&attr);
  // SCHED_FIFO, SCHED_RR, SCHED_SPORADIC
  (void)pthread_attr_setschedpolicy(&attr,flg ? SCHED_RR : SCHED_FIFO);
  (void)pthread_create(&tid,&attr,threadGo,this);
  int rr = pthread_detach(tid);
  if(rr)
    LOG(L_ERR,"<CThreadObj::CThreadObj Error pthread_detach %d\n",rr);
}

// $Log$
