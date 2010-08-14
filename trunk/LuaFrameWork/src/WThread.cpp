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
#include "tWrkLua.h"

#undef double
#define double double

#include <Timer.h>
#include <getTime.h>
#include <WThread.h>
#include <EvThread.h>

#include <dbCntxt.h>
#include <fsmCtxt.h>
#include <cashCtxt.h>
#include <tstCntxt.h>

char  CWThread::eFlag   = '\0';
uChar CWThread::nRun    = '\0';  // global number of running threads
uChar CWThread::nReload = '\0';  // global number of threads, reloaded Tcl-script

// global number of threads, waiting new event
volatile uChar CWThread::nWaiting = '\0';

extern CHKContext* pHKContext;

// array of pointer to working threads
pWThread pWrkThArr[MAX_N_WTHREAD];
// ========================================

CWThread::CWThread(uChar id):
  //CThreadObj('x')  - for SCHED_RR   politic
  //CThreadObj('\0') - for SCHED_FIFO politic
  CThreadObj('\0'),nMsgTot(0u),pe(NULL),pCont(NULL),
  seq('\0'),nMsg(0u),msgTm(0u),wState(0u)
{
  //////////////////////////////////////////////
  id = 0x0;
  while(pWrkThArr[id]) ++id;
  pWrkThArr[id++] = (CWThread*)31415926;
  wtId = id;
  //////////////////////////////////////////////
  ++nRun;
  WStstRun = 0x0;
  wState = TH_INST;
  nWait = 0u;
  DBG("CWThread::CWThread %u\n",id);
}

void CWThread::startWTh() {
  pWrkThArr[wtId-1] = this;
  start();
}

CWThread::~CWThread() {
  pWrkThArr[wtId-1] = NULL;
  --nRun;
  LOG(L_WARN,"CWThread::~CWThread %u %u> TERMINATED Now=%u\n",wtId,nRun,(uInt)tNow());
}

uInt CWThread::tmDelta() {
  STmVal x;
  int ss,us;
  ::gettimeofday(&x, 0);
  if(x.tv_usec > tmVal.tv_usec) {
    us = x.tv_usec - tmVal.tv_usec;
    ss = x.tv_sec - tmVal.tv_sec;
  } else {
    us = (MicroSec + x.tv_usec) - tmVal.tv_usec;
    ss = x.tv_sec - tmVal.tv_sec - 1;
  }
  tmVal.tv_sec = x.tv_sec;
  tmVal.tv_usec = x.tv_usec;
  return MicroSec * ss + us;
}

// Update alarm timer for signal thread and disable the old one
static void newSigThEv(uLong tn,uChar sThId) {
  uInt dlt = g_evThrdArr[sThId]->getKATime();
  uInt kaTm = dlt / 2;
  if((tn - g_evThrdArr[sThId]->getTm()) > kaTm) {
    DBG("newSigTnEv sigThId=%u new check Alarm Now-%u/%u tmOut=%u\n",
	sThId+1,(uInt)tn,(uInt)g_evThrdArr[sThId]->getTm(),dlt);
    // save time of last tmOut
    g_evThrdArr[sThId]->setTm(tn);
    pTimeThrd->set(7u+dlt,HkCId,TOut_sigThAlrm,sThId+1);
  }
}

// Factory to produce new context and put it in hash
CContext* CWThread::genNewCtx(pCEvent pe) {
  switch(pe->getEv()) {
  case Evnt_SaveData: {
    CContext* p = new CCashCtxt(pe->getCId(),pe->dtId(),77,8);
    p->hashCId();
    DBG("CWThread::genNewCtx %u> CCashCtxt\n",(uInt)pe->getCId());
    return p;
  }
  case Evnt_DbReady: {
    CContext* p = new CDbCntx(pe->getCId());
    p->hashCId();
    DBG("CWThread::genNewCtx %u> CDbCtxt\n",(uInt)pe->getCId());
    return p;
  }
  case Evnt_newFsm:
    DBG("CWThread::genNewCtx %u> FsmCtxt\n",(uInt)pe->getCId());
    return CFsmCtxt::newFsm(pe->getCId());
  case Evnt_TstCntxt:
    DBG("CWThread::genNewCtx %u> TstCntxt\n",(uInt)pe->getCId());
    return CSipCntx::newSCtxt(pe->getCId());
    //return CTstCntx::newTCtxt(pe->getCId());
  case Evnt_sipCtxt1:
  case Evnt_sipCtxt2:
    DBG("CWThread::genNewCtx %u> sipCtxt\n",(uInt)pe->getCId());
    return CSipCntx::newSCtxt(pe->getCId());
  default:
    if(!iCsCtxt) {
      iCsCtxt = CContext::reservId();
      iDbCtxt = CContext::reservId();
      // ===========================================
      DBG("CWThread::genNewCtx DbCtxtId=%u\n",iDbCtxt);
      CContext::send0(Evnt_DbReady,iDbCtxt,NULL,'x');
      DBG("CWThread::genNewCtx CashCtxtId=%u\n",iCsCtxt);
      CContext::send0(Evnt_SaveData,iCsCtxt,(pVoid)iDbCtxt,'x');
    }
    DBG("CWThread::genNewCtx %u CTstCntx Ev=%d\n",(uInt)pe->getCId(),pe->getEv());
    return CTstCntx::newTCtxt(pe->getCId());
    //LOG(L_ERR,"CWThread::genNewCtx for Event=%u no constructor found\n",pe->getEv());
    //return NULL;
  }
}

