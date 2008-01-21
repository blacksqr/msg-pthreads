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
#include <getTime.h>
#include <WThread.h>
#include <EvThread.h>

#include <tstCntxt.h>
#include <dbCntxt.h>
#include <cashCtxt.h>

char  CWThread::eFlag = '\0';
extern CHKContext* pHKContext;
char  CWThread::g_WStatStart = '\0';  // Statistic OFF / ON
uChar CWThread::nRun         = '\0';  // global number of running threads
uChar CWThread::nReload      = '\0';  // global number of threads, reloaded Tcl-script

// array of pointer to working threads
pWThread pWrkThArr[MAX_N_WTHREAD];
// ========================================

CWThread::CWThread(uChar id):
  //CThreadObj('x')  - for SCHED_RR   politic
  //CThreadObj('\0') - for SCHED_FIFO politic
  CThreadObj('x'),nMsgTot(0u),pe(NULL),pCont(NULL),
  wtId(++id),seq('\0'),nMsg(0u),msgTm(0u),stat(0u)
{
  ++nRun;
  pWrkThArr[wtId-1] = this;
  stat = TH_INST;
  Tcl.Init(this);
  WStstRun = 0x0;
  // Statstic
  Tx0 = Tx1 = Tx2 = Tx3 = nWait = 0u;
  nWait = 0u;
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
    us = (1000000 + x.tv_usec) - tmVal.tv_usec;
    ss = x.tv_sec - tmVal.tv_sec - 1;
  }
  tmVal.tv_sec = x.tv_sec;
  tmVal.tv_usec = x.tv_usec;
  return 1000000 * ss + us;
}

// Update alarm timer for signal thread, disable the old one
static void newSigThEv(uLong tn,uChar sThId) {
  uInt dlt = g_evThrdArr[sThId]->getKATime();
  uInt kaTm = dlt / 3;
  //DBG("newSigTnEv> sigThId=%u check Alarm tmOut=%u\n",sThId+1,(uInt)(5*kaTm));
  if((tn - g_evThrdArr[sThId]->getTm()) > kaTm) {
    DBG("newSigTnEv sigThId=%u new check Alarm Now-%u/%u tmOut=%u\n",
	sThId+1,(uInt)tn,(uInt)g_evThrdArr[sThId]->getTm(),dlt);
    g_evThrdArr[sThId]->setTm(tn);  // Store Time of last tmOut
    tmQueue.set(3+dlt,TOut_sigThAlrm,HkCId,sThId+1);
  }
}

