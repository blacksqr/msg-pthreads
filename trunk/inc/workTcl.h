#ifndef WORK_TCL_H
#define WORK_TCL_H

#include "tclAdapter.h"

extern "C" {
  int Sqlite3_Init(Tcl_Interp*);
  int Md5_Init(Tcl_Interp*);
#ifdef TCL_THREADS
  int TclThread_Init(Tcl_Interp*);
#endif
} // extern "C"

class CContext;
class CWThread;

class CGetEvnt: public CTclCmd {
 public:
  CGetEvnt(CTclInterp* pi): CTclCmd(pi,"getev") {}
  ~CGetEvnt() {}
  virtual int cmdProc(int argc,Tcl_Obj* const argv[]);
};

// macro for all wrk-Tcl Context base command
#define wrkGetCtxt() (((CWrkTcl*)interp)->getCtxt())

// Get context type for Tcl-switch statment
class CGetCtxtTp: public CTclCmd {
 public:
  CGetCtxtTp(CTclInterp* pi): CTclCmd(pi,"ctxtTp") {}
  ~CGetCtxtTp() {}
  virtual int cmdProc(int argc,Tcl_Obj* const argv[]);
};

// HauseKeep context - get application state
class CAppState: public CTclCmd {
 public:
  CAppState(CTclInterp* pi): CTclCmd(pi,"appSt") {}
  ~CAppState() {}
  virtual int cmdProc(int argc,Tcl_Obj* const argv[]);
};

class CSendMsg: public CTclCmd {
 public:
  CSendMsg(CTclInterp* pi): CTclCmd(pi,"sendEv") {}
  ~CSendMsg() {}
  virtual int cmdProc(int argc,Tcl_Obj* const argv[]);
};

class CSetTimer: public CTclCmd {
 public:
  CSetTimer(CTclInterp* pi): CTclCmd(pi,"sTimer") {}
  ~CSetTimer() {}
  virtual int cmdProc(int argc,Tcl_Obj* const argv[]);
};

// ===============================================

// Gen test data
class CTstData: public CTclCmd {
 public:
  CTstData(CTclInterp* pi): CTclCmd(pi,"setTstData") {}
  ~CTstData() {}
  virtual int cmdProc(int argc,Tcl_Obj* const argv[]);
};

// ===============================================

class CWrkTcl: public CTclInterp {
  friend class CGetEvnt;
  friend class CGetCtxtTp;
  friend class CAppState;
  friend class CSendMsg;
  friend class CSetTimer;
  //========================
  friend class CTstData;
 private:
  // Disable copy constructor
  CWrkTcl(const CWrkTcl&);
 protected:
  CWThread* pth;
 public:
  CWrkTcl() {}
  ~CWrkTcl() {}
  CContext* getCtxt();
  CWThread* getTh() { return pth; }
  // init custom commands
  char Init(CWThread* p);
};

#endif // WORK_TCL_H