// Return '\0' - go in Lua
//        'x'  - get new Event
uShort CWThread::getEvent() {
  uChar evType = 0x0;

  // Internal get event loop, entry point from script
  DBG("CWThread::getEvent> start from Lua\n");

  // Process events from central apps queue
  for(;;) {
    // clean last event
    if(pe) {
      CEvent::delEv(pe);
      evType = 0x0;
      pe = NULL;
    }
    while(pCont) {
      // Process all events from Context queue
      {
	CSglLock sl(pCont->getQm());
	pe = pCont->enQueue();
      }
      DBG("CWThread::getEvent> Ev-0x%X from Context queue cId-%u\n",
          (pe ? pe->getEv() : 0),pCont->getId());
      evType = pe ? pe->getEv() : 0x0;
      if(evType && (evType != Evnt_DelCtxt)) {
	uShort xRet = 0u;
        if(evType > TOut_Shift) {
          // Timer event
          pe->stripTm();
          wState |= PROC_TM;
          if((HkCId != pe->getCId()) && !pCont->isTimerOn(evType - TOut_Shift)) {
            DBG("CWThread::getEvent> %u timer not active Type=0x%X thId=%u\n",
                pCont->cId,evType-TOut_Shift,pe->sigThId());
            continue;     // timer disabled in context
          }
          msgTm = tNow();
          xRet = pCont->onTimer(msgTm,pe,this);
        } else {
          wState |= PROC_EV;
          xRet =  Run(pe);
        }
        DBG("CWThread::getEvent===> %u %s\n", pCont->cId, (xRet ? "GoScript" : "NewEvent"));
        if(xRet) {
          return xRet;  // return in Lua
        } else
          continue;
      } else {
        // release last Context lock
        DBG("CWThread::getEvent> pCont->remRef() Seq %d< %d >%d\n",
            pCont->cId,pCont->seq0,seq,pCont->seq1);
        pCont->remRef();
        pCont = NULL;
      }
    }

    // Get new Event from central apps queue
    wState &= ~(PROC_EV | PROC_TM);
    ++nWaiting;      // WrkThread - waiting on condition
    {
      // lock msg.queue
      CSglLock sl(GlEvFifo.getCond());
      pe = (pCEvent)GlEvFifo.getEl();
      --nWaiting;
      evType = pe->getEv();
      DBG("CWThread::getEvent %u> Event cId-%u evType=0x%X sigTn=%u\n",
	  wtId,pe->getCId(),evType,pe->sigThId());
      if((evType == Evnt_wThrdEnd) && !pe->getCId()) {
	CEvent::delEv(pe);
	pe = NULL;
	return 0x0;  // reload wrk-script
      }
      if(evType == Evnt_DelCtxt) {
	// Event to remove context (delete)
	pCont = (CContext*)(pe->getCId());
        DBG("CWThread::getEvent> remove context cId-%u\n",pCont->getId());
      } else {
        if(HkCId != pe->getCId()) {
          // Not hauseKeep
          if(pe->newCtx()) {
            // Event to create new context
            LOG(L_INFO,"CWThread::getEvent> NEW Context cId=%u\n",pe->getCId());
            goto gNewCtx;
          }
          pCont = hashCntxt.get(pe->getCId());
        } else
          pCont = pHKContext; // HausKeep

        if(pCont && pCont->rfCount && (evType != Evnt_PreDelCtxt)) {
          pCont->addRef();     // to prevent context destruction
          seq = pCont->seq0++; // save msg-get seq. & inc. it
          DBG("CWThread::getEvent>_LOCK %u set Seq %d< %d >%d\n",
              pCont->cId,pCont->seq0,seq,pCont->seq1);
        } else {
          LOG(L_WARN,"CWThread::getEvent> No Context cId=%u\n",pe->getCId());
          pCont = NULL;
          continue; // no Context found - wait new event
        }
      }
    }
    // msg.queue unlocked
    msgTm = tNow();
    if(pe->sigThId() && (evType < TOut_Shift)) {
      // Msg from sigThread - hausKeeping event
      newSigThEv( msgTm,pe->sigThId() - 1 );
    }
    if( ! pCont->lTry() ) {
      // === Context locked - Run it (context's queue is empty) ===
      uShort xRet = 0u;
      DBG("CWThread::getEvent> cId-%u locked Ev-0x%X\n",pCont->getId(),evType);
      if(evType == Evnt_DelCtxt) continue;
      if(evType > TOut_Shift) {
	// Timer event
	pe->stripTm();
	wState |= PROC_TM;
	if((HkCId != pe->getCId()) && !pCont->isTimerOn(evType - TOut_Shift)) {
	  DBG("CWThread::getEvent> %u timer not active Type=0x%X thId=%u\n",
	      pCont->cId,evType-TOut_Shift,pe->sigThId());
	  continue;     // timer disabled in context
	}
	xRet = pCont->onTimer(msgTm,pe,this);
      } else {
	wState |= PROC_EV;
	xRet =  Run(pe);
      }
      DBG("CWThread::getEvent===> %u %s\n", pCont->cId, (xRet ? "GoScript" : "NewEvent"));
      if(xRet)
	return xRet;  // return in Lua
    } else {
      // Context busy - lock context's queue
      CSglLock sl(pCont->getQm());

      // check predikat
      while( seq != pCont->seq1 ) {
	pCont->qUnlock();
	++nWait;
	yield();
	pCont->qLock();
      }  // acquired OK
      DBG("_qLock %u Seq %d< %d >%d nWait=%d\n",
	  pCont->cId, pCont->seq0, seq, pCont->seq1, nWait);
      ++(pCont->seq1);  // inc msg-proc seq.

      // Set stat bits to null
      wState &= ~(PROC_EV | PROC_TM);
      // === Queue event in the context ===
      pCont->Queue(pe);
      pe = NULL;  // to keep event in queue

    }
    continue;         // Wait new event

gNewCtx:
    if(!stopNewCtxt) {
      // Call context factory to produce new context & add it to hash
      pCont = genNewCtx(pe);
      if(pCont) {
	pCont->addRef();
	if(WStstRun) ++nMsgTot;
	uShort xRet = Run(pe);
	if(xRet)
	  return xRet;    // return in Lua
      } else {
	DBG("CWThread::getEvent> NULL=genNewCtx(Type=0x%X ThId=%u)\n",pe->getEv(),pe->sigThId());
	// Some Error action's ..............
      }
    }
  }
}

