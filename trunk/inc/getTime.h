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

class CAppTimer {
 private:
  struct timeval T;
 public:
  CAppTimer();
  ~CAppTimer() {}
  unsigned long now(int s=0, char ms='\0');
  void tmOut(unsigned long ms, struct timeval* pT, char flag='\0');
};

extern CAppTimer appTimer;
#define tNow() appTimer.now()
// with delta timeout
#define tOut(ms,pT) appTimer.tmOut(ms,pT)
// Absolut timeout
#define tOutAbs(ms,pT) appTimer.tmOut(ms,pT,'x')

#endif // ITSTIME_H
// $Log$
