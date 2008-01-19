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
#include <workTcl.h>
#include <WThread.h>

const char rr = 10;

CTstCntx::CTstCntx(uInt i): CContext(i),rx(0x0) {
  ctxtType = C_CTXT_TST;
  inTcl = cId % rr;
}

uShort CTstCntx::Run(CEvent* pe,CWThread* pwt) {
  rx = (rx > rr) ? '\0' : rx;
  DBG("CTstCntx::Run cId=%u> EventId=%u %d<>%d\n",cId,pe->getEv(),rx,inTcl);
  if(pe->getEv() == Evnt_DelCtxt) {
    DBG("CTstCntx::Run destruct %u> EventId=%u\n",cId,pe->getEv());
    destruct();
    return 0; // get new event
  }
  if(inTcl == rx++) {
    send0(Evnt_SaveData,iCsCtxt);
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
