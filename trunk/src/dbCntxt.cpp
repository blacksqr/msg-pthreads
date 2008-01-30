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
#include <dbCntxt.h>
#include <workTcl.h>
#include <WThread.h>
#include <getTime.h>
#include <cashCtxt.h>

//                  012345678901234567890123456789012345
const char* rStr = "asdfghjklqwertyuiopzxcvbnm0123456789";

int RRand() {
  static CPMutex tm;
  CSglLock sl(tm);
  return random();
}

CDbCntx::CDbCntx(uInt i): CContext(i) {
  ctxtType = C_CTXT_DB;
}

pTcl_Obj CDbCntx::InsData(CTclInterp* pTcl) {
  short strLen, k;
  char rString[0xFF];
  char* cAr[3] = {NULL,NULL,NULL};
  for(short n=0; n<3; ++n) {
    strLen = 10 + RRand()%8;
    for(k=0; k<strLen; ++k)
      rString[k] = rStr[RRand()%36];
    rString[k] = '\0';
    if(cAr[n]) free(cAr[n]);
    cAr[n] = strdup(rString);
  }
  pTcl_Obj pOLst = Arr2tlst(cAr,3);
  pTcl->tAddLstEl(pOLst,tSetObj(RRand()%99999));
  pTcl->tAddLstEl(pOLst,tSetObj(RRand()%99999));
  strLen = 23 + RRand()%128;
  for(k=0; k<strLen; ++k)
    rString[k] = rStr[RRand()%36];
  rString[k] = '\0';
  pTcl->tAddLstEl(pOLst,tSetObj(rString));
  return pOLst;
}

uShort CDbCntx::Run(CEvent* pe,CWThread* pwt) {
  DBG("CDbCntx::Run cId=%u> EventId=%u\n",cId,pe->getEv());
  if(pe->getEv() == Evnt_StoreInDb) {
    // Get data
    uShort nRec = 0u;
    CCsItem *pp, *pItmList = (CCsItem*)(pe->Data());
    while(pItmList) {
      ++nRec;
      pp = pItmList->next();
      CCsItem::delItem(pItmList);
      pItmList = pp;
    }
    DBG("CDbCntx::Run EventId=%u store nRec=%u\n",pe->getEv(),nRec);
    return nRec;
  }
  return 0x0;
}

uShort CDbCntx::onTimer(uLong tn,CEvent* pe,CWThread* pwt) {
  DBG("CDbCntx::onTimer %u> EventId=%u\n",cId,pe->getEv());
  return 0x0;
}

char CDbCntx::onHalt() {
  DBG("CDbCntx::onHalt %u\n",cId);
  return '\0';
}

int CDbCntx::cmdProc(CWrkTcl* interp, int argc, Tcl_Obj* const argv[]) {
  const char* sCmd = interp->tGetVal(argv[1]);
  DBG("CDbCntx::cmdProc sCmd - %s\n",sCmd);
  if(!strcmp(sCmd, "tstData")) {
    Tcl_SetObjResult(interp->getInterp(),InsData(interp));
    return TCL_OK;
  }
  return CContext::cmdProc(interp,argc,argv);
}
