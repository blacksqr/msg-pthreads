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
#ifndef CCONTEXT_H
#define CCONTEXT_H

#include <EvObj.h>
#include <stlObj.h>
#include <memPool.h>
#include <dprint.h>

class CWThread;
class CContext;

// ===============================================

class CCnxtMap:
public __gnu_cxx::hash_map < uInt,CContext*,__gnu_cxx::hash<uInt>,eqUInt >
{
  // disable copy constructor.
  CCnxtMap(const CCnxtMap&);
  void operator = (const CCnxtMap&);

  friend class CContext;

 protected:
  CPMutex m;
  CContext* getv(const uInt n) {
    iterator i = find(n);
    return (i==end()) ? NULL : (i->second);
  }
 public:
  CCnxtMap() {}
  virtual ~CCnxtMap() { DBG("CCnxtMap::~CCnxtMap %u\n",(uInt)size()); }
  CPMutex& getM() { return m; }
  CContext* get(const uInt n) {
    CSglLock sl(m);
    if(size())
      return getv(n);
    return NULL;
  }
  void del(const uInt n);
};
// call-context hash
extern CCnxtMap hashCntxt;
// timer event hash
//CUIntMap hashTimer;

// ===============================================

class CCtxtQq {
  // disable copy constructor.
  CCtxtQq(const CCtxtQq&);
  void operator = (const CCtxtQq&);

 protected:
  CCtxtQq* nxt;
  pCEvent pe;
 public:
  CCtxtQq(pCEvent p) : nxt(NULL), pe(p) {}
  ~CCtxtQq() {}

  CCtxtQq*& gNxt() { return nxt; }
  pCEvent get() { return pe; }
};

// ===============================================

// base class for event's context
class CContext {
  // disable copy constructor.
  CContext( const CContext& );
  void operator = ( const CContext& );

  friend class CWThread;
  friend class CSetTimer;

 private:
  static uInt cGlId;
  static CPMutex cGlIdm;
  CPMutex wMut;
  CPMutex qMut;

  CCtxtQq* pQqRoot;
  CCtxtQq* pQqLast;

 protected:
  uInt cId;       // current Context ID
  uChar timerFlags[1 + TOut_MAX/8];
  uChar State, seq0, seq1, ctxtType;
  char  rfCount; // Reff. count

  CContext(uInt i);
  // It's possible to place Cntxt-obj in memPoll
  // Destructor must be NOT virtual and ONLY in last derived class
  virtual ~CContext();

  virtual void delHook();
  // Hook after Lua-Script
  virtual void remRefHook();

  // called from WThread => Return ~0-in Lua; 0-get new Event
  virtual uShort Run(pCEvent pe,CWThread* pwt) = 0;
  virtual uShort onTimer(uLong tn,pCEvent pe,CWThread* pwt=NULL) = 0;
  // On apps exit - halt still running ctxt's
  //  will be called until return '\0'
  virtual char onHalt() = 0;
  void setTimer(uLong tm,uShort type,uShort f=0u);
 public:
  void hashCId();
  void addRef() { ++rfCount; }
  void remRef();
  uChar getCtxtType() { return ctxtType; }
  void destruct(); // free to delete
  void Halt();     // Remove context
  uInt getId() const { return cId; }
  static void  send(short evType, uInt cid, pVoid p=NULL, char n='\0');
  static void send0(short evType, uInt cid, pVoid p=NULL, char n='\0'); // high prio

  // wMut - Interface
  int lTry()   {
    DBG("Context::lTry> cId=%u Seq %u<>%u\n",cId,seq0,seq1);
    return wMut.ltry();
  }
  void lock()   {
    DBG("Context::lock> cId=%u Seq %u<>%u\n",cId,seq0,seq1);
    (void)wMut.lock();
  }
  void unlock() {
    DBG("Context::unlock> cId=%u Seq %u<>%u\n",cId,seq0,seq1);
    (void)wMut.unlock();
  }

  void QTop(pCEvent pe);
  void Queue(pCEvent pe);
  pCEvent GetEv();
  CPMutex& getQm() { return qMut; }
  void qLock() { qMut.lock(); }
  void qUnlock() { qMut.unlock(); }

  // Public timer if-ce
  void delTimer(uChar type);
  uChar isTimerOn(uChar type);
  uChar getState() { return State; }
  static uInt reservId();
  static CContext* findCtxt(uInt id);
};
typedef CContext* pContext;

// ===============================================

#define C_CTXT_HK 0x01

// on House-keeping event context
class CHKContext: public CContext {
  friend class CHsKeep;
 protected:
  // system runtime params
  uLong  rTime;   // Timer value
  uInt   totMsg;  // Num. of processed msg
  char*  lstWarn; // Last warning
  uShort qSize;   // Size of system queue
  // Num of msg in a last 256 HK-events
  uShort msgNum[0x100];
  uChar  lastPos;
  // called from WThread
  virtual uShort Run(pCEvent pe,CWThread* pwt);
  virtual uShort onTimer(uLong tn,pCEvent pe,CWThread* pwt);
  virtual char onHalt() { return '\0'; }
  void setHkTm(uLong tm,uChar type) { setTimer(tm,type); }
  ~CHKContext();
 public:
  CHKContext(uInt i);
  //char onFail() {} // IF to process system fail
  // Set info for wrk-tcl-script
  char* getStatus() {
    char stat[360];
    char* pc = stat;
    sprintf(pc,"HK=> %u \n",(uInt)rTime);
    return strdup(stat);
  }
};
// ID of House-keeping context & global pointer
extern uInt HkCId;
extern CHKContext* pHKContext;
// Flag to stop new context
extern char stopNewCtxt;

#endif // CCONTEXT_H

// $Id: Context.h 308 2010-01-13 17:36:43Z asus $
