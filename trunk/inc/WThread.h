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
#ifndef WTHREAD_H
#define WTHREAD_H

#include <sqlWrap.h>
#include <workTcl.h>
#include <Context.h>
//#include <udpSock.h>

typedef struct timeval STmVal;

class CContext;

// to check state of working threads & sys.optimisation
#define TH_INST  0x0001  // Obj instance created
#define TH_RUN   0x0002  // Thread run
#define PROC_EV  0x0004  // Thread proc. event
#define PROC_TM  0x0008  // Thread proc. timer event
// ..................

// working thread - reads msg's from fifo
//class CWThread: public CDbThread {
class CWThread: public CThreadObj {
  friend class CWrkTcl;
  // disable copy constructor.
  CWThread(const CWThread&);
  void operator = (const CWThread&);
 protected:
  static uChar nRun;         // global number of running threads
  static char  g_WStatStart; // run time statistik On/Off
  static uChar nReload;
  // Common for all wrkThreads TCL-Init
  static char  eFlag;        // flag to exit w-threads
  uInt         nMsgTot;
  STmVal       tmVal;  // For statistic
  CEvent*      pe;
  pContext     pCont;
  CWrkTcl      Tcl;
  //CUdpClntSock pSock;
  uShort nWait;     // Num of wait for (seq == pCont->seq1)
  uChar  wtId, seq; // event sequence
  char   WStstRun;
  // called from TCL command - getEvent
  int Run(pCEvent p) { return pCont->Run(p,this); }
  uInt tmDelta();
 public:
  CWThread(uChar id);
  virtual ~CWThread();
  int cmdProc(CWrkTcl* ptcl,int argc,Tcl_Obj* const argv[]);
  pContext getCtxt() { return pCont; }
  static void wStop()  { eFlag = 'x'; }
  static void wStart() { eFlag = '\0'; }
  virtual void* go();
  uShort getEvent();
  CWrkTcl& getTcl()  { return Tcl; }
  uChar    getWTId() { return wtId; }
  // Factory to produce new context & add it to hash
  static CContext* genNewCtx(CEvent* pe);
  static char nRThrd() { return nRun; }
  static void reloadTcl() { nReload = 0x0; }
  static void gWrkStat(char x) { g_WStatStart = x; }

  // Statistic
 private:
  // To diagnose blocked work thread
  uInt cntxtId;
  uInt nMsg,msgTm;  // Last msg Proc-Start-Time
  uInt T[8];
#define Tx0 T[0]    // Tx0 - End of last msg process
#define Tx1 T[1]    // Tx1 - read new event from queue
#define Tx2 T[2]    // Tx2 - Cntxt lock acquired
#define Tx3 T[3]    // Tx3 - Cntxt unlock (next Tx0 = Tx3) 
#define Tt0 T[4]    // Tt0 - time of End of last msg process
#define Tt1 T[5]    // Tt1 - time of read new event from queue
#define Tt2 T[6]    // Tt2 - time of Cntxt lock acquired
#define Tt3 T[7]    // Tt3 - time of Cntxt unlock (next Tx0 = Tx3) 
  uShort stat;      // Last state of work thread
 public:
  uInt gNMsg()    { return nMsg; }
  uInt gCtxtId()  { return cntxtId; }
  uInt gMTime()   { return msgTm; }
  int  getNMsg()  { return nMsg; }
  uShort gMId()   { return pe ? pe->getEv() : 0u; }
  uShort gStat()  { return stat; }
  uShort gNWiat() { return nWait; }
  const uInt* getTimes() { return T; }
};
typedef CWThread* pWThread;
extern pWThread pWrkThArr[];

#endif // WTHREAD_H
