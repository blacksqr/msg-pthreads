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
#include <dprint.h>
#include <getTime.h>
#include <tstSigThrd.h>

CEvent* CTstSgTh0::getEvent() {
  CEvent* ppe = NULL;
  if(ff < 1024) {
    sTm = tNow();
    // Get new context-id
    CId[ff] = CContext::reservId();
    DBG("CTstSgTh0::getEvent %u> %d Evnt_TstCntxt CId=%u\n",sigThId,ff,CId[ff]);
    ppe = CEvent::newEv(CId[ff],Evnt_TstCntxt,0u,sigThId,'x');
    ++nSend;
    ++ff;
    return ppe;
  }
  int timeOut = tNow() - sTm;
  if(timeOut > 200) {
    LOG(L_WARN,"CTstSgTh0::getEvent %u in Delta=%u send %u msg %u<>%u\n",
	sigThId,timeOut,nSend,(100*(int)nSend)/timeOut,nMsg/2);
    sTm += 200;
    timeOut -= 200;
    nSend = 0u;
  }
  timeOut = ((2000 * nSend++) / nMsg) - (10 * timeOut);
  if(timeOut > 11) {
    struct timespec req;
    req.tv_sec  = timeOut / 1000;
    // 1000000000
    req.tv_nsec = 1000000 * (timeOut % 1000);
    (void)nanosleep(&req, NULL);
  }
  kk = (kk > 1023) ? 0 : kk;
  //DBG("CTstSgTh0::getEvent %u> %d Evnt_TstSeqw CId=%u nSend=%u\n",sigThId,kk,CId[kk],nSend);
  ppe = CEvent::newEv(CId[kk++],Evnt_TstSeqw,0u,sigThId);
  return ppe;
}

//=================================================

CTstSgTh1::CTstSgTh1(uShort m0,uShort m1,uChar t0,uChar t1):
  CEvThrd(5000/m0),nMsg0(m0),nMsg1(m1),tm0(t0),tm1(t1)
{
  nSend = 0u;
  nMsg  = nMsg0;
  DtTm   = 10 * tm0;
  tName = "tstSgThrd_1";
  ll = jj = ff = kk = 0x0;
  DBG("CTstSgTh1::CTstSgTh1 %u> m0=%u m1=%u t0=%u t1=%u DtTm=%d\n",
      sigThId,m0,m1,t0,t1,DtTm);
}

CEvent* CTstSgTh1::getEvent() {
  CEvent* ppe = NULL;
  if(ff < 221) {
    ll = jj = 0;
    sTm = tNow();
    // Get new context-id
    CId[ff] = CContext::reservId();
    DBG("CTstSgTh1::getEvent %u> %d Evnt_TstCntxt CId=%u\n",sigThId,ff,CId[ff]);
    ppe = CEvent::newEv(CId[ff],Evnt_TstCntxt,0u,sigThId,'x');
    ++nSend;
    ++ff;
    return ppe;
  }
  if(jj++ < 1111) {
    int x = tNow() - sTm;
    if(x > DtTm) {
      LOG(L_ERR,"CTstSgTh1::getEvent %u in Delta=%u send %u msg %u<>%u\n",
	  sigThId,x,nSend,(100*(int)nSend)/x,nMsg);
      sTm += DtTm;
      x -= DtTm;
      nSend = 0u;
      DtTm = 10 * ((DtTm == (10 * tm0)) ? tm1 : tm0);
      nMsg = (nMsg == nMsg0) ? nMsg1 : nMsg0;
    }
    if(x * nMsg < 100 * (int)nSend) {
      int timeOut = (10 * (100*(int)nSend - x*nMsg)) / nMsg;
      if(timeOut > 3) {
	struct timespec req;
	req.tv_sec  = timeOut / 1000;
	// 1000000000
	req.tv_nsec = 1000000 * (timeOut % 1000);
	(void)nanosleep(&req, NULL);
      }
    }
    kk = (kk > 221) ? 0 : kk;
    ++nSend;
    DBG("CTstSgTh1::getEvent %u> %d Evnt_TstSeqw CId=%u nSend=%u\n",sigThId,kk,CId[kk],nSend);
    return CEvent::newEv(CId[kk++],Evnt_TstSeqw,0u,sigThId);
  }
  if(ll++ < 221) {
    ++nSend;
    sTm = tNow();
    DBG("CTstSgTh1::getEvent %d> Evnt_DelCtxt CId=%u\n",ll,CId[ll]);
    return CEvent::newEv(CId[ll],Evnt_DelCtxt,0u,sigThId);
  }
  ff = 0;
  return NULL;
}

//=================================================

CTstSgTh2::CTstSgTh2(uShort m0,uShort m1):
  CEvThrd(5000/m0),nMsg0(2*m0),nMsg1(2*m1),tm0(7),tm1(2)
{
  tName = "tstSgThrd_2";
  nSend = 0u;
  DtTm = 10 * tm0;
  nMsg = nMsg0;
  ll = jj = ff = kk = 0x0;
}

CEvent* CTstSgTh2::getEvent() {
  CEvent* ppe = NULL;
  if(ff < 5) {
    ll = jj = 0;
    sTm = tNow();
    // Get new context-id
    CId[ff] = CContext::reservId();
    DBG("CTstSgTh2::getEvent %u> %d Evnt_TstCntxt CId=%u\n",sigThId,ff,CId[ff]);
    ppe = CEvent::newEv(CId[ff],Evnt_TstCntxt,0u,sigThId,'x');
    ++ff;
    return ppe;
  }
  if(jj < 11) {
    ++jj;
    int x = tNow() - sTm;
    if(x > DtTm) {
      LOG(L_ERR,"CTstSgTh2::getEvent %u in Delta=%u send %u msg %u<>%u\n",
	  sigThId,x,nSend,(100*(int)nSend)/x,nMsg/2);
      sTm += DtTm;
      x -= DtTm;
      nSend = 0u;
      DtTm = 10 * ((DtTm == (10 * tm0)) ? tm1 : tm0);
      nMsg = (nMsg == nMsg0) ? nMsg1 : nMsg0;
    }
    int timeOut = ((2000 * nSend++) / nMsg) - (10 * x);
    if(timeOut > 4) {
      struct timespec req;
      req.tv_sec  = timeOut / 1000;
      // 1000000000
      req.tv_nsec = 1000000 * (timeOut % 1000);
      (void)nanosleep(&req, NULL);
    }
    kk = (kk > 4) ? 0 : kk;
    //DBG("CTstSgTh2::getEvent %u> %d Evnt_TstSeqw CId=%u nSend=%u\n",sigThId,kk,CId[kk],nSend);
    ppe = CEvent::newEv(CId[kk++],Evnt_TstSeqw,0u,sigThId);
    ++nSend;
    return ppe;
  }
  if(ll < 5) {
    ++ll;
    sTm = tNow();
    DBG("CTstSgTh2::getEvent %u> %d Evnt_DelCtxt CId=%u\n",sigThId,ll,CId[ll]);
    ppe = CEvent::newEv(CId[ll],Evnt_DelCtxt,0u,sigThId);
    return ppe;
  }
  // tmOut to produce ev.thread-alarm
  struct timespec req;
  req.tv_sec  = 2 + ((11 * (int)kpAlive) / 100);
  req.tv_nsec = 0;
  (void)nanosleep(&req, NULL);
  DBG("CTstSgTh2::getEvent %u>\n",sigThId);
  ff = 0;
  return NULL;
}
