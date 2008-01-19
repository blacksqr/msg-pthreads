// $Id$
#ifndef ITS_TCL_H
#define ITS_TCL_H

#include <tcl.h>
#include <string.h>
#include <Global.h>
#include <dprint.h>

typedef Tcl_Obj* pTcl_Obj;
typedef pTcl_Obj* ppTcl_Obj;

extern "C" {
  int TclObjCmd(ClientData cData,Tcl_Interp*,int argc,Tcl_Obj* const argv[]);
  void CleanTclCmd(ClientData cmdObj); // memory cleaning
  char* itoa(lLong ph,char* b);
} // extern "C"

  // Set value interface
inline Tcl_Obj* tSetObj(char* x)  { return Tcl_NewStringObj(x,strlen(x)); }
inline Tcl_Obj* tSetObj(char x)   { return Tcl_NewIntObj((int)x); }
inline Tcl_Obj* tSetObj(short x)  { return Tcl_NewIntObj((int)x); }
inline Tcl_Obj* tSetObj(int x)    { return Tcl_NewIntObj(x); }
inline Tcl_Obj* tSetObj(uInt x)   { return Tcl_NewIntObj(x); }
inline Tcl_Obj* tSetObj(lLong x)  { return Tcl_NewWideIntObj(x); }
inline Tcl_Obj* tSetObj(double x) { return Tcl_NewDoubleObj(x); }
inline Tcl_Obj* tSetObj(int n,pTcl_Obj* x) { return Tcl_NewListObj(n,x); }
// Get value interface

class CTclCmd;

class CTclInterp {
  friend class CTclCmd;
 protected:
  Tcl_Interp* pInterp;
  char d;  // Flag Own Tcl_Interp or from Apps
  void addCmd(const char* cmd, Tcl_CmdProc* proc,ClientData data=NULL,Tcl_CmdDeleteProc dProc=NULL)
    { Tcl_CreateCommand(pInterp, cmd, proc, data, dProc); }
  void addObjCmd(char *cmd, Tcl_ObjCmdProc *proc,ClientData data=NULL,Tcl_CmdDeleteProc dProc=NULL)
    { Tcl_CreateObjCommand(pInterp, cmd, proc, data, dProc); }
  int rmCmd(char* cmd) { return Tcl_DeleteCommand(pInterp, cmd); }
  const char* Result() { return Tcl_GetStringResult(pInterp); }
  void setResult(char* res,Tcl_FreeProc prs=TCL_VOLATILE)
                       { return Tcl_SetResult(pInterp,res,prs); }
  // Get value ot TCL-Var
  Tcl_Obj* getVar(const char* varName,const char* arrEl=NULL,int flag=TCL_GLOBAL_ONLY)
    { return Tcl_GetVar2Ex(pInterp,(char*)varName,(char*)arrEl,flag); }
  // disable copy constructor.
  CTclInterp(const CTclInterp&);
  void operator = (const CTclInterp&);
 public:
  CTclInterp(Tcl_Interp* p=NULL);
  virtual ~CTclInterp();
  Tcl_Interp* getInterp() { return pInterp; }
  int Eval(const char *script, int len, int flg=0);
  int EvalFile(const char *file) { return Tcl_EvalFile(pInterp, file); }
  int tAddLstEl(Tcl_Obj* lst,Tcl_Obj* o) { return Tcl_ListObjAppendElement(pInterp,lst,o); }
  // Set value of TCL-Var
  Tcl_Obj* setVar(const char* varName,Tcl_Obj* Value,const char* arrEl=NULL,int flag=TCL_GLOBAL_ONLY)
    { return Tcl_SetVar2Ex(pInterp,varName,arrEl,Value,flag); }
  // Get value interface
  char* tGetVal(Tcl_Obj* p)             { return Tcl_GetString(p); }
  int   tGetVal(Tcl_Obj* p,char& val) {
    int x, r = tGetVal(p, x);
    val = (char)x;
    return r;
  }
  int   tGetVal(Tcl_Obj* p,short& val) {
    int x, r = tGetVal(p, x);
    val = (short)x;
    return r;
  }
  int   tGetVal(Tcl_Obj* p,int& val)    { return Tcl_GetIntFromObj(pInterp,p,&val); }
  int   tGetVal(Tcl_Obj* p,uInt& val)   { return Tcl_GetIntFromObj(pInterp,p,(int*)&val); }
  int   tGetVal(Tcl_Obj* p,lLong& val)  { return Tcl_GetWideIntFromObj(pInterp,p,&val); }
  int   tGetVal(Tcl_Obj* p,double& val) { return Tcl_GetDoubleFromObj(pInterp,p,&val); }
  int   tGetVal(Tcl_Obj* list,int& n,ppTcl_Obj* pAr)
                                        { return Tcl_ListObjGetElements(pInterp,list,&n,pAr); }
};

////////////////////////////////////////////////////////////////
typedef const char* pcChar;

// base class for new commands
class CTclCmd {
 protected:
  CTclInterp* interp;
  pcChar tclCmd;
  // if(flg) - free(tclCmd)
  const char flg;
#ifndef NO_DEBUG
  // debug output of command's params
  void prParam(int argc, Tcl_Obj* const argv[]);
#endif // NO_DEBUG
  // disable copy constructor.
  CTclCmd(const CTclCmd&);
  void operator = (const CTclCmd&);
 public:
  CTclCmd(CTclInterp* i, pcChar c, char x='\0');
  virtual ~CTclCmd();
  void rmCmd() { Tcl_DeleteCommand(getTIntrpr(),(char*)tclCmd); }
  pcChar getCmd() { return tclCmd; }
  Tcl_Interp* getTIntrpr()   { return interp->getInterp(); }
  //CTclInterp* getCInterp() { return interp; }
#define TGVal(type,val,obj) type val; interp->tGetVal(obj,val)
  // interface to Tcl Result
  void tSetResult(Tcl_Obj* p) { Tcl_SetObjResult(interp->getInterp(),p); }
  // TCL command procedur
  virtual int cmdProc(int argc,Tcl_Obj* const argv[]) = 0;
};

// template function to put C-Array in Tcl_Obj(list)
template<class T> pTcl_Obj Arr2tlst(T a[],uChar len) {
  ppTcl_Obj oAr = (ppTcl_Obj)malloc(sizeof(pTcl_Obj[len]));
  for(uChar k=0; k<len; k++) { oAr[k] = tSetObj(a[k]); }
  return tSetObj(len,oAr);
}

// Debug macro to print args of a new TCL-Command
#ifndef NO_DEBUG
#define pParam(x,y) prParam(x,y)
#else  //NO_DEBUG
#define pParam(x,y)
#endif // NO_DEBUG


#endif // ITS_TCL_H
// $Log$
