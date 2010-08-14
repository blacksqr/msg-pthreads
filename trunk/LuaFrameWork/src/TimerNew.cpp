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
#include <dprint.h>

#include <Timer.h>

CTmQueue* tmQueue = NULL;

// ===================================

// timer pool
static CMPool CTmObj_mem(sizeof(CTmObj));

CTmObj* CTmObj::newTm(uInt tm,uChar t,uInt i,CTmObj* pn,uShort f) {
  return new (CTmObj_mem.pAlloc()) CTmObj(tm,t,i,pn,f);
}

void CTmObj::delTm(CTmObj* pt) {
  // Clean full list
  CTmObj* p = pt;
  while(p) {
    pt = p->nxt;

    p->~CTmObj();
    CTmObj_mem.pFree(p);
    p = pt;
  }
}

// ===================================

CTmQueue::CTmQueue(uLong d, uLong t, uChar l):
  delta(d), level(l)
{
  for(int k=0; k<TMQSize; ++k) {
    pTmPool[k] = 0u;  nTimer[k]  = '\0';
  }
  beg = 0x0;
  nt  = 0xEFFFFFFF;
  tmBeg = level ? (t+delta) : tNow();
  nxtLevel = (l < nCTMQLevel) ? new CTmQueue(TMQSize*d, tmBeg, l+1) : NULL;
}

CTmQueue::~CTmQueue() {
  DBG("CTimer::~CTmQueue> Delete all pending timeouts\n");
  // Delete next level
  delete nxtLevel;
  // Delete all pending timeouts
  for(Q_Ind k=0x0; k<TMQSize; ++k) {
    if(pTmPool[k]) {
      CTmObj* p = pTmPool[k];
      CTmObj* pp = p;
      while(p) {
	p = p->nxt;
	CTmObj::delTm(pp);
	pp = p;
      }
    }
  }
}

// ===================================

// Set relative timeout - tm
void CTmQueue::set(uLong tx,uInt id,uShort type,uShort f) {
  tx += tNow();
  DBG("CTimer::set> Tm=%lu\n",tx);
  absSet(tx,id,type,f);
}

// Set absolute timeout, tm - seconds since the Epoch
void CTmQueue::absSet(uLong tx,uInt id,uShort type,uShort f) {
  int x;
  Q_Ind w;
  nt = (tx < nt) ? tx : nt;
  {
    CSglLock sl(m);
    x = (tx - tmBeg) / delta;
    w = beg + x;
  }
  //DBG("CTimer::absSet> Tm=%lu Type=%u Id=%d NxtTo=%lu Ind=<%u %u>\n",tx,type,id,nt,w,nTimer[w]);
  if(x < TMQSize) {
    CSglLock sl(lm[w & MutArrSz]);
    nTimer[w] += 1;
    if(!pTmPool[w]) {
      pTmPool[w] = CTmObj::newTm(tx,type,id,NULL,f);
    } else if(pTmPool[w]->tmo > tx) {
      pTmPool[w] = CTmObj::newTm(tx,type,id,pTmPool[w],f);
    } else {
      CTmObj* px = pTmPool[w];
      CTmObj* pz = pTmPool[w]->nxt;
      while(pz) {
	if(pz->tmo > tx) {
	  px->nxt = CTmObj::newTm(tx,type,id,pz,f);
	  //DBG("CTimer::absSet> INS %lu-%lu-%lu\n",px->tmo,tx,pz->tmo)
	  return;
	}
	px = pz;
	pz = pz->nxt;
      }
      px->nxt = CTmObj::newTm(tx,type,id,NULL,f);
    }
    return;
  }
  if(nxtLevel)
    return nxtLevel->set(tx, type, id, f);
  LOG(L_ERR,"ERROR: timeout not set %lu\n",tx);
}

// ===================================

CTmObj* CTmQueue::get0(CTmObj*& pw,CTmObj* pe,uLong tx,uChar& nn) {
  short nEl  = 0;
  CTmObj* p  = pw;
  CTmObj* pp = p;
  CTmObj* pr = (p->tmo <= tx) ? p : NULL;
  if(pr) {
    while(p && (p->tmo <= tx)) {
      //DBG("CTimer::get0> N=%u-%u Tmo=%lu <> %lu Nxt-%s\n",nn,nEl,p->tmo,tx,(p->nxt?"nxtEl":"null"));
      pp = p;
      p  = p->nxt;
      ++nEl;
    }
    pp->nxt = pe;
    nn -= nEl;
    pw  = p;
  }
  return pr;
}

// Get ordered list of CTmObj objects with time < tx
CTmObj* CTmQueue::get(uLong tx) {
  uLong tb   = 0u;
  CTmObj* pp = NULL;
  Q_Ind b, w;
  {
    CSglLock sl(m);
    tb = tmBeg;
    b  = beg;
  }
  int x = (tx - tb) / delta;
  w = b + x;
  if(x < TMQSize) {
    {
      CSglLock sl(lm[w & MutArrSz]);
      if(nTimer[w]) {
	pp  = get0(pTmPool[w], NULL, tx, nTimer[w]);
      }
    }
    //DBG("CTimer::get> LVL=%u Tm=%lu-%lu nTimer[%u]=%u-%u\n",level,tx,nt,w,nTimer[w],b);
    if(b != w) {
      do {
	--w;
	{
	  CSglLock sl(lm[w & MutArrSz]);
	  if(nTimer[w]) {
	    pp = get0(pTmPool[w], pp, tx, nTimer[w]);
	  }
	}
	//DBG("CTimer::get>> Ind=<%u %u>-%u\n",w,nTimer[w],b);
      } while(b != w);
    }
    return pp;
  }
  LOG(L_ERR, "Error: timeout is out Level-0 of CTmQueue object %lu\n", tx);
  return NULL;
}

