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
#ifndef TST_SIG_TH_H
#define TST_SIG_TH_H

#include <EvThread.h>

typedef __gnu_cxx::hash_map < uInt,uShort,__gnu_cxx::hash<uInt>,eqUInt > CUIntShrt;

// Memory pool
class CTstCtxtMngr {
#define ctxtPoolSz 0xFFFF
private:
  CPMutex m;
  uInt cPool[ctxtPoolSz];
  uChar cPoolFlg[1 + ctxtPoolSz/8];
  CUIntShrt hsh;
  uShort Nn,pFree;
public:
  CTstCtxtMngr();
  ~CTstCtxtMngr() {}
  char isFree() { return (Nn < (ctxtPoolSz-16)) ? 'x' : '\0'; }
  uInt Alloc(uInt ci);
  uInt get(uInt rnd);
  void Free(uInt ci);
};

extern CTstCtxtMngr tstCtxtMngr;

//=================================================

class CTstSgTh: public CEvThrd {
 protected:
  uLong  sTm;   // Start time point
  uInt   nSend,DtTm,nMsg;
  // nMsg0 time periode tm0 in 0.1 sec & nMsg1 time periode tm1 in 0.1 sec
  const uShort nMsg0,nMsg1;   // nMsg1 >= nMsg0
  const uChar tm0, tm1;
  virtual CEvent* getEvent();
 public:
  CTstSgTh(uShort m0,uShort m1,uChar t0,uChar t1);
  virtual ~CTstSgTh() {}
};

#include <factory_myFctr1.h>
using namespace MsgModel;

#define DIMCID 0xFF

// send nMsg pro sec and makes pause to produce ALARM
class CTstSgThX: public CEvThrd {
 protected:
  static char* mxBuf;
  CFctryGen_myFctr1 mFactory;
  char*  nxMsg;    // Next msg in buffer
  uLong  sTm;      // Start time point
  // test Ctxt-id
  uInt   CId[DIMCID+1];
  uShort nSend,DtTm,nMsg,bfLen;
  short ff, kk, jj, ll;
  // nMsg0 time periode tm0 in 0.1 sec & nMsg1 time periode tm1 in 0.1 sec
  const uShort nMsg0,nMsg1;   // nMsg1 >= nMsg0
  const uChar tm0, tm1;
  virtual CEvent* getEvent();
 public:
  CTstSgThX(uShort m0,uShort m1);
  virtual ~CTstSgThX() {}
};

//=================================================
#define FSM_ARR_SIZE 2048

typedef struct {
  uChar state;
  uInt  CId;
  uInt  time;
} SFsmState;

// send nMsg to the set of FSM Context
class CTstFsmTh: public CEvThrd {
 protected:
  uInt nMsg;
  SFsmState fsmCtxtAr[FSM_ARR_SIZE];
  virtual CEvent* getEvent();
 public:
  CTstFsmTh(int m);
  virtual ~CTstFsmTh() {}
};

#endif // TST_SIG_TH_H

// $Id: tstSigThrd.h 385 2010-05-15 15:12:24Z asus $
