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
