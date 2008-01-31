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
#include <Timer.h>
#include <getTime.h>
#include <WThread.h>
#include <adminTcl.h>
#include <dprint.h>

// change to DebugLevel param -d ...
char dbgLevel = L_DBG;

// Global apps event FIFO
CFifo GlEvFifo;

extern "C" {
  void daemon_start(int);
} // extern "C"

uChar GlAppsFlag = '\0';
CHKCtxt* pHKCtxt = NULL;

CAppTimer appTimer;
//static short weekMinute;
time_t globCurentTime;

// ======================================

// helper fuctions: String phon => LLong
long long ph2LLong(char* ph) {
  char buff[32];
  ph[31] = '\0';
  strcpy(buff, "1");
  strcat(buff, ph);
  return atoll(buff);
}

// ======================================
const char* outNull = "/dev/null";

class CSysLog {
public:
  CSysLog(const char* of=outNull)  {
    if(!(GlAppsFlag & LOG_IN_STDERR)) {
      ::openlog("Its", LOG_CONS | LOG_PID, /*LOG_USER*/ LOG_DAEMON);
    } else if(GlAppsFlag & DAEMON_MODE) {
      FILE* fin = fopen(outNull, "a+");
      (void)fopen(of, "a+");
      (void)dup(1);
      fclose(fin);
    }
  }
  ~CSysLog() {
    if(!(GlAppsFlag & LOG_IN_STDERR)) {
      ::closelog();
    } else if(GlAppsFlag & DAEMON_MODE) {
      fclose(stdout);
      fclose(stderr);
    }
  }
};

char appsHomeArr[320];
const char* appsHome = NULL;

int main(int argc,char* argv[]) {
  int rrr = 0;
  const char* outFl = NULL;
  GlAppsFlag |= LOG_IN_STDERR;
  while(argc > 1) {
    if(!strcmp(argv[1],"-D")) {
      // Deamon mode
      DBG("***> switch to deamon mode\n");
      GlAppsFlag |= DAEMON_MODE;
      argv = &argv[1];
      --argc;
      continue;
    }
    if(!strcmp(argv[1],"-slog")) {
      // Deamon mode
      DBG("***> Use syslog\n");
      GlAppsFlag &= ~LOG_IN_STDERR;
      argv = &argv[1];
      --argc;
      continue;
    }
    if(!strcmp(argv[1],"-f")) {
      // File to print LOG
      argc -= 2;
      outFl = argv[2];
      argv = &argv[2];
      continue;
    }
    if(!strcmp(argv[1],"-d")) {
      // Debug level 0-4
      argc -= 2;
      argv = &argv[1];
      dbgLevel = *argv[1] - '0' + 1;
      argv = &argv[1];
      dbgLevel = (dbgLevel<0) ? 0x0 : dbgLevel;
      dbgLevel = (dbgLevel>4) ? 0x4 : dbgLevel;
      continue;
    }
    LOG(L_WARN,"Main: Bad option \"%s\"\n",argv[1]);
    argv = &argv[1];
    --argc;
  }
  {
    appsHome = getenv("ITS_APP_HOME");
    if(appsHome)
      strcpy(appsHomeArr,appsHome);
    else
      appsHome = getcwd(appsHomeArr,sizeof(appsHomeArr));
    LOG(L_WARN,"Main: Application HOME <%s>\n",(char*)appsHomeArr);
    strcat(appsHomeArr,"/tcl/");
    appsHome = appsHomeArr + strlen(appsHomeArr);
  }
  if(GlAppsFlag & DAEMON_MODE)
    daemon_start(0);
  {
    // Init random numbet
    struct timeval x;
    ::gettimeofday(&x, 0);
    srandom(x.tv_sec * x.tv_usec);
  }
  {
    // start SysLog interface
    CSysLog _AppSysLog(outFl);
    pHKCtxt = new CHKCtxt(CContext::reservId());
    // Start timer thread
    pTmThrd = new CTmThrd;
    // .....................
    // to clean start-up => all wait until unlock
    GlEvFifo.getCond().lock();
    {
      // Admin interpreter
      CAdmTcl tcl;
      strcpy((char*)appsHome,"admin.tcl");
      // Set first HkTmOut in 7.77 s
      tmQueue.set(777,TOut_HsKeep,HkCId,0);
      DBG("*** Start admin-TCL script <%s> ***\n",appsHomeArr);
      rrr = tcl.Init() ? -2 : tcl.EvalFile(appsHomeArr);
    }
  }
  LOG(L_NOTICE,"Main: Exit OK >%d<\n",rrr);
  return rrr;
}
