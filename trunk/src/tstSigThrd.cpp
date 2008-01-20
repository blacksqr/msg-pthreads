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
#include <ext/hash_map>

extern uInt RRand();

//=================================================

CTstCtxtMgr::CTstCtxtMgr() {
  Nn = 0u;
  for(uShort k=0u; k<ctxtPoolSz; ++k)
    cPool[k] = k+2;
  memset(cPoolFlg,0,sizeof(cPoolFlg));
  cPool[ctxtPoolSz-1] = 0u;
  pFree = 1u;
}

uInt CTstCtxtMgr::Alloc(uInt ci) {
  DBG("CTstCtxtMgr::Alloc cId=%u nn=%u pFree=%u cPool=%u\n",ci,Nn,pFree,cPool[pFree-1]);
  if(pFree) {
    CSglLock sl(m);
    --pFree;
    uShort nx = cPool[pFree];
    cPool[pFree] = ci;
    hsh[ci] = pFree+1;
    cPoolFlg[pFree/8] |= _tmFlg_[pFree%8];
    pFree = nx;
    ++Nn;
    return ci;
  }
  return 0u;
}

// Get existing tst-context
uInt CTstCtxtMgr::get(uInt rnd) {
  if(!Nn)
    return 0u;
  {
    CSglLock sl(m);
    uShort x = rnd % ((Nn < ctxtPoolSz-16 ) ? Nn : (ctxtPoolSz-3));
    uChar  z = x % 8;
    x /= 8;
    DBG("CTstCtxtMgr::get %u/%u x=%u z=%u\n",pFree,Nn,x,z);
    while(!cPoolFlg[x])
      x = x ? (x-1) : ((ctxtPoolSz/8)-1);
    while(!(cPoolFlg[x] & _tmFlg_[z]))
      z = z ? (z-1) : 7;
    DBG("CTstCtxtMgr::get cId=cPool[%u]=%u\n",8*x+z,cPool[8*x+z]);
    return cPool[(8 * x) + z];
  }
}

void CTstCtxtMgr::Free(uInt ci) {
  {
    CSglLock sl(m);
    uShort nx = hsh[ci];
    if(nx) {
      --nx;
      hsh.erase(ci);
      cPool[nx] = pFree;
      cPoolFlg[nx/8] &= ~_tmFlg_[nx%8];
      pFree = nx+1;
      --Nn;
    } else
      DBG("CTstCtxtMgr::Free Error Ctxt=%u not found\n",ci);
  }
  DBG("CTstCtxtMgr::Free cId=%u nn=%u pFree=%u\n",ci,Nn,pFree);
}

CTstCtxtMgr tstCtxtMgr;

CTstSgTh::CTstSgTh(uShort m0,uShort m1,uChar t0,uChar t1):
  CEvThrd(5000/m0),nMsg0(m0),nMsg1(m1),tm0(t0),tm1(t1)
{
  nSend = 0u;
  nMsg  = nMsg0;
  DtTm   = 10u * tm0;
  tName = "tstSgThrd";
  DBG("CTstSgTh::CTstSgTh %u> m0=%u m1=%u t0=%u t1=%u DtTm=%d\n",
      sigThId,m0,m1,t0,t1,DtTm);
  sTm = tNow();
}

//=================================================

CEvent* CTstSgTh::getEvent() {
  uInt x = tNow() - sTm;
  if(x > DtTm) {
    LOG(L_ERR,"CTstSgTh::getEvent %u in Delta=%u send %u msg %u<>%u\n",
	sigThId,x,nSend,(100*(int)nSend)/x,nMsg);
    sTm += DtTm;
    x -= DtTm;
    nSend = 0u;
    DtTm = 10 * ((DtTm == (10u*tm0)) ? tm1 : tm0);
    nMsg = (nMsg == nMsg0) ? nMsg1 : nMsg0;
  }
  if((x * nMsg) < (100u * nSend)) {
    int timeOut = (10 * (100*(int)nSend - x*nMsg)) / nMsg;
    if(timeOut > 3) {
      struct timespec req;
      req.tv_sec  = timeOut / 1000;
      // 1000000000
      req.tv_nsec = 1000000 * (timeOut % 1000);
      (void)nanosleep(&req, NULL);
    }
  }
  ++nSend;
  uInt CId, vRnd = RRand();
  if(!(vRnd%3) && tstCtxtMgr.isFree()) {
    // Gen new tstCtxt
    CId = tstCtxtMgr.Alloc(CContext::reservId());
    DBG("CTstSgTh::getEvent %u Evnt_TstCntxt CId-new=%u\n",sigThId,CId);
    return CEvent::newEv(CId,Evnt_TstCntxt,0u,sigThId,'x');
  }
  CId = tstCtxtMgr.get(vRnd);
  if(CId) {
    DBG("CTstSgTh::getEvent %u Evnt_TstSeqw CId=%u nSend=%u\n",sigThId,CId,nSend);
    return CEvent::newEv(CId,Evnt_TstCntxt,0u,sigThId);
  }
  return NULL;
}

//=================================================

CTstSgThX::CTstSgThX(uShort m0,uShort m1):
  CEvThrd(5000/m0),nMsg0(2*m0),nMsg1(2*m1),tm0(7),tm1(2)
{
  tName = "tstSgThrd_2";
  nSend = 0u;
  DtTm = 10 * tm0;
  nMsg = nMsg0;
  ll = jj = ff = kk = 0x0;
}

CEvent* CTstSgThX::getEvent() {
  CEvent* ppe = NULL;
  if(ff < 5) {
    ll = jj = 0;
    sTm = tNow();
    // Get new context-id
    CId[ff] = CContext::reservId();
    DBG("CTstSgThX::getEvent %u> %d Evnt_TstCntxt CId=%u\n",sigThId,ff,CId[ff]);
    ppe = CEvent::newEv(CId[ff],Evnt_TstCntxt,0u,sigThId,'x');
    ++ff;
    return ppe;
  }
  if(jj < 11) {
    ++jj;
    int x = tNow() - sTm;
    if(x > DtTm) {
      LOG(L_ERR,"CTstSgThX::getEvent %u in Delta=%u send %u msg %u<>%u\n",
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
    //DBG("CTstSgThX::getEvent %u> %d Evnt_TstSeqw CId=%u nSend=%u\n",sigThId,kk,CId[kk],nSend);
    ppe = CEvent::newEv(CId[kk++],Evnt_TstSeqw,0u,sigThId);
    ++nSend;
    return ppe;
  }
  if(ll < 5) {
    ++ll;
    sTm = tNow();
    DBG("CTstSgThX::getEvent %u> %d Evnt_DelCtxt CId=%u\n",sigThId,ll,CId[ll]);
    ppe = CEvent::newEv(CId[ll],Evnt_DelCtxt,0u,sigThId);
    return ppe;
  }
  // tmOut to produce ev.thread-alarm
  struct timespec req;
  req.tv_sec  = 2 + ((11 * (int)kpAlive) / 100);
  req.tv_nsec = 0;
  (void)nanosleep(&req, NULL);
  DBG("CTstSgThX::getEvent %u>\n",sigThId);
  ff = 0;
  return NULL;
}
