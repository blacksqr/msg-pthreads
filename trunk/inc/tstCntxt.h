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
  CTstCntx(uInt i);
  ~CTstCntx();
  virtual void delHook() { delTCtxt(this); }
 public:
  // memPool - new & delete
  static CTstCntx* newTCtxt(uInt i);
  static void delTCtxt(CTstCntx* p);
};

#endif // TST_CNTXT_H
