#ifndef DB_CNTXT_H
#define DB_CNTXT_H
#include <Context.h>

#define C_CTXT_DB 0x03

extern uInt iCsCtxt;

// on-Test-event context
class CDbCntx: public CContext {
 protected:
  virtual uShort Run(CEvent* pe,CWThread* pwt);
  virtual uShort onTimer(uLong tn,CEvent* pe,CWThread* pwt);
  virtual void remRefHook() { send0(Evnt_DbReady, iCsCtxt); }
  virtual char onHalt();
 public:
  CDbCntx(uInt i);
  ~CDbCntx() { DBG("CDbCntx::~CDbCntx cId=%u\n",cId); }
  void InsData(CWThread* pwt);
};

#endif // DB_CNTXT_H!
