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

#include <fsmCtxt.h>
#include <WThread.h>

// ======================================================

// event pool
static CMPool CFsm_mem(sizeof(CFsmCtxt), 0x20);

CContext* CFsmCtxt::newFsm(uInt i) {
  CFsmCtxt* p = new (CFsm_mem.pAlloc()) CFsmCtxt(i);
  p->hashCId();
  return p;
}

void CFsmCtxt::delCtxt() {
  CFsmCtxt* pt = this;
  pt->~CFsmCtxt();
  CFsm_mem.pFree(pt);
}

// ======================================================

// Registration of state methods in state array
CFsmCtxt::CFsmCtxt(uInt i): CContext(i) {
  inLua = state = 0;
  pSFunc[0] = &CFsmCtxt::onIdle;
  pSFunc[1] = &CFsmCtxt::onRing;
  pSFunc[2] = &CFsmCtxt::onProcid;
  pSFunc[3] = &CFsmCtxt::onConn;
  pSFunc[4] = &CFsmCtxt::onDiscon;
}

char CFsmCtxt::onHalt() {
  DBG("CFsmCtxt::onHalt %u\n",cId);
  return '\0';
}

uShort CFsmCtxt::Run(CEvent* pe,CWThread* pwt) {
  delTimer(TOut_FsmSwitch);
  state = (this->*pSFunc[(short)state])(pe,pwt,'e');
  if(state < 0)
    onEvError(pe,'e',state);  // Error handling
  setTimer(1777,TOut_FsmSwitch);
  return (uShort)inLua;
}

uShort CFsmCtxt::onTimer(uLong tn,CEvent* pe,CWThread* pwt) {
  if(isTimerOn(TOut_FsmSwitch)) {
    state = (this->*pSFunc[(short)state])(pe,pwt,'t');
    if(state < 0)
      onEvError(pe,'t',state);  // Error handling
    setTimer(1777,TOut_FsmSwitch);
  }
  return (uShort)inLua;
}


char CFsmCtxt::onIdle(CEvent* pe,CWThread* pwt,char flg) {
  DBG("CFsmCtxt::onIdle %u -%c- 0x%X\n",cId,flg,pe->getEv());
  return '\1';
}

char CFsmCtxt::onRing(CEvent* pe,CWThread* pwt,char flg) {
  DBG("CFsmCtxt::onRing %u -%c- 0x%X\n",cId,flg,pe->getEv());
  return '\2';
}

char CFsmCtxt::onProcid(CEvent* pe,CWThread* pwt,char flg) {
  DBG("CFsmCtxt::onProcid %u -%c- 0x%X\n",cId,flg,pe->getEv());
  return '\3';
}

char CFsmCtxt::onConn(CEvent* pe,CWThread* pwt,char flg) {
  DBG("CFsmCtxt::onConn %u -%c- 0x%X\n",cId,flg,pe->getEv());
  return '\4';
}

char CFsmCtxt::onDiscon(CEvent* pe,CWThread* pwt,char flg) {
  DBG("CFsmCtxt::onDiscon %u -%c- 0x%X\n",cId,flg,pe->getEv());
  return '\0';
}

void  CFsmCtxt::onEvError(CEvent* pe,char flg, char state) {
  DBG("CFsmCtxt::onEvError %u -%d-%d-\n",cId,flg,state);
}

// $Id: tstCntxt.cpp 323 2010-01-17 20:29:38Z asus $
