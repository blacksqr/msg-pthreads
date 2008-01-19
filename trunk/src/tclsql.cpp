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
// Part of tclSql application
#include <time.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <tclAdapter.h>
#include <dprint.h>

typedef long long      lLong;
typedef unsigned int   uInt;
typedef unsigned char  uChar;
typedef unsigned short uShort;
typedef unsigned long long ulLong;

typedef Tcl_Obj* pTcl_Obj;
typedef pTcl_Obj* ppTcl_Obj;

extern char* itoa(lLong ph,char* b);

#ifndef NO_DEBUG
char dbgLevel = L_INFO;
#endif // NO_DEBUG

short weekMinute;
time_t globCurentTime;
// SET in InitScript
unsigned char FeuerTagArr[45],
  tag0X[8] = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};
uChar GlAppsFlag = '\0';

extern "C" {
  int TclObjCmd(ClientData cData,Tcl_Interp*,int argc,Tcl_Obj* const argv[]);
  // memory cleaning function
  void CleanTclCmd(ClientData cmdObj);
  int Sqlite3_Init(Tcl_Interp*);
  int Md5_Init(Tcl_Interp*);
#ifdef TCL_THREADS
  int TclThread_Init(Tcl_Interp*);
#endif
} // extern "C"

///////////////////////////////
/////// Time interface ////////
///////////////////////////////
extern short weekMinute;

short getWkMinute() {
  static ulLong weekStart = 0l;
  static char WW[] = {6,0,1,2,3,4,5};
  time_t ss = time(NULL);
  if((ss - weekStart) > 604800) {
    struct tm* pTM = localtime(&ss);
    weekStart = ss -
	(60 * (60 * (24 * WW[pTM->tm_wday] + pTM->tm_hour) +
	       pTM->tm_min) + pTM->tm_sec);
  }
  return (ss - weekStart) / 60;
}

class CAppTimer {
 private:
  struct timeval T;
 public:
  CAppTimer();
  ~CAppTimer() {}
  unsigned long now(int s=0, char ms='\0');
  void tmOut(unsigned long ms, struct timeval* pT, char flag='\0');
};

CAppTimer appTimer;
#define tNow() appTimer.now()
// with delta timeout
#define tOut(ms,pT) appTimer.tmOut(ms,pT)
// Absolut timeout
#define tOutAbs(ms,pT) appTimer.tmOut(ms,pT,'x')

