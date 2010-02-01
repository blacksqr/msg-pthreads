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
#ifndef TST_CNTXT_H
#define TST_CNTXT_H
#include <Context.h>

#define C_CTXT_TST 0x02

extern uInt iCsCtxt;
extern uInt iDbCtxt;

// FSM - FiniteStateMaschine context
class CFsmCtxt: public CContext {
  // disable copy constructor.
  CFsmCtxt(const CFsmCtxt&);
  void operator = (const CFsmCtxt&);
 protected:
  char state;    // current FSM state (Init state=0)
  uChar inLua;   // Flag to return in TCL from state func
  virtual char onHalt();
  // called from WThread => Return ~0-in Lua; 0-get new Event
  virtual uShort Run(CEvent* pe,CWThread* pwt);
  virtual uShort onTimer(uLong tn,CEvent* pe,CWThread* pwt=NULL);
  void onEvError(CEvent* pe,char flg, char state);
  char onIdle(   CEvent* pe,CWThread* pwt,char flg);
  char onRing(   CEvent* pe,CWThread* pwt,char flg);
  char onProcid( CEvent* pe,CWThread* pwt,char flg);
  char onConn(   CEvent* pe,CWThread* pwt,char flg);
  char onDiscon( CEvent* pe,CWThread* pwt,char flg);
  typedef char ( CFsmCtxt::*TpStateFunc)(CEvent* pe,CWThread* pwt,char flg);
  TpStateFunc pSFunc[5];  // Array of pointers to state function
  CFsmCtxt(uInt i);
  ~CFsmCtxt() {}
 public:
  // memPool - new & delete
  static CContext* newFsm(uInt i);
  static void delFsm(CFsmCtxt* p);
};

// ======================================================

// on-Test-event context
class CTstCntx: public CContext {
  // disable copy constructor.
  CTstCntx(const CTstCntx&);
  void operator = (const CTstCntx&);
 protected:
  virtual uShort Run(CEvent* pe,CWThread* pwt);
  virtual uShort onTimer(uLong tn,CEvent* pe,CWThread* pwt);
  virtual char onHalt();
  uInt cStart;
  uChar nm,nMsg;
  virtual void delHook() { delTCtxt(this); }
  CTstCntx(uInt i);
  ~CTstCntx();
 public:
  // memPool - new & delete
  static CTstCntx* newTCtxt(uInt i);
  static void delTCtxt(CTstCntx* p);
};

#endif // TST_CNTXT_H

// $Id: tstCntxt.h 323 2010-01-17 20:29:38Z asus $
