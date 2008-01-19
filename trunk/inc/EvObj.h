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
#ifndef EVOBJ_H
#define EVOBJ_H

#include <string.h>
#include <Global.h>
#include <memPool.h>

class CEvent;
typedef CEvent* pCEvent;

#ifdef BIG_FIFO
#define FfQueSize 0x10000
#define FfQueSz   0xFFFF
typedef uShort _FFInd;
#else  // BIG_FIFO
#define FfQueSize 0x100
#define FfQueSz   0xFF
typedef uChar _FFInd;
#endif // BIG_FIFO

class CFifo {
#define nFf ((end >= beg) ? (end - beg) : ((FfQueSz - beg) + end + 1))
  // disable copy constructor.
  CFifo(const CFifo&);
  void operator = (const CFifo&);
 protected:
  CPMutex wMut;
  CPCond  rCnd;
  pCEvent pObj[FfQueSize];
  _FFInd beg,end,gr1,gr2;
  char gFlg;
 public:
  CFifo();
  ~CFifo();
  _FFInd addEl(pCEvent p);
  _FFInd addTopEl(pCEvent p);
  pCEvent getEl();
  _FFInd getNN() { return nFf; }
  CPCond& getCond() { return rCnd; }
  _FFInd qSize() { return nFf; }
  void signal() {
    if(nFf > 1) rCnd.broadcast();
    else rCnd.signal();
  }
};
extern CFifo GlEvFifo;

// ===============================================

class CEvent {
  // disable copy constructor.
  CEvent(const CEvent&);
  void operator = (const CEvent&);
 protected:
  const uInt   cId;    // Context ID
  pVoid        pData;  // Data pointer or ID
  uShort       Event;  // Event ID
  uChar        sgThId; // sign thread-id
  char         nCtxt;  // flag to start new Ctxt
  // private Cons - Dest
  CEvent(uInt i,short e,uInt  d,uChar tId,char x);
  CEvent(uInt i,short e,pVoid d,uChar tId,char x);
  ~CEvent() {
    // Dummy -  use memPool
    DBG(">CEvent::~CEvent> %u %d %u %u %d\n",cId,Event,(uInt)pData,sgThId,nCtxt);
  }
 public:
  uInt getCId()   { return cId; }
  uShort getEv()  { return Event; }
  // Get data pointer OR Id
  pVoid Data() { return pData; }       // data pointer
  uInt  dtId() { return (uInt)pData; } // Id
  // put event in event Fifo
  uChar put(char x = '\0') { return x ? GlEvFifo.addTopEl(this) : GlEvFifo.addEl(this); }
  uChar sigThId() { return sgThId; }
  // send Signal
  void sign() { GlEvFifo.signal(); }
  // if event to start new context
  char newCtx() { return nCtxt; }
  pCEvent stripTm() {
    Event -= TOut_Shift;
    return this;
  }
  // memPool - new & delete
  static pCEvent newEv(uInt i,short ev,uInt  d=0u,uChar tId='\0',char x='\0');
  static pCEvent newEv(uInt i,short ev,pVoid d,   uChar tId='\0',char x='\0');
  static void delEv(pCEvent pt);
};

class CProcAllEv {
  // disable copy constructor.
  CProcAllEv(const CProcAllEv&);
  void operator = (const CProcAllEv&);
 public:
  CProcAllEv() {}
  ~CProcAllEv() {
    GlEvFifo.signal();
  }
};

#endif // EVOBJ_H
// $Log$
