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
#include <errno.h>
#include <EvObj.h>
#include <Timer.h>
#include <Context.h>
#include <getTime.h>
#include <EvThread.h>
#include <dprint.h>

// Global timer queue
CTmQueue tmQueue;
// timer pool
static CMPool CTmObj_mem(sizeof(CTmObj));

CTmObj* CTmObj::newTm(uChar t,uInt i,uShort f) {
  return new (CTmObj_mem.pAlloc()) CTmObj(t,i,f);
}

void CTmObj::delTm(CTmObj* pt) {
  pt->~CTmObj();
  if(pt)
    CTmObj_mem.pFree(pt);
}

// ===================================

void CTmQueue::set(uLong t,uChar type,int id,uShort f) {
  uLong tx = tNow();
  t += tx;
  CTmObj* pt = CTmObj::newTm(type,id,f);
  {
    CSglLock sl(m);
    (void)insert(std::make_pair(t,pt));
  }
  DBG(">CTmQueue::set> Tm=%u-%u-Now(%u) Type=%u Id=%d %s\n",
      (uInt)t,(uInt)nt,(uInt)tx,type,id,(t < nt) ? "SIGN" : "--");
  if(t < nt) {
    nt = t;
    (void)wc.signal();
  }
}

// return 'x'  - new timeOut
//        '\0' - check pending tmOuts
char CTmQueue::wait() {
  char rr = '\0';
  do {
    if(wc.twait(nt) == ETIMEDOUT) {
      uLong tx = tNow();
      DBG(">CTmQueue::wait> Timer=%u Now=%u\n",(uInt)nt,(uInt)tx);
      if(tx >= nt) {
	rr = 'x';
      } else
	(void)pthread_yield();
    } else {
      DBG(">CTmQueue::wait> Sign to Add new tmOut Now=%u\n",(uInt)tNow());
      rr = 'z';
    }
  } while(!rr);
  return (rr == 'z') ? 'x' : '\0';
}

// ===================================

typedef CTmQueue::iterator IT;

void* CTmThrd::go() {
  uInt numTm = 0u;
  LOG(L_NOTICE,"\n***** CTmThrd::go%c *****\n",
      (GlAppsFlag & APPL_EXIT) ? '-' : '+');
  while(!(GlAppsFlag & APPL_EXIT)) {
    if(!tmQueue.wait()) {
      // To signal Fifo on scope exit
      CProcAllEv evProc;
      // TimeOut => check pending events
      uLong Tt = tNow() + 1;  // + 0.01 sec
      DBG("CTmThrd::go %s> check pending tmOut=%u-%u Sz=%u\n",
	  thName(),(uInt)Tt,(uInt)tmQueue.nt,(uInt)tmQueue.size());
      {
	CSglLock sl(tmQueue.m);
	CTmObj* pTm = NULL;
	IT i;
	IT lb = tmQueue.lower_bound(0);
	IT ub = tmQueue.upper_bound(Tt);
	tmQueue.nt = (*ub).first;
	for(i=lb; i!=ub; ++i) {
	  pTm = (*i).second;
	  // Check for TOut_NoTmOut - to exit Timer
	  if(pTm->type == TOut_NoTmOut)
	    return NULL;
	  // put event in Fifo
	  DBG("CTmThrd::go %s> putEv Tm=%u id=%u Type=%u Inf=%u\n",
	    thName(),(uInt)((*i).first),pTm->id,pTm->type,pTm->inf);
	  (void)GlEvFifo.addEl(CEvent::newEv(pTm->id,TOut_Shift+(pTm->type),0u,(uChar)(pTm->inf)));
	  // Del timerObj
	  CTmObj::delTm(pTm);
	  ++numTm;
	}
	// Erase timeOut's from map
	tmQueue.erase(lb,ub);
	lb = tmQueue.begin();
	Tt = (*lb).first;
	// If no pending tmOut's
	tmQueue.nt = Tt ? Tt : 0xFFFFFFFF;
      }
      if(!numTm)
	continue;
      LOG(L_WARN,"CTmThrd::go Queue=%u \tnTm=%u    \tnxtTmOut- %u / %u\n",
	  GlEvFifo.qSize(),numTm,(uInt)tmQueue.nt,(uInt)Tt);
      numTm = 0u;
    } else {
      // Add new TimeOut cond. Signaled
      DBG("CTmThrd::go TimeOut added To=%u Sz=%u\n",(uInt)tmQueue.nt,(uInt)tmQueue.size());
    }
  }
  LOG(L_WARN,"CTmThrd::go TERMINATED Now=%u\n",(uInt)tNow());
  return NULL;
}

// $Log$
