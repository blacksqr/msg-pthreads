#include <dprint.h>
#include <workTcl.h>
#include <WThread.h>
#include <getTime.h>
#include <cashCtxt.h>

uInt iCsCtxt = 0u;
uInt iDbCtxt = 0u;

// Cash pool
CMPool CCsItem::Cash_mem(sizeof(CCsItem));

CCsItem* CCsItem::newItem(CCsItem* n,pVoid d) {
  return new (Cash_mem.pAlloc()) CCsItem(n,d);
}

void CCsItem::delItem(CCsItem* pt) {
  pt->~CCsItem();
  Cash_mem.pFree(pt);
}

CCashCtxt::CCashCtxt(uInt i,uInt db,uShort tm,uShort m):
  CContext(i),root(NULL),iDbCtxt(db),T(0u),mWait(tm),mCsh(m),cshSize(0u)
{
  ctxtType = C_CTXT_CASH;
  flg = '\0';  // flag - DB ready
}

CCashCtxt::~CCashCtxt() {
  DBG("CashCtxt::~CCashCtxt cId=%u> cshSize=%u Flg=%d\n",cId,cshSize,flg);
  // send cash in DB
  if(cshSize)
    send0(Evnt_StoreInDb,iDbCtxt,root);
}

void CCashCtxt::saveCash(uInt xt) {
  flg = 'x';
  // send cash in DB
  send0(Evnt_StoreInDb,iDbCtxt,root);
  cshSize = 0u;
  root = NULL;
  if(xt - T > mWait/3) {
    // set new TOut_FlushCash timer
    setTimer(mWait,TOut_FlushCash);
    T = xt;
  }
}

uShort CCashCtxt::Run(CEvent* pe,CWThread*) {
  DBG("CashCtxt::Run cId=%u> EventId=%u %d %s\n",
      cId,pe->getEv(),cshSize,flg ? "Cash" : "inDB");
  switch(pe->getEv()) {
    case Evnt_SaveData:
      root = CCsItem::newItem(root,pe->Data());
      ++cshSize;
      if(!flg && (cshSize > mCsh))
	saveCash(tNow());
      break;
    case Evnt_DbReady:
      DBG("CashCtxt::Run cId=%u> Evnt_DbReady flg=%d\n",cId,flg);
      if(cshSize > mCsh) {
	saveCash(tNow());
      } else
	flg = '\0';
      break;
    default:
      DBG("CashCtxt::Run cId=%u> Unknow EventId=%u %d\n",cId,pe->getEv(),cshSize);
  }
  return 0x0;
}

uShort CCashCtxt::onTimer(uLong tn,CEvent* pe,CWThread* pwt) {
  if(pe->getEv() == TOut_FlushCash) {
    uInt xt = tNow();
    DBG("CashCtxt::onTimer %u> EventId=%u Xt=%u T=%u %d\n",
	cId,pe->getEv(),xt,T,xt-T);
    if(xt - T > mWait)
      saveCash(xt);
  } else {
    LOG(L_WARN,"CashCtxt::onTimer %u> Thrd=%u Wrong timeout %u\n",
	cId,pwt->getWTId(),pe->getEv());
  }
  return 0x0;
}

char CCashCtxt::onHalt() {
  DBG("CashCtxt::onHalt %u  csSize=%u\n",cId,cshSize);
  if(cshSize)
    saveCash(tNow()); // Try to store cash
  return '\0';
}
