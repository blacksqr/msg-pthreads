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
#include <string.h>
#include <stlObj.h>
#include <memPool.h>
#include <workTcl.h>
#include <dprint.h>

class CWThread;
class CContext;

// ===============================================

class CCnxtMap:
public __gnu_cxx::hash_map < uInt,CContext*,__gnu_cxx::hash<uInt>,eqUInt >
{
  friend class CContext;
 protected:
  CPMutex m;
  CContext* getv(const uInt n) {
    iterator i = find(n);
    return (i==end()) ? NULL : (i->second);
  }
 public:
  CCnxtMap() {}
  virtual ~CCnxtMap() { DBG("CCnxtMap::~CCnxtMap %u\n",size()); }
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

// base class for event's context
class CContext {
  friend class CWThread;
  friend class CSetTimer;
 private:
  static uInt cGlId;
  static CPMutex cGlIdm;
  CPMutex wMut;
 protected:
  uInt cId;       // current Context ID
  uChar timerFlags[1 + TOut_MAX/8];
  uChar State, seq0, seq1, ctxtType;
  char  rfCount; // Reff. count
  CContext(uInt i);
  // It's possible to place Cntxt-obj in memPoll
  // Destructor must be NOT virtual & and only in last derived class
  virtual ~CContext();
  virtual void delHook() { delete this; }
  // called from WThread => Return ~0-in TCL; 0-get new Event
  virtual uShort Run(CEvent* pe,CWThread* pwt)                   = 0;
  virtual uShort onTimer(uLong tn,CEvent* pe,CWThread* pwt=NULL) = 0;
  // Hook after TCL-Script
  virtual void remRefHook() { DBG("Context::remRefHook %u Seq %u<>%u\n",cId,seq0,seq1); }
  // On apps exit - halt still running ctxt's
  //  will be called until return '\0'
  virtual char onHalt()            = 0;
  void setTimer(uLong tm,uChar type,uShort f=0u);
  // disable copy constructor.
  CContext(const CContext&);
  void operator = (const CContext&);
 public:
  virtual int cmdProc(CWrkTcl* pTcl,int argc,Tcl_Obj* const argv[]);
  void addRef() { ++rfCount; }
  void remRef();
  uChar getCtxtType() { return ctxtType; }
  void destruct(); // free to delete
  void Halt();     // Remove context
  uInt getId() const { return cId; }
  static void  send(short evType, uInt cid, pVoid p=NULL, char n='\0');
  static void send0(short evType, uInt cid, pVoid p=NULL, char n='\0'); // high prio
  // wMut - Interface
  int lTry()   { return wMut.ltry(); }
  void lock()   {
    DBG("Context::lock> cId=%u Seq %u<>%u\n",cId,seq0,seq1);
    (void)wMut.lock();
  }
  void unlock() {
    DBG("Context::unlock> cId=%u Seq %u<>%u\n",cId,seq0,seq1);
    (void)wMut.unlock();
  }
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
class CHKCtxt: public CContext {
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
  virtual uShort Run(CEvent* pe,CWThread* pwt);
  virtual uShort onTimer(uLong tn,CEvent* pe,CWThread* pwt);
  virtual char onHalt() { return '\0'; }
  void setHkTm(uLong tm,uChar type) { setTimer(tm,type); }
  ~CHKCtxt();
 public:
  CHKCtxt(uInt i);
  virtual int cmdProc(CWrkTcl* interp,int argc,Tcl_Obj* const argv[]);
  //char onFail() {} // IF to process system fail
  // Set info for wrk-tcl-script
  char* getStatus(char* stat) {
    sprintf(stat,"HK> %u\n",(uInt)rTime);
    return stat;
  }
};
// ID of House-keeping context & global pointer
extern uInt HkCId;
extern CHKCtxt* pHKCtxt;
// Flag to stop new context
extern char stopNewCtxt;

#endif // CCONTEXT_H
