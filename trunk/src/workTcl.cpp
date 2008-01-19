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
#include <Timer.h>
#include <WThread.h>
#include <workTcl.h>
#include <dbCntxt.h>

int CGetEvnt::cmdProc(int argc,Tcl_Obj* const argv[]) {
  CWrkTcl* pTcl = (CWrkTcl*)interp;
  int res = pTcl->pth->getEvent();
  //DBG(">CGetEvnt::cmdProc> tclCmd-getev event res=%d\n",res);
  tSetResult(tSetObj(res));
  return TCL_OK;
}

// ===================================================

int CGetCtxtTp::cmdProc(int argc,Tcl_Obj* const argv[]) {
  CContext* pc = wrkGetCtxt();
  short res = pc->getCtxtType();
  //DBG(">CGetCtxtTp::cmdProc> CntxtType=%d\n",res);
  tSetResult(tSetObj(res));
  return TCL_OK;
}

// ===================================================

int CAppState::cmdProc(int argc,Tcl_Obj* const argv[]) {
  CHKContext* pc = (CHKContext*)wrkGetCtxt();
  tSetResult(tSetObj(pc->getStatus()));
  return TCL_OK;
}

// ===================================================

int CSendMsg::cmdProc(int argc,Tcl_Obj* const argv[]) {
  pParam(argc, argv);
  if(argc == 4) {
    TGVal(short,ev,argv[1]);
    TGVal(int,id,argv[2]);
    TGVal(int,pp,argv[3]);
    CContext* pc = wrkGetCtxt();
    if(id >= 0) {
      pc->send(ev, id ? id : pc->getId(), (pVoid)pp);
    } else
      pc->send0(ev, -id, (pVoid)pp);
  }
  return TCL_OK;
}

// ===================================================

extern uInt HkCId;
int CSetTimer::cmdProc(int argc,Tcl_Obj* const argv[]) {
  pParam(argc, argv);
  if(argc == 5) {
    TGVal(int,cid,argv[1]);
    TGVal(int,tm,argv[2]);
    TGVal(short,type,argv[3]);
    TGVal(int,f,argv[4]);
    if(!cid) {
      CContext* pc = wrkGetCtxt();
      pc->setTimer((uLong)tm,(uChar)type,(uShort)f);
    } else // hauseKeep timer
      tmQueue.set(tm,type,HkCId,f);
  }
  return TCL_OK;
}

// ===================================================

int CTstData::cmdProc(int argc,Tcl_Obj* const argv[]) {
  CDbCntx* pc = (CDbCntx*)wrkGetCtxt();
  pc->InsData(((CWrkTcl*)interp)->getTh());
  return TCL_OK;
}

// ===================================================

CContext* CWrkTcl::getCtxt() { return pth->pCont; }

char CWrkTcl::Init(CWThread* p) {
  pth = p;
  // Add Sqlite commands
  (void)Sqlite3_Init(getInterp());
  // Add new TCL commands
  new CGetEvnt(this);
  new CGetCtxtTp(this);
  new CAppState(this);
  new CSendMsg(this);
  new CSetTimer(this);
  // ===============================================
  new CTstData(this);
  // ===============================================
  // Set wrkThreadId in interpreter - wThId
  (void)setVar("wThId",tSetObj((short)(p->getWTId())));
  return 0x0;
}
