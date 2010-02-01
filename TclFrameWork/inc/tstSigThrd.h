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

class CTstCtxtMgr {
#define ctxtPoolSz 0xFFFF
private:
  CPMutex m;
  uInt cPool[ctxtPoolSz];
  uChar cPoolFlg[1 + ctxtPoolSz/8];
  CUIntShrt hsh;
  uShort Nn,pFree;
  // disable copy constructor.
  CTstCtxtMgr(const CTstCtxtMgr&);
  void operator = (const CTstCtxtMgr&);
public:
  CTstCtxtMgr();
  ~CTstCtxtMgr() {}
  char isFree() { return (Nn < (ctxtPoolSz-16)) ? 'x' : '\0'; }
  uInt Alloc(uInt ci);
  uInt get(uInt rnd);
  void Free(uInt ci);
};

extern CTstCtxtMgr tstCtxtMgr;

//=================================================

class CTstSgTh: public CEvThrd {
  friend class CSigThrd;
 protected:
  uLong  sTm;   // Start time point
  uInt   nSend,DtTm,nMsg;
  // nMsg0 time periode tm0 in 0.1 sec & nMsg1 time periode tm1 in 0.1 sec
  const uShort nMsg0,nMsg1;   // nMsg1 >= nMsg0
  const uChar tm0, tm1;
  virtual CEvent* getEvent();
  // Protected constructor
  CTstSgTh(uShort m0,uShort m1,uChar t0,uChar t1);
 public:
  virtual ~CTstSgTh() {}
};

// send nMsg pro sec and makes pause to produce ALARM
class CTstSgThX: public CEvThrd {
  friend class CSigThrd;
 protected:
  uLong  sTm;     // Start time point
  uInt   CId[11];  // test Ctxt-id
  uShort nSend,DtTm,nMsg;
  short ff, kk, jj, ll;
  // nMsg0 time periode tm0 in 0.1 sec & nMsg1 time periode tm1 in 0.1 sec
  const uShort nMsg0,nMsg1;   // nMsg1 >= nMsg0
  const uChar tm0, tm1;
  virtual CEvent* getEvent();
  // Protected constructor
  CTstSgThX(uShort m0,uShort m1);
 public:
  virtual ~CTstSgThX() {}
};

#endif // TST_SIG_TH_H
// $Log$
