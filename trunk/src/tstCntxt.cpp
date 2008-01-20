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

CTstCntx::CTstCntx(uInt i): CContext(i) {
  ctxtType = C_CTXT_TST;
  nMsg = 3u + (random() % 7);
  nm = 0u;
  DBG("CTstCntx::CTstCntx Cid=%u mMsg=%u\n",cId,nMsg);
}

CTstCntx::~CTstCntx() {
  DBG("CTstCntx::~CTstCntx cId=%u Flg=%u/%u\n",cId,nm,nMsg);
  if(nMsg)
    send0(Evnt_SaveData,iCsCtxt);
  tstCtxtMgr.Free(cId);
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
