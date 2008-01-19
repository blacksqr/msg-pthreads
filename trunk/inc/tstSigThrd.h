// $Id$
#ifndef TST_SIG_TH_H
#define TST_SIG_TH_H

#include <EvThread.h>

// send nMsg pro sec (check point 1/25 sec)
class CTstSgTh0: public CEvThrd {
 protected:
  uLong  sTm;     // Start time point
  uInt   CId[1024];  // test Ctxt-id
  const uShort nMsg;
  uShort nSend;
  short ff, kk;
  virtual CEvent* getEvent();
 public:
  CTstSgTh0(uShort n): CEvThrd(5000/n),nMsg(2*n) {
    tName = "tstSgThrd_0";
    nSend = 0u;
    ff = kk = 0x0;
  }
  virtual ~CTstSgTh0() {}
};

class CTstSgTh1: public CEvThrd {
 protected:
  uLong  sTm;     // Start time point
  uInt   CId[224];  // test Ctxt-id
  uShort nSend,DtTm,nMsg;
  short ff, kk, jj, ll;
  // nMsg0 time periode tm0 in 0.1 sec & nMsg1 time periode tm1 in 0.1 sec
  const uShort nMsg0,nMsg1;   // nMsg1 >= nMsg0
  const uChar tm0, tm1;
  virtual CEvent* getEvent();
 public:
  CTstSgTh1(uShort m0,uShort m1,uChar t0,uChar t1);
  virtual ~CTstSgTh1() {}
};

// send nMsg pro sec and makes pause to produce ALARM
class CTstSgTh2: public CEvThrd {
 protected:
  uLong  sTm;     // Start time point
  uInt   CId[11];  // test Ctxt-id
  uShort nSend,DtTm,nMsg;
  short ff, kk, jj, ll;
  // nMsg0 time periode tm0 in 0.1 sec & nMsg1 time periode tm1 in 0.1 sec
  const uShort nMsg0,nMsg1;   // nMsg1 >= nMsg0
  const uChar tm0, tm1;
  virtual CEvent* getEvent();
 public:
  CTstSgTh2(uShort m0,uShort m1);
  virtual ~CTstSgTh2() {}
};

#endif // TST_SIG_TH_H
// $Log$