CAppTimer::CAppTimer() {
  ::gettimeofday(&T, 0);
  // init week's minute
  (void)getWkMinute();
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

// ========================================
// Custom TCL commands
// ========================================

class CWkTime: public CTclCmd {
 public:
  CWkTime(CTclInterp* i): CTclCmd(i,"wkTime") {}
  virtual ~CWkTime() {}
  virtual int cmdProc(int argc,Tcl_Obj* const argv[]);
};

int CWkTime::cmdProc(int argc,Tcl_Obj* const argv[]) {
  static char WW[] = {6,0,1,2,3,4,5};
  TGVal(lLong,x,argv[1]);
  time_t weekStart = 0;
  time_t tm = x;
  {
    struct tm* pTM = localtime(&tm);
    weekStart = tm - (60 * (60 * (24 * WW[pTM->tm_wday] + pTM->tm_hour) +
			    pTM->tm_min) + pTM->tm_sec);
  }
  tSetResult(tSetObj((short)((tm - weekStart) / 60)));
  return TCL_OK;
}

class CCntrCode: public CTclCmd {
  lLong setMaschtab(char l);
 public:
  CCntrCode(CTclInterp* i): CTclCmd(i,"cnCode") {}
  virtual ~CCntrCode() {}
  virtual int cmdProc(int argc,Tcl_Obj* const argv[]);
};

lLong CCntrCode::setMaschtab(char len) {
  lLong maschtab = 1ll;
  switch(len) {
    //                  01234567890
    case 1:  maschtab = 10000000000ll; break;
    case 2:  maschtab = 1000000000ll; break;
    case 3:  maschtab = 100000000ll; break;
    case 4:  maschtab = 10000000ll; break;
    case 5:  maschtab = 1000000ll; break;
    case 6:  maschtab = 100000ll; break;
    case 7:  maschtab = 10000ll; break;
    case 8:  maschtab = 1000ll; break;
    case 9:  maschtab = 100ll; break;
    case 10: maschtab = 10ll;
  }
  return maschtab;
}

int CCntrCode::cmdProc(int argc,Tcl_Obj* const argv[]) {
  const char* sCmd = interp->tGetVal(argv[1]);
  switch(*(sCmd++)) {
    case 'n': {
      // norm - normalise phon
      if(!strcmp(sCmd,"orm") && (argc==3)) {
	char buff[12];
	char* pp = interp->tGetVal(argv[2]);
	memset(buff,'0',sizeof(buff));
	for(uChar k='\0'; k<11; ++k) {
	  if(!pp[k]) break;
	  buff[k] = pp[k];
	}
	buff[11] = '\0';
	lLong kk = atoll(buff);
	lLong uk = 10000000000ll * (buff[0] - '0');
	Tcl_Obj* po[2];
	po[0] = tSetObj(kk);
	po[1] = tSetObj(uk);
	tSetResult(tSetObj(2,po));
      }
      break;
    }
    case 'r': {
      // rvert - country code
      if(!strcmp(sCmd,"vert") && (argc==5)) {
	TGVal(char,ln,argv[2]);
	TGVal(lLong,px,argv[3]);
	TGVal(lLong,pz,argv[4]);
	lLong mm = setMaschtab(ln);
	px /= mm; pz /= mm;
	tSetResult(tSetObj(px - pz));
      }
      break;
    }
    case 'k': {
      // key - CntryCode to key form
      if(!strcmp(sCmd,"ey") && (argc==4)) {
	TGVal(lLong,pp,argv[2]);
	TGVal(char,l,argv[3]);
	//printf("cnCode kcode %lld %d => %lld\n",pp,l,setMaschtab(l)*pp);
	tSetResult(tSetObj(setMaschtab(l) * pp));
      }
      break;
    }
    case 'c': {
      // ccode - CntryCode to orig form
      if(!strcmp(sCmd,"code") && (argc==4)) {
	TGVal(lLong,pp,argv[2]);
	TGVal(char,l,argv[3]);
	//printf("cnCode kcode %lld %d => %lld\n",pp,l,setMaschtab(l)*pp);
	tSetResult(tSetObj(pp/setMaschtab(l)));
      }
      break;
    }
  }
  return TCL_OK;
}

// ========================================
// MAIN
// ========================================
int myTcl_AppInit(Tcl_Interp* interp) {
  if (Tcl_Init(interp) == TCL_ERROR)
    return TCL_ERROR;
#ifdef TCL_THREADS
  if (TclThread_Init(interp) == TCL_ERROR)
    return TCL_ERROR;
#endif
  if((Sqlite3_Init(interp) == TCL_ERROR) || (Md5_Init(interp) == TCL_ERROR))
    return TCL_ERROR;
  CTclInterp* myTcl = new CTclInterp(interp);
  // init custom TCL commands
  new CWkTime(myTcl);
  new CCntrCode(myTcl);
  /* user-specific startup file to invoke if the application
   * is run interactively.  Typically the startup file is "~/.apprc"
   * where "app" is the name of the application.  If this line is deleted
   * then no user-specific startup file will be run under any conditions. */
  Tcl_SetVar(interp, "tcl_rcFileName", "~/.cdrsysrc", TCL_GLOBAL_ONLY);
  return TCL_OK;
}

int main(int argc,char* argv[]) {
  Tcl_Main(argc, argv, myTcl_AppInit);
  return 0;
}
