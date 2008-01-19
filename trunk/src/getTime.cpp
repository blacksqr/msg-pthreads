// $Id$
#include <getTime.h>
#include <dprint.h>

typedef unsigned int uInt;

CAppTimer::CAppTimer() {
  ::gettimeofday(&T, 0);
  // init week's minute
  //(void)getWkMinute();
}

// time is in 1/100 sec from Apps startup
unsigned long CAppTimer::now(int s, char ms /* in 1/100 sec*/) {
  struct timeval x;
  ::gettimeofday(&x, 0);
  x.tv_sec -= T.tv_sec;
  x.tv_usec -= T.tv_usec;
  if(x.tv_usec < 0) {
    --(x.tv_sec);
    x.tv_usec += 1000000;
  }
  // time is in 1/100 sec
  return 100 * (x.tv_sec + s) + (x.tv_usec+5000)/10000 + ms;
}

void CAppTimer::tmOut(unsigned long ms, struct timeval* pT, char flag) {
  if(!flag) {
    // ms - delta timeout
    ::gettimeofday(pT, 0);
    pT->tv_sec  += ms/100;
    pT->tv_usec += 10000*(ms%100);
    if( pT->tv_usec > 999999) {
       pT->tv_usec -= 1000000;
       pT->tv_sec++;
    }
    //DBG(">CAppTimer::tmOut> delta\n\tNow   S=%u Ms=%u\n\tStart S=%u Ms=%u\n",
    //(uInt)(pT->tv_sec),(uInt)(pT->tv_usec),
    //(uInt)(T.tv_sec),(uInt)(T.tv_usec));
  } else {
    // ms - absolut apps time
    pT->tv_sec  = T.tv_sec + ms/100;
    pT->tv_usec = T.tv_usec + 10000*(ms%100);
    if( pT->tv_usec > 999999) {
       pT->tv_usec -= 1000000;
       pT->tv_sec++;
    }
  }
}

#if 0
#ifdef FEUER_TAG_DEF  // Setup in InitScript
unsigned char FeuerTagArr[45];
#endif // FEUER_TAG_DEF

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

// $Log$
