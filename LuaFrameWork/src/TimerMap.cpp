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
#include <dprint.h>

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

void CTmQueue::set(uLong t,uInt id,uShort type,uShort f) {
  uLong tx = tNow();  t += tx;
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
  int ww = 0;
  char rr = '\0';
  while( ! rr ) {
    {
      CSglLock sl(wc);
      ww = wc.twait(nt);
    }
    if(ETIMEDOUT == ww) {
      uLong tx = tNow();
      DBG(">CTmQueue::wait> Timer=%lu Now=%lu\n",nt,tx);
      if(tx > (nt-1)) rr = 'x';
    } else {
      DBG(">CTmQueue::wait> Sign to Add new tmOut Now=%lu\n",tNow());
      rr = 'z';
    }
  }
  return (rr == 'z') ? 'x' : '\0';
}

// ===================================

CTmThrd* pTimeThrd = NULL;
typedef CTmQueue::iterator IT;

CTmThrd::CTmThrd() {
  LOG(L_NOTICE,"CTmThrd::CTmThrd - AppsTimer\n");
}

CTmThrd::~CTmThrd() {
  LOG(L_NOTICE,"CTmThrd::~CTmThrd - AppsTimer\n");
  // delete all tmOut from tQueue
  for(IT i=tQueue.begin(); i!=tQueue.end(); ++i)
    CTmObj::delTm((CTmObj*)((*i).second));
}

void* CTmThrd::go() {
  uInt numTm = 0u;
  pTimeThrd = this;

  LOG(L_NOTICE,"\n*** CTimerThrd::go ***\n\n");

  while(!(GlAppsFlag & APPL_EXIT)) {
    if(!tQueue.wait()) {
      // To signal Fifo on scope exit
      CProcAllEv evProc;
      // TimeOut => check pending events
      uLong Tt = tNow();
      DBG("CTmThrd::go> check pending tmOut=%lu-%lu Sz=%lu\n",
	  Tt,tQueue.nt,(uLong)tQueue.size());
      {
	CSglLock sl(tQueue.m);
	CTmObj* pTm = NULL;
	IT i;
	IT lb = tQueue.lower_bound(0);
	IT ub = tQueue.upper_bound(Tt);
	tQueue.nt = (*ub).first;
	for(i=lb; i!=ub; ++i) {
	  pTm = (*i).second;
	  // Check for TOut_NoTmOut - to exit Timer
	  if(pTm->type == TOut_NoTmOut)
	    return NULL;
	  // put event in Fifo
	  DBG("CTmThrd::go> putEv Tm=%lu id=%u Type=%u Inf=%u\n",
	      ((*i).first),pTm->id,pTm->type,pTm->inf);
	  (void)GlEvFifo.addEl(CEvent::newEv(pTm->id,TOut_Shift+(pTm->type),0u,(uChar)(pTm->inf)));
	  // Del timerObj
	  CTmObj::delTm(pTm);
	  ++numTm;
	}
	// Erase timeOut's from map
	tQueue.erase(lb,ub);
	lb = tQueue.begin();
	Tt = (*lb).first;
	// If no pending tmOut's
	tQueue.nt = Tt ? Tt : 0xFFFFFFFF;
      }
      if(!numTm)
	continue;
      LOG(L_WARN,"CTmThrd::go Queue=%u \tnTm=%u    \tnxtTmOut- %u / %u\n",
	  GlEvFifo.qSize(),numTm,(uInt)tQueue.nt,(uInt)Tt);
      numTm = 0u;
    } else {
      // Add new TimeOut cond. Signaled
      DBG("CTmThrd::go TimeOut added To=%u Sz=%u\n",(uInt)tQueue.nt,(uInt)tQueue.size());
    }
  }
  LOG(L_WARN,"CTmThrd::go> TERMINATED Now=%u\n",(uInt)tNow());
  return NULL;
}

// $Id: TimerMap.cpp 319 2010-01-15 20:43:05Z asus $
