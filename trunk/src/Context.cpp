#include <Timer.h>
#include <WThread.h>
#include <getTime.h>
#include <EvThread.h>

// Global context ID
uInt CContext::cGlId      = 0;
uInt CCnxtMap::numOfCntxt = 0;
CPMutex CContext::cGlIdm;
// House-keeping context ID
uInt HkCId;

// Global context map
CCnxtMap hashCntxt;

// Flag to stop new context
char stopNewCtxt ='\0';

void CCnxtMap::del(const uInt n) {
  CSglLock sl(m);
  DBG("Class CCtxtMap.erase[%d] %u\n",n,numOfCntxt);
  erase(n); // Called in context destructor
  --numOfCntxt;
}

char CCnxtMap::next(iterator& i,pVoid& v) {
  CSglLock sl(m);
  if(i != end()) {
    v = i->second; ++i;
    return '\0';
  }
  return 'x';
}

// ==================================================== 

CContext::CContext(uInt i): cId(i),ctxtType(0x0),rfCount(0x01) {
  lock();
  {
    // Add new context ID in hash
    CSglLock sl(hashCntxt.getM());
    hashCntxt[cId] = this;
    ++CCnxtMap::numOfCntxt;
  }
  seq0 = seq1 = '\0';
  DBG("Context::CContext %u Tot-%u\n",cId,CCnxtMap::numOfCntxt);
}

CContext::~CContext() {
  DBG("Context::~CContext> cId=%u\n",cId);
  hashCntxt.del(cId);
  unlock();
}

uInt CContext::reservId() {
  CSglLock sl(CContext::cGlIdm);
  DBG("Context::reservId> GcId=%u\n",cGlId+1);
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
  DBG("Context::setTimer> ID-%u tmType=%u tOut=%u\n",cId,type,(uInt)tm);
}
void CContext::delTimer(uChar type) {
  uChar a = type / 8;
  uChar b = type % 8;
  timerFlags[a] &= ~_tmFlg_[b]; // ~(0x01<<b);
  DBG("Context::delTimer> ID-%u tmType=%u\n",cId,type);
}
uChar CContext::isTimerOn(uChar type) {
  uChar a = type / 8;
  uChar b = type % 8;
  DBG("Context::isTimerOn> ID-%u tmType=%u\n",cId,type);
  return (timerFlags[a] & _tmFlg_[b]);
}

void CContext::remRef() {
  remRefHook();
  --rfCount;
  unlock();
  if(!rfCount) {
    DBG("Context::remRef> %u Ref>%d Seq %u<>%u\n",cId,rfCount,seq0,seq1);
    delete this;
  }
}

void CContext::destruct() {
  hashCntxt.del(cId);
  // Set del tmOut - 3,33 sec delete obj
  tmQueue.set(333u,TOut_DelCtxt,(uInt)this);
  DBG("Context::destruct %u> Ref-%u Seq %u<>%u\n",cId,rfCount,seq0,seq1);
}

CContext* CContext::findCtxt(uInt id) {
  return (HkCId == id) ? pHKContext : hashCntxt.get(id);
}

void CContext::Halt() {
  while(onHalt())
    yield();
  DBG("Context::Halt> %u Ref>%d Seq %u<>%u\n",cId,rfCount,seq0,seq1);
  destruct();
  remRef();
}

// ===============================================

CHKContext::CHKContext(uInt i): CContext(i) {
  HkCId = i;
  ctxtType = C_CTXT_HK;
  LOG(L_NOTICE,"CHKContext::CHKContext> cId=%u Seq %u<>%u\n",cId,seq0,seq1);
  unlock();
}

CHKContext::~CHKContext() {
  LOG(L_NOTICE,"<CHKContext::~CHKContext> destruction of HauseKeep context!!!\n");
}

uShort CHKContext::Run(CEvent* pe,CWThread* pwt) {
  uChar evId = pe->getEv();
  // save state of sending sign.thread
  DBG("CHKContext::Run> EventId=%u SThrd=%d\n",evId,pe->sigThId());
  if(evId == Evnt_Que12Full) {
    // on Evnt_Que12Full message - LOG output ...
    LOG(L_ERR,"CHKContext::Run cId=%u %u Evnt_Que12Full\n",cId,pwt->getWTId());
    ;
    return 0x0;
  }
  if(evId == Evnt_EndSigTh) {
    // Signal thread exited
    LOG(L_ERR,"CHKContext::Run %u Signaling Thread %d fail\n",pwt->getWTId(),pe->sigThId());
    ;
    return 0x0;
  }
  if(evId == Evnt_SThAlive) {
    // Signal thread no event's - timeout
    DBG("CHKContext::Run Sign.Thread %d Evnt_SThAlive\n",pe->sigThId());
    ;
    return 0x0;
  }
  if(evId == Evnt_HsKeepCtxt) {
    // HK-Event from event proc. context
    if(pe->sigThId() == hk_wTclFail)
      LOG(L_ERR,"CHKContext::Run %u wrkTcl-script fail ctxtId=%u\n",pwt->getWTId(),pe->dtId());
    ;
    return 0x0;
  }
  // if... {...}
  LOG(L_ERR,"<CHKContext::Run %u> Unhandled Event=%u\n",pwt->getWTId(),evId);
  return 0x01; // Error proc in Tcl
}

uShort CHKContext::onTimer(uLong tNn,CEvent* pe, CWThread* pwt) {
  static char rr = 0x0;
  rr = 0x01 - rr;
  DBG("CHKContext::onTimer> EventId=0x%X - %s\n",pe->getEv(),rr ? "LOOP" : "In TCL");
  switch(pe->getEv()) {
    case TOut_sigThAlrm: {
      // on TOut_sigThAlrm - signal-thread Alarm
      uChar sThId = pe->sigThId() - 1;
      uInt dlt = g_evThrdArr[sThId]->getTm();
      uInt tn  = tNn - dlt;
      DBG("<CHKContext::onTimer> Signal thread %s[%u] Alarm Now=%u  %u  %u\n",
	  g_evThrdArr[sThId]->thName(),sThId,(uInt)tNn,(uInt)dlt,
	  (uInt)g_evThrdArr[sThId]->getKATime());
      if(tn > 2u * g_evThrdArr[sThId]->getKATime()) {
	// some actions ... HookFunction !!!
	LOG(L_ERR,"<CHKContext::onTimer> Error Signal thread %s[%u] no responce %u/%u\n",
	    g_evThrdArr[sThId]->thName(),sThId+1,tn,(uInt)dlt);
      }
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
	    LOG(L_ERR,"<CHKContext::onHsKeep WThread %u State %u proc.msg %u BLOCKED %u s\n",
		k+1,st,pWrkThArr[k]->gMId(),tt);
	    // some action ...
	  }
	}
	if(pWrkThArr[k]->getNMsg() > 9) {
	  // print statistic info
	  const uInt* st = pWrkThArr[k]->getTimes();
	  // Message processing timing
	  DBG("CHKContext::onHsKeep<# %u NMsg=%u\n\t%u \t%u \t%u \t%u\n",
	      k,pWrkThArr[k]->getNMsg(),st[0],st[1],st[2],st[3]);
	}
	DBG("CHKContext::onHsKeep># Now=%u wtId=%u cId=%u msgId=%u thState=%u nWait=%u\n",
	    (uInt)tNn,k,pWrkThArr[k]->gCtxtId(),pWrkThArr[k]->gMId(),st,pWrkThArr[k]->gNWiat());
      }
      break;
    }
    default:
      LOG(L_ERR,"<CHKContext::onTimer> Error unknow timer Ev=%u\n",pe->getEv());
  }
  return rr;
}