// Factory to produce new context & put it into hash
CContext* CWThread::genNewCtx(CEvent* pe) {
  switch(pe->getEv()) {
    case Evnt_SaveData:
      DBG("CWThread::genNewCtx %u> CCashCtxt\n",(uInt)pe->getCId());
      return new CCashCtxt(pe->getCId(),pe->dtId(),77,8);
    case Evnt_DbReady:
      DBG("CWThread::genNewCtx %u> CDbCtxt\n",(uInt)pe->getCId());
      return new CDbCntx(pe->getCId());
  }
  //if(Evnt_TstCntxt == pe->getEv()) {
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

// Return '\0' - go in TCL
//        'x'  - get new Event
uShort CWThread::getEvent() {
  uChar evType = 0x0;
  DBG("CWThread::getEvent> start from TCL\n");
  // Internal get event loop
  for(;;) {
    // clean last event
    if(pe) {
      CEvent::delEv(pe);
      pe = NULL;
    }
    if(pCont) {
      // release last Context lock
      DBG("_LOCK %u unlock Seq %d< %d >%d\n",pCont->cId,pCont->seq0,seq,pCont->seq1);
      pCont->remRef();
      pCont = NULL;
    }
    // Statistic
    if(g_WStatStart != WStstRun) {
      // Start statistic
      WStstRun = g_WStatStart;
      ::gettimeofday(&tmVal, 0);
      Tx0 = Tx1 = Tx2 = Tx3 = nMsg = nWait = 0u;
    } else if(WStstRun) {
      ++nMsg;
      Tt3 = tmDelta();
      uInt dltTm = Tx0 + Tx1 + Tx2 + Tx3;
      if(dltTm > 10000000u) {
	dltTm /= 1000;  // Delta Tm in 0.1s
	LOG(L_ERR,"CWThread::getEvent Stat %u A=%u Msg/s=%u> %u \t%u \t%u \t%u \t%u<>%u \t%u\n",
	    wtId,dltTm,(1000*nMsg)/dltTm,Tx0/nMsg,Tx1/nMsg,Tx2/nMsg,Tx3/nMsg,nMsg,nMsgTot,nWait);
	Tx0 = Tx1 = Tx2 = Tx3 = nMsg = nMsgTot = nWait = 0u;
      }
      Tx0 += Tt0; Tx1 += Tt1; Tx2 += Tt2; Tx3 += Tt3;
    }
    // Get new Event/Message
    stat &= ~(PROC_EV | PROC_TM);
    {
      // lock msg.queue
      CSglLock sl(GlEvFifo.getCond());
      pe = (CEvent*)GlEvFifo.getEl();
      if(WStstRun) Tt0 = tmDelta();
      evType = pe->getEv();
      DBG("CWThread::getEvent %u> Event Ctxt-%u Type=0x%X sigTn=%u\n",
	  wtId,pe->getCId(),evType,pe->sigThId());
      if(evType == Evnt_wThrdEnd) {
	CEvent::delEv(pe);
	pe = NULL;
	return 0x0;  // reload wrkTcl-Script
      }
      if(evType == (TOut_Shift + TOut_DelCtxt)) {
	// tmOut to delete context
	pCont = (CContext*)(pe->getCId());
	continue;    // Wait new event
      }
      if(HkCId != pe->getCId()) { // Not hauseKeep
	if(pe->newCtx()) {        // if event to produce new context
	  LOG(L_INFO,"CWThread::getEvent> NEW Context cId=%u\n",pe->getCId());
	  goto gNewCtx;
	}
	pCont = hashCntxt.get(pe->getCId());
      } else
	pCont = pHKContext;  // HausKeep
      if(pCont && pCont->rfCount) {
	pCont->addRef();
	seq = pCont->seq0++; // save msg-get seq. & inc. it
	DBG("_LOCK %u set Seq %d< %d >%d\n",pCont->cId,pCont->seq0,seq,pCont->seq1);
      } else {
	LOG(L_WARN,"CWThread::getEvent> no Context found cId=%u\n",pe->getCId());
	pCont = NULL;
	continue; // no Context found - wait new event
      }
    }
    msgTm = tNow();
    pCont->lock();
    // msg.queue unlocked - Cntxt locked
    if(WStstRun) Tt1 = tmDelta();
    // hsKeep msg from sign.thread
    if(pe->sigThId() && (evType < TOut_Shift))
      newSigThEv(msgTm,pe->sigThId() - 1); // Msg from sigThread - some hauseKeeping
    if(pCont->rfCount > 0) {
      char isTimer = 0x0;
      DBG("_LOCK %u lock Seq %d< %d >%d\n",pCont->cId,pCont->seq0,seq,pCont->seq1);
      while(seq != pCont->seq1) { // check predikat
	pCont->unlock();
	++nWait;
	LOG(L_WARN,"_C_LOCK %u> Id=%u Type=%d Ev=%d wt_Seq %d<>%d\n",
	    wtId,pCont->cId,pCont->getCtxtType(),evType,pCont->seq0,pCont->seq1);
	yield();
	pCont->lock();  // addRef() - disallow delete this Ctxt
      }
      // acquired OK
      ++(pCont->seq1);  // inc msg-proc seq.
      if(WStstRun) { Tt2 = tmDelta(); ++nMsgTot; }
      // Set stat bits to null
      stat &= ~(PROC_EV | PROC_TM);
      if(evType > TOut_Shift) {
	// Timer event
	isTimer = 'x';
	pe->stripTm();
	stat |= PROC_TM;
	if((HkCId != pe->getCId()) && !pCont->isTimerOn(evType - TOut_Shift)) {
	  DBG("CWThread::getEvent> %u timer not active Type=0x%X thId=%u\n",
	      pCont->cId,evType-TOut_Shift,pe->sigThId());
	  continue;     // timer not active
	}
      } else
	stat |= PROC_EV;
      cntxtId = pCont->getId();
      uShort xRet = isTimer ? pCont->onTimer(msgTm,pe,this) : Run(pe);
      DBG("CWThread::getEvent===> %u %s\n",pCont->cId,(xRet ? "Go_in_Tcl" : "Get_new_Ev"));
      if(xRet)
	return xRet;    // return in TCL
      continue;         // Wait new event
    } else
      continue;         // Wait new event
gNewCtx:
    if(!stopNewCtxt) {
      // Factory to produce new context & add it to hash
      pCont = genNewCtx(pe);
      if(pCont) {
	pCont->addRef();
	if(WStstRun) { Tt2 = tmDelta(); ++nMsgTot; }
	uShort xRet = Run(pe);
	if(xRet)
	  return xRet;    // return in TCL
      } else {
	DBG("CWThread::getEvent> NULL=genNewCtx(Type=0x%X ThId=%u)\n",pe->getEv(),pe->sigThId());
	// Some Error action's ..............
      }
    }
  }
}

void* CWThread::go() {
  stat = TH_RUN;
  while(!eFlag) {
    ++nReload;
    while(nReload < nRun) {
      // Barier to wait
      DBG("CWThread::go %u YIELD %d<>%d\n",wtId,nReload,nRun);
      yield();
    }
    DBG("CWThread::go %u YIELD_End %d<>%d\n",wtId,nReload,nRun);
    nReload = nRun;
    if(Tcl.EvalFile("./tcl/wrkThread.tcl")) {
      LOG(L_CRIT,"CWThread::go %u> Reload TCL-Script Now=%u\n",wtId,(uInt)tNow());
      // Put Evnt_HsKeepCtxt - Inform HauseKeep about fail wrkTcl
      uInt ctxId = pCont ? pCont->getId() : 0u;
      CEvent* ev = CEvent::newEv(HkCId,Evnt_HsKeepCtxt,ctxId,hk_wTclFail);
      ev->put();
      ev->sign();
    }
  }
  LOG(L_WARN,"CWThread::go %u> TERMINATED\n",wtId);
  return NULL;
}

// $Log$
