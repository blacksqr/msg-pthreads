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
#include <getTime.h>
#include <EvThread.h>

// Global context ID
uInt CContext::cGlId = 0u;
CPMutex CContext::cGlIdm;
// House-keeping context ID
uInt HkCId;

// Global context map
CCnxtMap hashCntxt;

// Flag to stop new context
char stopNewCtxt ='\0';

// Called from context destructor
void CCnxtMap::del(const uInt n) {
  CSglLock sl(m);
  DBG("CCtxtMap.erase[%d] %u\n",n,size());
  erase(n);
}

// ==================================================== 

CContext::CContext(uInt i): cId(i),ctxtType(0x0),rfCount(0x01) {
  lock();
  {
    // Add new context ID in hash
    CSglLock sl(hashCntxt.getM());
    hashCntxt[cId] = this;
  }
  seq0 = seq1 = '\0';
  DBG("Context::CContext %u Tot-%u\n",cId,hashCntxt.size());
}

CContext::~CContext() {
  DBG("Context::~CContext cId=%u\n",cId);
  unlock();
}

uInt CContext::reservId() {
  CSglLock sl(CContext::cGlIdm);
  DBG("Context::reservId GcId=%u\n",cGlId+1);
  return ++cGlId;
}

void CContext::send(short e, uInt id, pVoid pp, char n) {
  CEvent* pe = CEvent::newEv(id,e,pp,0,n);
  (void)GlEvFifo.addEl(pe);
  GlEvFifo.signal();
}

void CContext::send0(short e, uInt id, pVoid pp, char n) {
  CEvent* pe = CEvent::newEv(id,e,pp,0,n);
  (void)GlEvFifo.addTopEl(pe);
  GlEvFifo.signal();
}

void CContext::setTimer(uLong tm,uChar type,uShort f) {
  uChar a = type / 8;
  uChar b = type % 8;
  timerFlags[a] |= _tmFlg_[b]; // 0x01<<b;
  tmQueue.set(tm,type,cId,f);
  DBG("Context::setTimer ID-%u tmType=%u tOut=%u\n",cId,type,(uInt)tm);
}
void CContext::delTimer(uChar type) {
  uChar a = type / 8;
  uChar b = type % 8;
  timerFlags[a] &= ~_tmFlg_[b]; // ~(0x01<<b);
  DBG("Context::delTimer ID-%u tmType=%u\n",cId,type);
}
uChar CContext::isTimerOn(uChar type) {
  uChar a = type / 8;
  uChar b = type % 8;
  DBG("Context::isTimerOn ID-%u tmType=%u\n",cId,type);
  return (timerFlags[a] & _tmFlg_[b]);
}

void CContext::remRef() {
  --rfCount;
  if(!rfCount) {
    DBG("Context::remRef %u   Seq %u<>%u\n",cId,seq0,seq1);
    lock();
    delHook();
  } else if(rfCount > 0) {
    remRefHook();
    unlock();
  } else
    LOG(L_ERR,"Context::remRef Error %u Seq %u<>%u Ref=%d\n",
	cId,seq0,seq1,rfCount);
}

void CContext::destruct() {
  DBG("Context::destruct %u Ref-%u Type=%d Seq %u<>%u\n",cId,rfCount,ctxtType,seq0,seq1);
  hashCntxt.del(cId);
  // Set del tmOut - 0,03s delete obj
  send(Evnt_DelCtxt,(uInt)this);
}

CContext* CContext::findCtxt(uInt id) {
  return (HkCId == id) ? pHKCtxt : hashCntxt.get(id);
}

void CContext::Halt() {
  while(onHalt())
    yield();
  DBG("Context::Halt %u Ref>%d Seq %u<>%u\n",cId,rfCount,seq0,seq1);
  destruct();
}

int CContext::cmdProc(CWrkTcl* interp, int argc, Tcl_Obj* const argv[]) {
  const char* sCmd = interp->tGetVal(argv[1]);
  DBG("CContext::cmdProc sCmd - %s\n",sCmd);
  switch(*(sCmd++)) {
    case 's':  // sndev - send event from context
      if(!strcmp("ndev",sCmd) && (argc == 5)) {
	TGVal(short,ev,argv[2]);
	TGVal(int,id,argv[3]);
	TGVal(int,pp,argv[4]);
	if(id >= 0) {
	  send(ev, id ? id : getId(), (pVoid)pp);
	} else
	  send0(ev, -id, (pVoid)pp);
      }
      break;
    case 't':  // tmout - set timeout from context
      if(!strcmp("mout",sCmd) && (argc == 5)) {
	TGVal(int,tm,argv[2]);
	TGVal(short,type,argv[3]);
	TGVal(int,f,argv[4]);
	setTimer((uLong)tm,(uChar)type,(uShort)f);
      }
      break;
    default :
      LOG(L_ERR,"CContext::cmdProc Error <%s>\n",sCmd);
  }
  return TCL_OK;
}

// ===============================================

CHKCtxt::CHKCtxt(uInt i): CContext(i) {
  HkCId = i;
  ctxtType = C_CTXT_HK;
  LOG(L_NOTICE,"CHKCtxt::CHKCtxt cId=%u Seq %u<>%u\n",cId,seq0,seq1);
  unlock();
}

