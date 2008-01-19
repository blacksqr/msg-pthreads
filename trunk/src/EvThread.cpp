// $Id$
#include <stlObj.h>
#if WITH_LIZENS
#include <ItsLizens.h>
#endif // WITH_LIZENS
#include <getTime.h>
#include <Context.h>
#include <EvThread.h>
#include <dprint.h>

// ID of House-keeping context
extern uInt HkCId;
uChar CEvThrd::g_sigThId = '\0';
pCEvThrd g_evThrdArr[MAX_N_STHREAD];

CEvThrd::CEvThrd(uShort ka): sigThId(++(CEvThrd::g_sigThId)) {
#if WITH_LIZENS
  // Lizens Code
  bool lizOK = validateApp();
  if(!lizOK) {
    LOG(L_WARN, lizError, tNOW);
    exit(2);
  }
#endif // WITH_LIZENS
  if(g_sigThId == 1) {
    // first time init
    for(uChar k=0x0; k<MAX_N_STHREAD; ++k)
      g_evThrdArr[k] = NULL;
  }
  Flag = 0x0;
  kpAlive = (ka<40) ? 40 : ka;
  g_evThrdArr[sigThId-1] = this;
  DBG("CEvThrd::CEvThrd> hrdArr[%u] This=0x%X\n",sigThId,(int)this);
  tmOfLastMsg = 0u;
}

// only check preConditions & Translate events in FSM-events
extern CHKContext* pHKContext;

void* CEvThrd::go() {
  CEvent* ev = NULL;
  {
    // wait 0.33 sec
    struct timespec req;
    req.tv_sec  = 0; req.tv_nsec = 333333333;
    (void)nanosleep(&req, NULL);
  }
  LOG(L_NOTICE,"\n*** CEvThrd::go ***\n");
  while(!(Flag & STOP_EV_THRD)) {
    ev = getEvent();
    if(!ev) {
      // send ev.thread keep alive msg.
      DBG("CEvThrd::go %s> new Evnt_SThAlive Now=%u\n",thName(),(uInt)tNow());
      ev = CEvent::newEv(HkCId,Evnt_SThAlive,0u,sigThId);
    }
    //DBG("CEvThrd::go %s> put ev-signal Th-id %u\n",thName(),sigThId);
    ev->put(); ev->sign();
  }
  // thread termination event
  ev = CEvent::newEv(HkCId,Evnt_EndSigTh,0u,sigThId);
  ev->put(); ev->sign();
  LOG(L_WARN,"CEvThrd::go %s> TERMINATED Now=%u\n",thName(),(uInt)tNow());
  return NULL;
}
