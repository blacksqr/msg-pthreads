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
#include <EvObj.h>
#include <Global.h>
#include <Thread.h>
#include <getTime.h>
#include <dprint.h>

// event pool
static CMPool CEvent_mem(sizeof(CEvent));

pCEvent CEvent::newEv(uInt i,short e,uInt d,uChar ti,char x) {
  return new (CEvent_mem.pAlloc()) CEvent(i, e, d, ti, x);
}

pCEvent CEvent::newEv(uInt i,short e,pVoid p,uChar ti,char x) {
  return new (CEvent_mem.pAlloc()) CEvent(i, e, p, ti, x);
}

CEvent::CEvent(uInt i,short e,uInt d,uChar tId,char x): cId(i),Event(e),sgThId(tId),nCtxt(x) {
  pData  = (pVoid)d;
  DBG("CEvent::CEvent#> %u %d %u %u %d\n",cId,Event,(uInt)pData,sgThId,nCtxt);
}

CEvent::CEvent(uInt i,short e,pVoid p,uChar tId,char x): cId(i),Event(e),sgThId(tId),nCtxt(x) {
  pData  = p;
  DBG("CEvent::CEvent$> %u %d 0x%X %u %d\n",cId,Event,(uInt)pData,sgThId,nCtxt);
}

void CEvent::delEv(pCEvent pt) {
  pt->~CEvent();
  CEvent_mem.pFree(pt);
}

// ===============================================

CFifo::CFifo() {
  beg = end = 0x00;
  gr1 = FfQueSize/3;
  gr2 = FfQueSize/2;
  gFlg = 'x';
  DBG("CFifo::CFifo> QSize=%d\n",FfQueSize);
}

CFifo::~CFifo() {
  LOG(L_WARN,"CFifo::~CFifo %d> %d <%d clean\n", beg,nFf,end);
  while(beg != end)
    CEvent::delEv(getEl()); // Clean memory
  rCnd.unlock();
  wMut.unlock();
}

// ID of House-keeping context
extern uInt HkCId;

_FFInd CFifo::addEl(pCEvent p) {
  DBG("CFifo::addEl> 0x%X<>0x%X\n",beg,end);
  while(nFf > (FfQueSize - 0x10))
    yield();  // Queue fast full - yield
  if((nFf > gr2) && gFlg) {
    pCEvent pw = CEvent::newEv(HkCId,Evnt_Que12Full);
    // Send warning msg Queue 50% full
    LOG(L_WARN,"CFifo::addEl> 50%%-Full beg=%d end=%d\n",beg,end);
    gFlg = '\0';
    {
      CSglLock slr(rCnd);
      pObj[--beg] = pw;
    }
  }
  {
    CSglLock slw(wMut);
    pObj[end++] = p;
    return end;
  }
}

_FFInd CFifo::addTopEl(pCEvent p) {
  CSglLock slr(rCnd);
  pObj[--beg] = p;
  return beg;
}

// rCnd - must be locked befor getEl called
pCEvent CFifo::getEl() {
  while(end == beg)
    rCnd.wait();
  if(!gFlg && (nFf < gr1))
    gFlg = 'x';
  return pObj[beg++];
}
