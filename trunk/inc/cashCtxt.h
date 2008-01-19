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
#ifndef CASH_CTXT_H
#define CASH_CTXT_H
#include <Context.h>

#define C_CTXT_CASH 0x04

class CCsItem {
  friend class CCashCtxt;
  // disable copy constructor.
  CCsItem(const CCsItem&);
  void operator = (const CCsItem&);
 protected:
  CCsItem* nxt;
  pVoid    data;
  CCsItem(CCsItem* n,pVoid p): nxt(n),data(p) {}
  ~CCsItem() {
  }
 public:
  CCsItem* next() { return nxt; }
  // memPool - new & delete
  static CMPool Cash_mem;
  static CCsItem* newItem(CCsItem* n,pVoid d);
  static void delItem(CCsItem* pt);
};

// Common cash context
class CCashCtxt: public CContext {
  // disable copy constructor.
  CCashCtxt(const CCashCtxt&);
  void operator = (const CCashCtxt&);
 protected:
  CCsItem* root;
  uInt iDbCtxt,T;
  uShort mWait,mCsh,cshSize;
  uChar flg;
  void saveCash(uInt xt);
  virtual uShort Run(CEvent* pe,CWThread* pwt);
  virtual uShort onTimer(uLong tn,CEvent* pe,CWThread* pwt);
  virtual char onHalt();
 public:
  CCashCtxt(uInt i,uInt db,uShort tm,uShort m);
  ~CCashCtxt();
};

#endif // CASH_CTXT_H
