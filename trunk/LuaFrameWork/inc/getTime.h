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
#ifndef ITSTIME_H
#define ITSTIME_H
#include <sys/time.h>

#define MicroSec    1000000
#define NanoSec     (1000*MicroSec)
#define TimeQuant   1000          // application's time quant is 1/1000 s
#define QuantMsSec  (MicroSec/TimeQuant)
#define QuantNnSec   (NanoSec/TimeQuant)

class CAppTimer {
 private:
  // Timer start point
  unsigned long T;
 public:
  CAppTimer();
  ~CAppTimer() {}
  unsigned long now();
  // flag ? Relative_timer : Absolute_timer
  void tmOut(unsigned long ms, struct timeval* pT, char flag='\0');
  void nSleep(unsigned long timeOut);
};

extern CAppTimer appTimer;

#define tNow()         appTimer.now()          // with delta timeout
#define tOut(ms,pT)    appTimer.tmOut(ms,pT)   // Absolut timeout
#define tOutAbs(ms,pT) appTimer.tmOut(ms,pT,'x')
#define nsleep(to)     appTimer.nSleep(to)

#endif // ITSTIME_H

// $Id: getTime.h 317 2010-01-15 17:34:59Z asus $
