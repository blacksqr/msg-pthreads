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
