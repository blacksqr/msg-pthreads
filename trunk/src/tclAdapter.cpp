// $Id$
#include <dprint.h>
#include <tclAdapter.h>

///////////////////////////////
// TCL interface
///////////////////////////////
extern "C" {
  // functions to make / clean TclObjCmd
  int TclObjCmd(ClientData cData,Tcl_Interp*,int argc,Tcl_Obj* const argv[]) {
    return ((CTclCmd*)cData)->cmdProc(argc, argv);
  }
  void CleanTclCmd(ClientData cmdObj) {
    LOG(L_INFO,"Clean-Tcl-Cmd <%s>\n",((CTclCmd*)cmdObj)->getCmd());
    delete ((CTclCmd*)cmdObj);
  }
} // extern "C"

// ======================================================

CTclInterp::CTclInterp(Tcl_Interp* p) {
  pInterp = p ? p : Tcl_CreateInterp();
  d = p ? 0x0 : 'x';
  LOG(L_INFO,"CTclInterp - Tcl_CreateInterp <%d>\n",d);
}

CTclInterp::~CTclInterp() {
  LOG(L_INFO,"~CTclInterp - Tcl_DeleteInterp <%d>\n",d);
  if(d && pInterp)
    (void)Tcl_DeleteInterp(pInterp);
}

int CTclInterp::Eval(const char *script, int len, int flg) {
  Tcl_Obj *pScript = Tcl_NewStringObj(script, len);
  return Tcl_EvalObjEx(pInterp, pScript, flg);
}

// ======================================================

CTclCmd::CTclCmd(CTclInterp* i, pcChar c, char x):
  interp(i), tclCmd(c), flg(x)
{
  DBG("Creat CTclCmd CreateObjCommand <%s>\n",tclCmd);
  (void)Tcl_CreateObjCommand(i->getInterp(),(char*)tclCmd,TclObjCmd,(ClientData)this,CleanTclCmd);
}

CTclCmd::~CTclCmd() {
  DBG("~CTclCmd command <%s> Flg=%u\n",tclCmd,flg);
  if(flg && tclCmd)
    (void)free((char*)tclCmd);
}

#ifndef NO_DEBUG
// Debug output of command's params
void CTclCmd::prParam(int argc, Tcl_Obj* const argv[]) {
  char *pc, buff[360];
  pc = buff;
  (void)sprintf(buff, "%d ", argc);
  for(short k=0; k<argc; ++k) {
    pc = strchr(pc, 0);
    (void)sprintf(pc, "%s ", interp->tGetVal(argv[k]));
  }
  pc = strchr(pc, 0);
  (void)sprintf(pc, "\n");
  DBG("$$Tcl> argc=%s", buff);
}
#endif // NO_DEBUG

// $Log$
