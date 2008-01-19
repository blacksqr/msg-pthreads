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
