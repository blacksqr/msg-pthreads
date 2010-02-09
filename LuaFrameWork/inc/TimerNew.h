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
#ifndef FRACT_TIME_H
#define FRACT_TIME_H

#include <Thread.h>
#include <memPool.h>
#include <getTime.h>

class CTmThrd;

// Singl-link list of timers
class CTmObj {
  /* disable copy constructor */
  CTmObj(const CTmObj&);
  void operator = (const CTmObj&);

  friend class CTmQueue;
  friend class CTmThrd;

 protected:
  CTmObj* nxt;
  uLong   tmo;   // Timeout
  uInt    id;    // Context ID
  uShort  inf;   // Timeout info
  uShort  type;  // Timeout type

  CTmObj(uInt tm,uChar tp,uInt i,CTmObj* pn,uShort f): nxt(pn),tmo(tm),id(i),inf(f),type(tp)
  { DBG(">>CTmObj::CTmObj  %lu ID=0x%X Type=0x%X\n",tmo,id,type); }
  ~CTmObj() {}
 public:
  static CTmObj* newTm(uInt tm,uChar t,uInt i=0,CTmObj* pn=NULL,uShort f=0u);
  static void delTm(CTmObj* pt);
};

// ===================================

// The base of the timer optimization --
//   The number of the short timeout's is exponentially
//   more as the number of the long one

#define MutArrSz   0x3FF  // 0x1FF

#ifdef BIG_TmQueue

#define TMQSize    0x10000
typedef uShort     Q_Ind;
#define nCTMQLevel 2
#define L0Delta    0x20

#else  // BIG_TmQueue

#define TMQSize    0x100
typedef uChar      Q_Ind;
#define nCTMQLevel 3
#define L0Delta    0x40

#endif // BIG_TmQueue

class CTmQueue {
  // disable copy constructor.
  CTmQueue(const CTmQueue&);
  void operator = (const CTmQueue&);

  friend class CTmThrd;
#define nDelta(i) ((i >= beg) ? (i - beg) : (i + (TMQSize - beg) + 1))

  // All element's timeouts are relative to position in array pTmPool
  // Time of pTmPool[beg]     = tmBeg;
  // Time of pTmPool[beg + 1] = tmBeg + delta;
 protected:
  CPMutex   m;               // To protect object members
  CPMutex   lm[1+MutArrSz];  // To protect List operations

  // Pointer to next level CTmQueue object
  CTmQueue* nxtLevel;
  const uLong delta;    // Delta time from pTmPool[n] to pTmPool[n+1]
  uLong     nt;         // next timeout (relative to tmBeg)
  uLong     tmBeg;      // Abs. time of element pTmPool[beg]
  CTmObj*   pTmPool[TMQSize];
  uChar     nTimer [TMQSize];
  Q_Ind     beg;        // Current start element
  const uChar level;    // Level of CTmQueue object

  CTmObj* get0(CTmObj*& pw,CTmObj* pe,uLong tx,uChar& nn);

  void nextTmOut();
  // CTmQueue reorganisation to optimal state
  void normalise(uLong tn);

  CTmQueue(uLong d, uLong t, uChar l='\0');
  ~CTmQueue();

  // Set relative timeout - tm
  void set(uLong tm,uInt id,uShort type,uShort f=0u);
  void operator () (uLong tm,uInt id,uShort type,uShort f=0u) { set(tm,id,type,f); }
  void operator () (CTmObj* p) { set(p->tmo,p->id,p->type,p->inf); }

  // Set absolute timeout, tm - seconds since the Epoch
  void absSet(uLong tm,uInt id,uShort type,uShort f=0u);

  // Get ordered list of CTmObj objects with time < tm
  CTmObj* get(uLong tm);
  CTmObj* operator () (uLong tm) { return get(tm); }
};

// ===================================

class CTmThrd: public CThreadObj {
  // disable copy constructor.
  CTmThrd(const CTmThrd&);
  void operator = (const CTmThrd&);

 protected:
  CPCond    wc;      // Time-wait on condition (Timer itself)
  CTmQueue  tQueue;  // Timer object

  // return 'x'  - new timeOut was set
  //        '\0' - timeto send tmOut events
  char wait();
 public:
  CTmThrd();
  ~CTmThrd();

  void set(uLong tm,uChar type,int id = 0,uShort f=0u) {
    tQueue(tm, type, id, f);
    wc.signal();
  }
  void absSet(uLong tm,uChar type,int id = 0,uShort f=0u) {
    tQueue.absSet(tm, type, id, f);
    wc.signal();
  }
  virtual void* go();
};
extern CTmThrd* pTimeThrd;

#endif // FRACT_TIME_H

// $Id: TimerNew.h 350 2010-02-08 18:38:31Z asus $
