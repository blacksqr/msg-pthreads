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
#include <Timer.h>
#include <WThread.h>
#include <tstSigThrd.h>
#include "tAdminLua.h"

void ansleep(int timeOut) { nsleep(timeOut); }

//====================================
// Start signaling threads

void tstSgThX(int m1, int m2) {
  DBG("tstSgThX> Param %d %d \n",m1,m2);
  CEvThrd* p = new CTstSgThX(m1,m2);
  p->startEvTh();
}

void tstSgTh(int m1, int m2, int t1, int t2) {
  DBG("tstSgTh> Param %d %d %d %d\n",m1,m2,t1,t2);
  CEvThrd* p = new CTstSgTh(m1,m2,t1,t2);
  p->startEvTh();
}

void tstFsmTh(int m) {
  DBG("tstFsmTh> Param %d\n",m);
  CEvThrd* p = new CTstFsmTh(m);
  p->startEvTh();
}

//====================================
// Manage work threads

int startWrkTh() {
  CWThread* p = new CWThread(0x0);
  p->startWTh();
  int nr = (int)CWThread::nRThrd();
  DBG("startWrkTh> WrkThread %d started\n",nr);
  return nr;
}

int stopWrkTh() {
  CWThread::wStop();
  DBG("stopWrkTh> Stop WrkThread\n");
  CEvent* ev = CEvent::newEv(0u,Evnt_wThrdEnd);
  ev->put('x');
  ev->sign();
  yield();
  CWThread::wStart();
  int rr = (int)CWThread::nRThrd();
  DBG("stopWrkTh> WrkThread running %d\n",rr);
  return rr;
}

int rLoadScrpt() {
  int k = 0;
  CWThread::rloadScript();
  DBG("rLoadScrpt> Reload Wrk-Script\n");
  for(k=0; k<CWThread::nRThrd(); ++k) {
    CEvent* ev = CEvent::newEv(0u,Evnt_wThrdEnd);
    ev->put('x');
    ev->sign();
    yield();
  }
  return k;
}

int numRunWTh() {
  return (int)CWThread::nRThrd();
}

// Common application tasks
//====================================
extern char GlobEvThreadStoped;

// Unlock / Lock event queue
void aStart() {
  (void)GlEvFifo.getCond().unlock();
}
int aStop() {
  return (GlEvFifo.getCond()).lock();
}

// Set new HauseKeep timeout
void checkTOut(int tm, short tm_type, short sIi) {
  pTimeThrd->set(tm,HkCId,tm_type,sIi);
}

void stopAllSgTh() {
  GlAppsFlag |= STOP_EV_THRD;
  for(uChar k=0x0; k<MAX_N_STHREAD; ++k) {
    if(g_evThrdArr[k]) {
      DBG("stopAllSgTh> Stop sign.thread %s\n",g_evThrdArr[k]->thName());
      g_evThrdArr[k]->stop();
    }
  }
  GlobEvThreadStoped = 'x';
}

// Stop new context factory
const char* stopCFactory(int n) {
  stopNewCtxt = n ? 'x' : '\0';
  DBG("stopCFactory> Stop context factory %d\n",n);
  return n ? "Off" : "On";
}

// Delete all context from hashCntxt
void ctxtHashClean() {
  CCtxtMap::iterator I = hashCntxt.begin();
  while(I != hashCntxt.end()) {
    uInt cid     = (*I).first;
    CContext* pc = (*I).second;
    if(pc) {
      DBG("ctxtHashClean> Halt context[%u]\n",pc->getId());
      pc->send(Evnt_PreDelCtxt,cid);
    }
    ++I;
  }
}

// Exit all work threads & timer
void preExitApps() {
  GlAppsFlag |= APPL_EXIT;  // Stop timer
  CWThread::wStop();
  CWThread::rloadScript();
  for(uChar k=0u; k<MAX_N_WTHREAD; ++k) {
    DBG("preExitApps> woff %d\n",CWThread::nRThrd());
    CEvent* ev = CEvent::newEv(0u,Evnt_wThrdEnd);
    ev->put();
    ev->sign();
  }
}

#if 0
#include <udpSock.h>

#define SOCK_BIND   0x0001
#define RECV_SEND   0x0002
#define RECV_END    0x0004

class CLuaSock: public CUdpCSock {
protected:
  const uShort port;
  uShort state;
  const char* func;
public:
  CLuaSock(const char* host,uShort p, const char* f): port(p),state(0u),func(f) {}
  ~CLuaSock() {}
  int getMsg(char* buf, int l) { return crecv(buf, l); }
  int sndMsg(char* msg) { return csend(msg, strlen(msg)); }
};
#endif // 0

// $Id: tAdminLua.cpp 372 2010-05-08 15:29:49Z asus $
