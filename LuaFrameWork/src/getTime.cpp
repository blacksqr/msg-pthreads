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
#include <time.h>
#include <getTime.h>
#include <dprint.h>
#include <stdio.h>

CAppTimer::CAppTimer() {
  struct timeval x;
  ::gettimeofday(&x, 0);
  T = 1 + x.tv_sec;
  nSleep((MicroSec - x.tv_usec)/QuantMsSec);
  printf("CAppTimer::CAppTimer X<%lu,%lu> %lu Sleep-%lu %lu\n",
	 x.tv_sec,x.tv_usec,T,((MicroSec - x.tv_usec)/QuantMsSec),(MicroSec - x.tv_usec));
  // init week's minute
  //(void)getWkMinute();
}

// time is in 1/TimeQuant sec from Apps init
unsigned long CAppTimer::now() {
  struct timeval x;
  ::gettimeofday(&x, 0);
  // time is in 1/TimeQuant sec
  return (TimeQuant * (x.tv_sec - T)) +
    ((x.tv_usec+(QuantMsSec/2))/QuantMsSec);
}

void CAppTimer::tmOut(unsigned long ms, struct timeval* pT, char flag) {
  if(!flag) {
    // ms - delta timeout
    ::gettimeofday(pT, 0);
    pT->tv_sec  += ms/TimeQuant;
    pT->tv_usec += QuantMsSec * (ms%TimeQuant);
  } else {
    // ms - absolut apps time
    pT->tv_sec  = T + ms/TimeQuant;
    pT->tv_usec = QuantMsSec * (ms%TimeQuant);
  }
}

// sleep wrapper in 1/TimeQuant s
void CAppTimer::nSleep(unsigned long timeOut) {
  struct timespec req;
  req.tv_sec  = timeOut / TimeQuant;
  req.tv_nsec = QuantNnSec * (timeOut % TimeQuant);
  (void)nanosleep(&req, 0x0);
}

#if 0
#ifdef FEUER_TAG_DEF  // Setup in InitScript
unsigned char FeuerTagArr[45];
#endif // FEUER_TAG_DEF

// Get week's min
short getWkMinute() {
static CPMutex Mwm;
  static unsigned long weekStart = 0l;
  static char WW[] = {6,0,1,2,3,4,5};
  time_t ss = time(NULL);
  if((ss - weekStart) > 604800) {
    CSglLock sl(Mwm);
    struct tm* pTM = localtime(&ss);
    weekStart = ss -
	(60 * (60 * (24 * WW[pTM->tm_wday] +
         pTM->tm_hour) + pTM->tm_min) + pTM->tm_sec);
  }
#ifdef FEUER_TAG_DEF
  {
    // process Feuertagen as Sohntag
    unsigned char n,nn;
    n = pTM->tm_yday / 8;
    nn = pTM->tm_yday % 8;
    if(FeuerTagArr[n] & _tmFlg_[nn])
      return 9000;
  }
#endif // FEUER_TAG_DEF
  return (ss - weekStart) / 60;
}
#endif // 0


// $Id: getTime.cpp 317 2010-01-15 17:34:59Z asus $