CHKCtxt::~CHKCtxt() {
  HkCId = 0u;
  pHKCtxt = NULL;
  LOG(L_NOTICE,"CHKCtxt::~CHKCtxt destruction of HauseKeep context!!!\n");
}

uShort CHKCtxt::Run(CEvent* pe,CWThread* pwt) {
  uChar evId = pe->getEv();
  // save state of sending sign.thread
  DBG("CHKCtxt::Run EventId=%u SThrd=%d\n",evId,pe->sigThId());
  if(evId == Evnt_Que12Full) {
    // on Evnt_Que12Full message - LOG output ...
    LOG(L_ERR,"CHKCtxt::Run cId=%u %u Evnt_Que12Full\n",cId,pwt->getWTId());
    ;
    return 0x0;
  }
  if(evId == Evnt_EndSigTh) {
    // Signal thread exited
    LOG(L_ERR,"CHKCtxt::Run %u Signaling Thread %d fail\n",pwt->getWTId(),pe->sigThId());
    ;
    return 0x0;
  }
  if(evId == Evnt_SThAlive) {
    // Signal thread no event's - timeout
    DBG("CHKCtxt::Run Sign.Thread %d Evnt_SThAlive\n",pe->sigThId());
    ;
    return 0x0;
  }
  if(evId == Evnt_HsKeepCtxt) {
    // HK-Event from event proc. context
    if(pe->sigThId() == hk_wTclFail)
      LOG(L_ERR,"CHKCtxt::Run %u wrkTcl-script fail ctxtId=%u\n",pwt->getWTId(),pe->dtId());
    ;
    return 0x0;
  }
  // if... {...}
  LOG(L_ERR,"CHKCtxt::Run %u Unhandled Event=%u\n",pwt->getWTId(),evId);
  return 0x01; // Error proc in Tcl
}

extern char GlobEvThreadStoped;

uShort CHKCtxt::onTimer(uLong tNn,CEvent* pe, CWThread* pwt) {
  DBG("CHKCtxt::onTimer EventId=0x%X Now=%u\n",pe->getEv(),(uInt)tNn);
  switch(pe->getEv()) {
    case TOut_sigThAlrm: {
      // on TOut_sigThAlrm - signal-thread Alarm
      uChar sThId = pe->sigThId();
      if(!GlobEvThreadStoped && sThId--) {
	uInt dlt = g_evThrdArr[sThId]->getTm();
	uInt tn  = tNn - dlt;
	DBG("CHKCtxt::onTimer Signal thread %s[%u] Alarm Now=%u  %u  %u\n",
	    g_evThrdArr[sThId]->thName(),sThId,(uInt)tNn,(uInt)dlt,
	    (uInt)g_evThrdArr[sThId]->getKATime());
	if(tn > 2u * g_evThrdArr[sThId]->getKATime()) {
	  // some actions ... HookFunction !!!
	  LOG(L_ERR,"CHKCtxt::onTimer Error Signal thread %s[%u] no responce %u/%u\n",
	      g_evThrdArr[sThId]->thName(),sThId+1,tn,(uInt)dlt);
	}
      } else
	LOG(L_ERR,"CHKCtxt::onTimer Error TOut_sigThAlrm sThId==0\n");
      break;
    }
    case TOut_HsKeep: {
      setTimer(222, TOut_HsKeep);  // set new hausekeep event
      // on hausekeep timer - check state of all work threads
      for(uChar k=0x0; k<CWThread::nRThrd(); ++k) {
	if(!pWrkThArr[k]) continue;
	uChar st = pWrkThArr[k]->gStat();
	if(st && (st & (PROC_EV | PROC_TM))) {
	  // Thread process event
	  uInt tt = tNn - pWrkThArr[k]->gMTime();
	  if(tt > 11) {
	    LOG(L_ERR,"CHKCtxt::onHsKeep WThread %u State %u proc.msg %u BLOCKED %u s\n",
		k+1,st,pWrkThArr[k]->gMId(),tt);
	    // some action ...
	  }
	}
	DBG("CHKCtxt::onHsKeep wtId=%u \tTt=%u \tcId=%u \tmsgId=%u \tthState=%u \tnWait=%u\n",
	    k+1,pWrkThArr[k]->gMTime(),pWrkThArr[k]->gCtxtId(),
	    pWrkThArr[k]->gMId(),st,pWrkThArr[k]->gNWiat());
      }
      break;
    }
    default:
      LOG(L_ERR,"CHKCtxt::onTimer Error unknow timer Ev=%u\n",pe->getEv());
  }
  return 0u;
}

int CHKCtxt::cmdProc(CWrkTcl* interp, int argc, Tcl_Obj* const argv[]) {
  const char* sCmd = interp->tGetVal(argv[1]);
  DBG("CHKCtxt::cmdProc sCmd - %s\n",sCmd);
  if(!strcmp(sCmd, "apStat")) {
    Tcl_SetObjResult(interp->getInterp(),tSetObj(getStatus()));
    return TCL_OK;
  }
  return CContext::cmdProc(interp,argc,argv);
}

