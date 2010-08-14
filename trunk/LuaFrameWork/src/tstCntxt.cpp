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
#include <tstCntxt.h>
#include <tstSigThrd.h>
#include <WThread.h>
#include <getTime.h>

// event pool
static CMPool CTCtxt_mem(sizeof(CTstCntx), 0x80);

// ======================================================

CTstCntx* CTstCntx::newTCtxt(uInt i) {
  CTstCntx* p = new (CTCtxt_mem.pAlloc()) CTstCntx(i);
  p->hashCId();
  return p;
}

void CTstCntx::delCtxt() {
  CTstCntx* pt = this;
  pt->~CTstCntx();
  CTCtxt_mem.pFree(pt);
}

// ======================================================

extern uInt RRand();

CTstCntx::CTstCntx(uInt i): CContext(i) {
  ctxtType = C_CTXT_TST;
  cStart = tNow();
  nMsg = 3u + (RRand() % 7);
  nm = 0u;
  DBG("CTstCntx::CTstCntx Cid=%u mMsg=%u\n",cId,nMsg);
}

CTstCntx::~CTstCntx() {
  LOG(L_WARN,"CTstCntx::~CTstCntx cId=%u Flg=%u/%u Live=%u\n",
      cId,nm,nMsg,(uInt)(tNow()-cStart));
  if(nMsg)
    send0(Evnt_SaveData,iCsCtxt);
  tstCtxtMngr.Free(cId);
}

uShort CTstCntx::Run(CEvent* pe,CWThread* pwt) {
  ++nm;
  DBG("CTstCntx::Run cId=%u EventId=%u %u<>%u\n",cId,pe->getEv(),nm,nMsg);
  if(nm == nMsg) {
    DBG("CTstCntx::Run save_destruct cId=%u\n",cId);
    send0(Evnt_SaveData,iCsCtxt);
    destruct();
    nMsg = 0u;
    return 0x1;
  }
  return 0x0;
}

uShort CTstCntx::onTimer(uLong tn,CEvent* pe,CWThread* pwt) {
  DBG("CTstCntx::onTimer %u> EventId=%u\n",cId,pe->getEv());
  return 0x0;
}

char CTstCntx::onHalt() {
  DBG("CTstCntx::onHalt %u\n",cId);
  return '\0';
}

// Test context for MsgParser
// ======================================================

CSipCntx* CSipCntx::newSCtxt(uInt i) {
  CSipCntx* p = new (CTCtxt_mem.pAlloc()) CSipCntx(i);
  p->hashCId();
  return p;
}

void CSipCntx::delCtxt() {
  CSipCntx* pt = this;
  pt->~CSipCntx();
  CTCtxt_mem.pFree(pt);
}

// ======================================================

CSipCntx::CSipCntx(uInt i): CContext(i) {
  ctxtType = C_SIP_TST;
  cStart = tNow();
  nMsg = 3u + (RRand() % 7);
  nm = 0u;
  DBG("CSipCntx::CSipCntx Cid=%u mMsg=%u\n",cId,nMsg);
}

CSipCntx::~CSipCntx() {
  LOG(L_WARN,"CSipCntx::~CSipCntx cId=%u Flg=%u/%u Live=%u\n",
      cId,nm,nMsg,(uInt)(tNow()-cStart));
  if(nMsg)
    send0(Evnt_SaveData,iCsCtxt);
  tstCtxtMngr.Free(cId);
}

uShort CSipCntx::Run(CEvent* pe,CWThread* pwt) {
  ++nm;
  DBG("CSipCntx::Run cId=%u EventId=%u %u<>%u\n",cId,pe->getEv(),nm,nMsg);

  switch(pe->getEv()) {
    case Evnt_sipCtxt1: {
      CMsgGen_mymsg1* pMm1 = (CMsgGen_mymsg1*)pe->Data();
      char rr = pMm1->parse1();
      DBG("CSipCntx::Run parse MSG1 ret =%d\n", rr);
      delete pMm1;
      break;
    }
    case Evnt_sipCtxt2: {
      CMsgGen_mymsg2* pMm2 = (CMsgGen_mymsg2*)pe->Data();
      char rr = pMm2->parse1();
      DBG("CSipCntx::Run parse MSG2 ret =%d\n", rr);
      delete pMm2;
      break;
    }
    default:
      LOG(L_ERR, "Error - unknown MsgId\n");
  }

  if(nm == nMsg) {
    DBG("CSipCntx::Run save_destruct cId=%u\n",cId);
    send0(Evnt_SaveData,iCsCtxt);
    destruct();
    nMsg = 0u;
    return 0x1;
  }
  return 0x0;
}

uShort CSipCntx::onTimer(uLong tn,CEvent* pe,CWThread* pwt) {
  DBG("CSipCntx::onTimer %u> EventId=%u\n",cId,pe->getEv());
  return 0x0;
}

char CSipCntx::onHalt() {
  DBG("CSipCntx::onHalt %u\n",cId);
  return '\0';
}

// $Id: tstCntxt.cpp 386 2010-05-15 16:05:24Z asus $