// ===================================


void CTmQueue::nextTmOut() {
  Q_Ind w,b;
  {
    CSglLock sl(m);
    b = w = beg;
  }
  --b;
  {
    while(w != b) {
      {
	CSglLock sl(lm[w & MutArrSz]);
	if(nTimer[w]) {
	  nt = pTmPool[w]->tmo;
	  return;
	}
      }
      ++w;
    }
    nt = 0xEFFFFFFF;
  }
}

// CTmQueue reorganisation to normal form
void CTmQueue::normalise(uLong tn) {
  Q_Ind normFlg = 0x0;
  {
    CSglLock sl(m);
    normFlg = (tn - tmBeg) / delta;
    tmBeg  += normFlg * delta;
    beg    += normFlg;
  }
  if(normFlg) {
    Q_Ind w = 0x0;
    if(nxtLevel) {
      uLong tnx = (delta * TMQSize) + tmBeg;
      CTmObj* p = nxtLevel->get(tnx);
      DBG("CTimer::normalise> LVL=%u Tn=%lu Tnx=%lu N-Delta=%u Lvl-%u PP-%s\n",
	  level,nt,tnx,normFlg,level,(p?"nxtEl":"null"));
      if(p) {
	CSglLock sl(m);
	w = beg + ((p->tmo - tmBeg) / delta);
      } else
	return;

      while(p) {
	CTmObj* pp = NULL;
	{
	  CSglLock sl(lm[w & MutArrSz]);
	  pTmPool[w] = p;
	  nTimer [w] = 0x1;
	  uLong x    = tmBeg + (nDelta(w) * delta);
	  DBG("CTimer::normalise> beg=%u w=%u <%u>\n",beg,w,nDelta(w));
	  while(p && (p->tmo < x)) {
	    nTimer[w] += 1;
	    pp = p;
	    p = p->nxt;
	  }
	  pp->nxt = NULL;
	}
	--w;
      }
      return nxtLevel->normalise(tnx);
    }
  }
}

// ===================================

CTmThrd* pTimeThrd = NULL;

CTmThrd::CTmThrd(): tQueue(L0Delta,0u) {
  LOG(L_NOTICE,"CTimer::CTmThrd - AppsTimer\n");
  tmQueue = &tQueue;
}

CTmThrd::~CTmThrd() {
  LOG(L_NOTICE,"CTimer::~CTmThrd - AppsTimer\n");
}

// return 'x'  - new timeOut was set
//        '\0' - send tmOut events
char CTmThrd::wait() {
  int ww = 0;
  char rr = '\0';
  while( ! rr ) {
    {
      CSglLock sl(wc);
      ww = wc.twait(tQueue.nt);
    }
    if( ETIMEDOUT == ww ) {
      uLong tx = tNow();
      DBG("CTimer::wait> Timer=%lu  %lu\n",tQueue.nt,tx);
      if(tx >= tQueue.nt) {
	rr = 'x';
      }
    } else {
      DBG("CTimer::wait> Add new Timer=%lu  %lu\n",tQueue.nt,tNow());
      rr = 'z';
    }
  }
  return (rr == 'z') ? 'x' : '\0';
}

void* CTmThrd::go() {
  pTimeThrd = this;
  LOG(L_NOTICE,"\n*** CTimerThrd::go ***\n");

  while(!(GlAppsFlag & APPL_EXIT)) {
    if( ! wait() ) {
      // Check pending timeouts
      CProcAllEv evProc;  // To signal Fifo on scope exit
      uLong Tt = tNow();
      DBG("CTimer::go> check pending Now-%lu\n",Tt);

      CTmObj* pTm = tQueue.get(Tt);
      while(pTm) {
	// Check for TOut_NoTmOut - to exit Timer
	if(pTm->type == TOut_NoTmOut)
	  return NULL;
	// put event in Fifo
	DBG("CTimer::go> putEv Tm=%lu id=%u Type=%u Inf=%u\n",
	    pTm->tmo,pTm->id,pTm->type,pTm->inf);
	  (void)GlEvFifo.addEl(CEvent::newEv(pTm->id,TOut_Shift+(pTm->type),0u,(uChar)(pTm->inf)));
	  // Del timerObj
	  CTmObj* pp = pTm;
	  pTm = pp->nxt;
	  pp->nxt = NULL;
	  CTmObj::delTm(pp);
      }
      tQueue.normalise(Tt);
      tQueue.nextTmOut();
    }
  }
  LOG(L_WARN,"CTimer::go> APPL_EXIT Now=%lu\n",tNow());
  return NULL;
}

// $Id: TimerNew.cpp 372 2010-05-08 15:29:49Z asus $
