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
#include <dbCtxt.h>

extern "C" {
  int Md5_Init(Tcl_Interp*);
  int Sqlite3_Init(Tcl_Interp* interp);
  int Db_tcl_Init(Tcl_Interp* interp);
#ifdef TCL_THREADS
  int TclThread_Init(Tcl_Interp*);
#endif
} // extern "C"

// ===================================================

int CGetEvnt::cmdProc(int argc,Tcl_Obj* const argv[]) {
  CWThread* pwth = ((CWrkTcl*)interp)->pth;
  tSetResult(tSetObj(pwth->getEvent()));
  pContext pc = pwth->getCtxt();
  (void)interp->setVar("ctxtType", tSetObj(pc ? pc->getCtxtType() : '\0'));
  DBG("CGetEvnt::cmdProc getev ctxtType-%u\n",pc ? pc->getCtxtType() : '\0');
  return TCL_OK;
}

// ===================================================

int CWrkTcl::cmdProc(int argc,Tcl_Obj* const argv[]) {
  DBG("CWrkTcl::cmdProc argc - %d\n",argc);
  return pth->cmdProc(this,argc,argv);
}

// ===================================================

char CWrkTcl::Init(CWThread* p) {
  pth = p;
  // Add Sqlite commands
  (void)Sqlite3_Init(getInterp());
  // Add Berkly-db commands
  (void)Db_tcl_Init(getInterp());
  // Add new TCL commands
  new CGetEvnt(this);
  new CCtxtProc(this);
  // Set wrkThreadId in interpreter - wThId
  (void)setVar("wThId",tSetObj((short)(p->getWTId())));
  return 0x0;
}
