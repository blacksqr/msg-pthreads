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
#ifndef ITS_ADMIN_H
#define ITS_ADMIN_H

#include "tclAdapter.h"

extern "C" {
  int Sqlite3_Init(Tcl_Interp*);
  int Md5_Init(Tcl_Interp*);
#ifdef TCL_THREADS
  int TclThread_Init(Tcl_Interp*);
#endif
} // extern "C"

// ====================================

// new TCL commant to start SIGNAL thread
class CSigThrd: public CTclCmd {
 private:
  // disable copy constructor.
  CSigThrd(const CSigThrd&);
  void operator = (const CSigThrd&);
 public:
  CSigThrd(CTclInterp* pi): CTclCmd(pi,"sThrd") {}
  ~CSigThrd() {}
  virtual int cmdProc(int argc,Tcl_Obj* const argv[]);
};

// new TCL commant to start WORK thread
class CWrkThrd: public CTclCmd {
 private:
  // disable copy constructor.
  CWrkThrd(const CWrkThrd&);
  void operator = (const CWrkThrd&);
 public:
  CWrkThrd(CTclInterp* pi): CTclCmd(pi,"wThrd") {}
  ~CWrkThrd() {}
  virtual int cmdProc(int argc,Tcl_Obj* const argv[]);
};

// new TCL commant for coomon Apps taks
class CComApp: public CTclCmd {
 private:
  // disable copy constructor.
  CComApp(const CComApp&);
  void operator = (const CComApp&);
 public:
  CComApp(CTclInterp* pi): CTclCmd(pi,"apps") {}
  ~CComApp() {}
  virtual int cmdProc(int argc,Tcl_Obj* const argv[]);
};

// ====================================

class CAdmTcl: public CTclInterp {
 private:
  // disable copy constructor.
  CAdmTcl(const CAdmTcl&);
  void operator = (const CAdmTcl&);
 public:
  CAdmTcl() {}
  ~CAdmTcl() {}
  char Init();  // init custom commands
};

#endif // ITS_ADMIN_H
// $Log$
