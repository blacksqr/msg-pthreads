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
#ifndef WORK_TCL_H
#define WORK_TCL_H

#include "tclAdapter.h"

class CGetEvnt;
class CCtxtProc;
class CWThread;

// ===============================================

class CWrkTcl: public CTclInterp {
  friend class CGetEvnt;
  friend class CCtxtProc;
  //========================
  friend class CTstData;
 private:
  // Disable copy constructor
  CWrkTcl(const CWrkTcl&);
  void operator = (const CWrkTcl&);
 protected:
  CWThread* pth;
 public:
  CWrkTcl() {}
  ~CWrkTcl() {}
  CWThread* getTh() { return pth; }
  // init custom commands
  char Init(CWThread* p);
  int cmdProc(int argc,Tcl_Obj* const argv[]);
};

// ===============================================

class CGetEvnt: public CTclCmd {
 public:
  CGetEvnt(CTclInterp* pi): CTclCmd(pi,"getev") {}
  ~CGetEvnt() {}
  virtual int cmdProc(int argc,Tcl_Obj* const argv[]);
};

// call current Context->cmdProc
class CCtxtProc: public CTclCmd {
 public:
  CCtxtProc(CTclInterp* pi): CTclCmd(pi,"aCtxt") {}
  ~CCtxtProc() {}
  virtual int cmdProc(int argc,Tcl_Obj* const argv[]) {
    DBG("CCtxtProc::cmdProc argc - %d\n",argc);
    return ((CWrkTcl*)interp)->cmdProc(argc,argv);
  }
};

#endif // WORK_TCL_H
