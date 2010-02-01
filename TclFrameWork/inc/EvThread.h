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
#ifndef ITS_CALL_EV_H
#define ITS_CALL_EV_H

#include <Global.h>
#include <Thread.h>
#include <Context.h>

// Values to use with <Flag>
#define STOP_EV_THRD   0x01
#define APPL_EXIT      0x02
// max number of sign threads
#define MAX_N_STHREAD 16

class CEvThrd: public CThreadObj {
 private:
  static uChar g_sigThId;
  uLong  tmOfLastMsg;  // Time of last <set new tmOut>
  // disable copy constructor.
  CEvThrd(const CEvThrd&);
  void operator = (const CEvThrd&);
 protected:
  const char* tName;
  // time to block before keepAlive msg in 0.01 sec
  uShort      kpAlive;
  const uChar sigThId;
  virtual CEvent* getEvent()=0;
  uChar Flag;
  // find CtxtId for new external event
  // it used in hash to map external event => CtxtId
  // in base class - reserve id for new CtxtId
  uInt getCId() { return CContext::reservId(); }
  // Protected constructor
  CEvThrd(uShort ka);
 public:
  ~CEvThrd() {}
  void stop() { Flag |= STOP_EV_THRD; }
  virtual void* go();
  uShort getKATime()   { return kpAlive; }
  uLong getTm()        { return tmOfLastMsg; }
  void  setTm(uLong t) { tmOfLastMsg = t; }
  const char* thName() { return tName; }
};
typedef CEvThrd* pCEvThrd;

extern pCEvThrd g_evThrdArr[];

#endif // ITS_CALL_EV_H
// $Log$
