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
#ifndef ITS_TIME_H
#define ITS_TIME_H

#include <map>
#include <Thread.h>
#include <memPool.h>
#include <getTime.h>

class CTmThrd;

class CTmObj {
  // disable copy constructor.
  CTmObj(const CTmObj&);
  void operator = (const CTmObj&);

  friend class CTmThrd;

 protected:
  uInt   id;   // Context ID
  uShort inf;  // some info
  uChar  type; // timer type
  CTmObj(uChar t,uInt i,uShort f): id(i),inf(f),type(t) {
    DBG(">>CTmObj::CTmObj  ID=0x%X Type=0x%X\n",id,type);
  }
  ~CTmObj() {  // Dummy -  use memPool
    DBG(">>CTmObj::~CTmObj  ID=0x%X Type=0x%X\n",id,type);
  }
 public:
  static CTmObj* newTm(uChar t,uInt i=0,uShort f=0u);
  static void delTm(CTmObj* pt);
};

// timer events multimap
class CTmQueue:
public std::multimap < uLong, CTmObj* >
{
  // disable copy constructor.
  CTmQueue(const CTmQueue&);
  void operator = (const CTmQueue&);

  friend class CTmThrd;

 private:
  CPMutex m;   // to protect stl::multimap
  CPCond  wc;  // wait on condition
  uLong   nt;  // next timeout
  char wait();

  CTmQueue(): nt(0xFFFFFFFF) { wc.lock(); }
  ~CTmQueue() { wc.unlock(); }

  void set(uLong tm,uInt id,uShort type,uShort f=0u);
  void operator () (uLong tm,uInt id,uShort type,uShort f=0u) { set(tm,id,type,f); }
  // Set absolute timeout, tm - seconds since the Epoch
  void absSet(time_t tm,uInt id,uShort type,uShort f=0u) {
    int t = tm - time(NULL);
    if(t > 0) set(100u*t,type,id,f);
  }
};

class CTmThrd: public CThreadObj {
  // disable copy constructor.
  CTmThrd(const CTmThrd&);
  void operator = (const CTmThrd&);

 private:
  CTmQueue tQueue;
 public:
  CTmThrd();
  virtual ~CTmThrd();
  virtual void* go();

  void set(uLong tm,uInt id,uShort type,uShort f=0u) {
    tQueue.set(tm,id,type,f);
  }
  void absSet(time_t tm,uInt id,uShort type,uShort f=0u) {
    tQueue.absSet(tm,id,type,f);
  }
};
extern CTmThrd* pTimeThrd;

#endif // ITS_TIME_H

// $Id: TimerMap.h 319 2010-01-15 20:43:05Z asus $
