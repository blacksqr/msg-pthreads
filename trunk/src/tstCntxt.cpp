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
#include <workTcl.h>
#include <WThread.h>
#include <getTime.h>

// event pool
static CMPool CTCtxt_mem(sizeof(CTstCntx),0x80);

CTstCntx* CTstCntx::newTCtxt(uInt i) {
  return new (CTCtxt_mem.pAlloc()) CTstCntx(i);
}

void CTstCntx::delTCtxt(CTstCntx* pt) {
  pt->~CTstCntx();
  CTCtxt_mem.pFree(pt);
}

// ======================================================

extern int RRand();

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
  tstCtxtMgr.Free(cId);
}

uShort CTstCntx::Run(CEvent* pe,CWThread* pwt) {
  DBG("CTstCntx::Run cId=%u EventId=%u %u<>%u\n",cId,pe->getEv(),nm,nMsg);
  if(nm == nMsg) {
    DBG("CTstCntx::Run save_destruct cId=%u\n",cId);
    send0(Evnt_SaveData,iCsCtxt);
    destruct();
    nMsg = 0u;
    return 0x1;
  }
  return nm++ ? 1u : 0x0;
}

uShort CTstCntx::onTimer(uLong tn,CEvent* pe,CWThread* pwt) {
  DBG("CTstCntx::onTimer %u> EventId=%u\n",cId,pe->getEv());
  return 0x0;
}

char CTstCntx::onHalt() {
  DBG("CTstCntx::onHalt %u\n",cId);
  return '\0';
}
