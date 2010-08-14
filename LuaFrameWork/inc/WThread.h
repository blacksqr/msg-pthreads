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

//#include <sqlWrap.h>
#include <Context.h>

typedef struct timeval STmVal;
class CContext;

// to check state of working threads & sys.optimisation
#define TH_INST  0x0001  // Obj instance created
#define TH_RUN   0x0002  // Thread run
#define PROC_EV  0x0004  // Thread proc. event
#define PROC_TM  0x0008  // Thread proc. timer event
// ..................

// working thread - reads msg's from fifo
//class CWThread: public CDbThread
class CWThread: public CThreadObj {
  // disable copy constructor.
  CWThread(const CWThread&);
  void operator = (const CWThread&);

  friend class CWrkLua;

 protected:
  static uChar nRun;         // global number of running threads
  static uChar nReload;
  volatile static uChar nWaiting;
  // Common for all wrkThreads Lua-Init
  static char  eFlag;  // flag to exit w-threads
  uInt         nMsgTot;
  STmVal       tmVal;  // For statistic
  CEvent*      pe;
  CContext*    pCont;
  //CUdpClntSock pSock;
  uShort nWait;     // Num of context queued events
  uChar  wtId, seq; // event sequence
  char   WStstRun;

  uInt tmDelta();
  int Run(pCEvent p) { return pCont->Run(p,this); }

 public:
  CWThread(uChar id);
  virtual ~CWThread();
  static void wStop()  { eFlag = 'x'; }
  static void wStart() { eFlag = '\0'; }
  virtual void* go();
  uShort getEvent();
  uChar  getWTId() { return wtId; }
  void   setTimer(int cid, int tm, int type, int f);
  // Factory to produce new context & add it to hash
  static CContext* genNewCtx(CEvent* pe);
  static uChar nRThrd() { return nRun; }
  static void rloadScript() { nReload = 0x0; }

 private:
  // To diagnose blocking of work thread
  uInt   cntxtId;
  uInt   nMsg,msgTm;  // Last msg Proc-Start-Time
  uShort wState;      // Last state of work thread

 public:
  void startWTh();
  uInt gNMsg()    { return nMsg; }
  uInt gCtxtId()  { return cntxtId; }
  uInt gMTime()   { return msgTm; }
  int  getNMsg()  { return nMsg; }
  uShort gMId()   { return pe ? pe->getEv() : 0u; }
  uShort gStat()  { return wState; }
  uShort gNWiat() { return nWait; }
  // Get number of waiting threads
  static const uChar nWaitingTh() { return nWaiting; }
};
typedef CWThread* pWThread;
extern pWThread pWrkThArr[];

#endif // WTHREAD_H

// $Id: WThread.h 372 2010-05-08 15:29:49Z asus $
