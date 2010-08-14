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

CTstCtxtMngr::CTstCtxtMngr() {
  Nn = 0u;
  for(uShort k=0u; k<ctxtPoolSz; ++k)
    cPool[k] = k+2;
  memset(cPoolFlg,0,sizeof(cPoolFlg));
  cPool[ctxtPoolSz-1] = 0u;
  pFree = 1u;
}

uInt CTstCtxtMngr::Alloc(uInt ci) {
  DBG("CTstCtxtMngr::Alloc cId=%u nn=%u pFree=%u cPool=%u\n",ci,Nn,pFree,cPool[pFree-1]);
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
uInt CTstCtxtMngr::get(uInt rnd) {
  if(!Nn)
    return 0u;
  {
    CSglLock sl(m);
    uShort x = rnd % ((Nn < ctxtPoolSz-16 ) ? Nn : (ctxtPoolSz-3));
    uChar  z = x % 8;
    x /= 8;
    DBG("CTstCtxtMngr::get %u/%u x=%u z=%u\n",pFree,Nn,x,z);
    while(!cPoolFlg[x])
      x = x ? (x-1) : ((ctxtPoolSz/8)-1);
    while(!(cPoolFlg[x] & _tmFlg_[z]))
      z = z ? (z-1) : 7;
    DBG("CTstCtxtMngr::get cId=cPool[%u]=%u\n",8*x+z,cPool[8*x+z]);
    return cPool[(8 * x) + z];
  }
}

void CTstCtxtMngr::Free(uInt ci)
{
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
      DBG("CTstCtxtMngr::Free Error Ctxt=%u not found\n",ci);
  }
  DBG("CTstCtxtMngr::Free cId=%u nn=%u pFree=%u\n",ci,Nn,pFree);
}

CTstCtxtMngr tstCtxtMngr;

//=================================================

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
      req.tv_nsec = MicroSec * (timeOut % 1000);
      (void)nanosleep(&req, NULL);
    }
  }
  ++nSend;
  uInt CId, vRnd = RRand();
  if(!(vRnd%3) && tstCtxtMngr.isFree()) {
    // Gen new tstCtxt
    CId = tstCtxtMngr.Alloc(CContext::reservId());
    DBG("CTstSgTh::getEvent %u Evnt_TstCntxt CId-new=%u\n",sigThId,CId);
    return CEvent::newEv(CId,Evnt_TstCntxt,0u,sigThId,'x');
  }
  CId = tstCtxtMngr.get(vRnd);
  if(CId) {
    DBG("CTstSgTh::getEvent %u Evnt_TstSeqw CId=%u nSend=%u\n",sigThId,CId,nSend);
    return CEvent::newEv(CId,Evnt_TstCntxt,0u,sigThId);
  }
  return NULL;
}

//=================================================

CTstFsmTh::CTstFsmTh(int m)
  : CEvThrd(5000/m),nMsg(1000000000/m)
{
  DBG("CTstFsmTh::CTstFsmTh %d\n",m);
  tName = "tstFsmTh";
  for(short k=0; k<FSM_ARR_SIZE; ++k) {
    fsmCtxtAr[k].state = 0u;
    fsmCtxtAr[k].CId   = 0u;
    fsmCtxtAr[k].time  = 0u;
  }
}

CEvent* CTstFsmTh::getEvent() {
  static struct timespec req;
  int kk = RRand() % FSM_ARR_SIZE;
  req.tv_sec = 0;
  req.tv_nsec = nMsg;
  (void)nanosleep(&req, NULL);
  switch(fsmCtxtAr[kk].state) {
    case Evnt_fsmEv1: {
      DBG("CTstFsmTh::getEvent Evnt_fsmEv1 CId=%u\n",fsmCtxtAr[kk].CId);
      fsmCtxtAr[kk].state = Evnt_fsmEv2;
      fsmCtxtAr[kk].time  = tNow();
      return  CEvent::newEv(fsmCtxtAr[kk].CId,Evnt_fsmEv2,0u,sigThId);
    }
    case Evnt_fsmEv2: {
      DBG("CTstFsmTh::getEvent Evnt_fsmEv2 CId=%u\n",fsmCtxtAr[kk].CId);
      fsmCtxtAr[kk].state = Evnt_fsmEv3;
      fsmCtxtAr[kk].time  = tNow();
      return  CEvent::newEv(fsmCtxtAr[kk].CId,Evnt_fsmEv3,0u,sigThId);
    }
    case Evnt_fsmEv3: {
      DBG("CTstFsmTh::getEvent Evnt_fsmEv3 CId=%u\n",fsmCtxtAr[kk].CId);
      fsmCtxtAr[kk].state = Evnt_fsmEv4;
      fsmCtxtAr[kk].time  = tNow();
      return  CEvent::newEv(fsmCtxtAr[kk].CId,Evnt_fsmEv4,0u,sigThId);
    }
    case Evnt_fsmEv4: {
      DBG("CTstFsmTh::getEvent Evnt_fsmEv4 CId=%u\n",fsmCtxtAr[kk].CId);
      fsmCtxtAr[kk].state = Evnt_fsmEv5;
      fsmCtxtAr[kk].time  = tNow();
      return  CEvent::newEv(fsmCtxtAr[kk].CId,Evnt_fsmEv5,0u,sigThId);
    }
    case Evnt_fsmEv5: {
      DBG("CTstFsmTh::getEvent Evnt_fsmEv5 CId=%u\n",fsmCtxtAr[kk].CId);
      fsmCtxtAr[kk].state = Evnt_fsmEv6;
      fsmCtxtAr[kk].time  = tNow();
      return  CEvent::newEv(fsmCtxtAr[kk].CId,Evnt_fsmEv6,0u,sigThId);
    }
    case Evnt_newFsm:
    case Evnt_fsmEv6:
    default: { // new context-id
      uInt id = CContext::reservId();
      DBG("CTstFsmTh::getEvent new context-id CId=%u\n",id);
      fsmCtxtAr[kk].state = Evnt_fsmEv1;
      fsmCtxtAr[kk].CId   = id;
      fsmCtxtAr[kk].time  = tNow();
      return  CEvent::newEv(id,Evnt_newFsm,0u,sigThId,'x');
    }
  }
}

