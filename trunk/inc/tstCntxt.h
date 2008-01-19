#ifndef TST_CNTXT_H
#define TST_CNTXT_H
#include <Context.h>

#define C_CTXT_TST 0x02

extern uInt iCsCtxt;
extern uInt iDbCtxt;

// on-Test-event context
class CTstCntx: public CContext {
 protected:
  virtual uShort Run(CEvent* pe,CWThread* pwt);
  virtual uShort onTimer(uLong tn,CEvent* pe,CWThread* pwt);
  virtual char onHalt();
  char rx, inTcl;
 public:
  CTstCntx(uInt i);
  ~CTstCntx() { DBG("CTstCntx::~CTstCntx cId=%u\n",cId); }
};

#endif // TST_CNTXT_H