extern char appsHomeArr[];
extern const char* appsHome;

static int getWLObj(lua_State* pl) {
  lua_getfield(pl, LUA_REGISTRYINDEX, "__IY__");
  return 1;
}

extern "C" {
  int luaopen_socket_core(lua_State *L);
  int luaopen_lsqlite3(lua_State *L);
}

void* CWThread::go() {
  // Work interpreter
  int  tolua_tWrkLua_open(lua_State*);
  CWrkLua WLua(lua_open(),this);
  luaL_openlibs(&WLua);

  luaopen_socket_core(&WLua);
  luaopen_lsqlite3(&WLua);

  tolua_tWrkLua_open(&WLua);

  lua_pushcfunction(&WLua, getWLObj);
  lua_setglobal(&WLua, "getWLuaObj");

  strcpy((char*)appsHome,"wrkTh.lua");

  wState = TH_RUN;
  while(!eFlag) {
    ++nReload;
    while(nReload < nRun) {
      // Barier to wait
      DBG("CWThread::go %u YIELD %d<>%d\n",wtId,nReload,nRun);
      yield();
    }
    WLua.init();
    nReload = nRun;
    strcpy((char*)appsHome,"wrkTh.lua");
    DBG("CWThread::go %u wrkTh.lua %d<>%d %s\n",wtId,nReload,nRun,appsHomeArr);
    {
      int rrr = luaL_dofile(&WLua,appsHomeArr);
      LOG(L_CRIT,"ERR - CWThread::go=%u wrkLuaScript fail-%d Now=%u\n",wtId,rrr,(uInt)tNow());
      if(rrr) {
        // Put Evnt_HsKeepCtxt - Inform HauseKeep about fail wrk-th
        uInt ctxId = pCont ? pCont->getId() : 0u;
        pCEvent ev = CEvent::newEv(HkCId,Evnt_HsKeepCtxt,ctxId,hk_wTclFail);
        ev->put();
        ev->sign();
      }
    }
  }
  lua_close(&WLua);
  LOG(L_WARN,"CWThread::go %u TERMINATED\n",wtId);
  return NULL;
}

void CWThread::setTimer(int cid, int tm, int type, int f) {
  cid ? pTimeThrd->set(tm,HkCId,type,f) : pCont->setTimer((uLong)tm,(uChar)type,(uShort)f);
}

// Called from CEvent::sign method
uChar WThreadRun() {
  return CWThread::nRThrd();
}

// $Id: WThread.cpp 388 2010-05-15 21:27:40Z asus $