//=================================================

template<typename T> class TMsgWrap {
private:
  T* pp;
public:
  TMsgWrap(T* p) : pp(p) {}
  ~TMsgWrap() { delete pp; }

  T& operator () () { return *pp; }
};
// Get object reference
template<typename T> T& r_(T x) {
  static T z = x; return z;
}

/////////////////////////////////

char* CTstSgThX::mxBuf = NULL;

CTstSgThX::CTstSgThX(uShort m0,uShort m1):
  CEvThrd(5000/m0),nMsg0(2*m0),nMsg1(2*m1),tm0(8),tm1(2)
{
  DBG("CTstSgThX::CTstSgThX %u %u\n",m0,m1);
  tName = "tstSgThrd_2";
  nSend = 0u;
  DtTm = 10 * tm0;
  nMsg = nMsg0;
  ll = jj = ff = kk = 0x0;
  if(!mxBuf) {
    char bxx[0x10000];
    int  fd = open("/tmp/msg_store.bin", O_RDONLY);
    bfLen =  read(fd, bxx, sizeof(bxx));
    close(fd);
    mxBuf = (char*)malloc(bfLen + 1);
    memmove(mxBuf, bxx, bfLen);
    nxMsg = mxBuf;
  }
}

CEvent* CTstSgThX::getEvent() {
  char rr = '\0';
  CEvent* ppe = NULL;
  if(ff < DIMCID/2) {
    kk = ll = jj = 0;
    sTm = tNow();
    // Get new context-id
    CId[ff] = CContext::reservId();
    DBG("CTstSgThX::getEvent %u ff-%d Evnt_TstCntxt CId=%u\n",sigThId,ff,CId[ff]);
    ppe = CEvent::newEv(CId[ff++],Evnt_TstCntxt,0u,sigThId,'x');
    return ppe;
  }
  if(jj < DIMCID) {
    ++jj;
    int x = tNow() - sTm;
    if(x > DtTm) {
      LOG(L_ERR,"CTstSgThX::getEvent %u jj-%d in Delta=%u send %u msg %u<>%u\n",
	  sigThId,jj,x,nSend,(100*(int)nSend)/x,nMsg/2);
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
      req.tv_nsec = MicroSec * (timeOut % 1000);
      (void)nanosleep(&req, NULL);
    }
    if(++kk > DIMCID) kk = 0;
    DBG("CTstSgThX::getEvent %u kk-%d Evnt_TstSeqw CId=%u nSend=%u\n",sigThId,kk,CId[kk],nSend);
    if(kk/2) {
      ppe = CEvent::newEv(CId[kk],Evnt_TstCntxt,0u,sigThId);
      ++nSend;
      return ppe;
    } else {
      PMessage pMsg = mFactory.reStore(nxMsg);
      printf("MsgFactory restore MsgId %u\n", pMsg->getMsgId());
      if((nxMsg-mxBuf) > (bfLen-7)) nxMsg = mxBuf;

      switch(pMsg->getMsgId()) {
        case 1: {
          CMsgGen_mymsg1* pMm1 = (CMsgGen_mymsg1*)pMsg;
          rr = pMm1->parse0();
          ppe = CEvent::newEv(CId[kk++],Evnt_sipCtxt1,pMm1,sigThId);
          ++nSend;
          return ppe;
        }
        case 2: {
          CMsgGen_mymsg2* pMm2 = (CMsgGen_mymsg2*)pMsg;
          rr = pMm2->parse0();
          ppe = CEvent::newEv(CId[kk++],Evnt_sipCtxt2,pMm2,sigThId);
          ++nSend;
          return ppe;
        }
        default: {
          printf("Error - unknown MsgId\n");
          return NULL;
        }
      }
    }
  }
  if(ll++ < DIMCID/2) {
    sTm = tNow();
    DBG("CTstSgThX::getEvent %u ll-%d Evnt_DelCtxt CId=%u\n",sigThId,ll,CId[ll]);
    ppe = CEvent::newEv(CId[ll],Evnt_SRemCtxt,0u,sigThId);
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

// $Id: tstSigThrd.cpp 388 2010-05-15 21:27:40Z asus $
