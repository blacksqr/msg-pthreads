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
#include "tAdminLua.h"

#undef double
#define double double

#include <Timer.h>
#include <getTime.h>
#include <WThread.h>
#include <dprint.h>

// Global apps event FIFO
CFifo GlEvFifo;

extern "C" {
  void daemon_start(int);
} // extern "C"

uChar GlAppsFlag = '\0';
CHKContext* pHKContext = NULL;

CAppTimer appTimer;
//static short weekMinute;
//time_t globCurentTime;

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

// Wrapper for STDOUT & STDERR
const char* outNull = "/dev/null";

class CSysLog {
public:
  CSysLog(const char* of=outNull)  {
    if(!(GlAppsFlag & LOG_IN_STDERR)) {
      ::openlog("Its", LOG_CONS | LOG_PID, /*LOG_USER*/LOG_DAEMON);
    } else if(GlAppsFlag & DAEMON_MODE) {
      close(0); close(1); close(2);
      int   fin =  open(outNull, O_RDONLY);
      FILE* fof = fopen(of,      "w+");
      int r = dup(1);
      LOG(L_INFO, "CSysLog constructor %s %d\n", fof ? "Ok":"Err", r);
      close(fin);
    }
  }
  ~CSysLog() {
    if(!(GlAppsFlag & LOG_IN_STDERR)) {
      ::closelog();
    } else if(GlAppsFlag & DAEMON_MODE) {
      fclose(stdout);
      close(2);
    }
  }
};

char appsHomeArr[320];
const char* appsHome = NULL;

// Admin interpreter
extern int  tolua_tAdminLua_open( lua_State* pLua );

extern "C" {
  int luaopen_socket_core(lua_State *L);
  int luaopen_lsqlite3(lua_State *L);
  //int luaopen_libRex_pcre(lua_State *L);
}

// ====================================

int main(int argc,char* argv[]) {
  int rrr = 0;
  const char* outFl = NULL;
  GlAppsFlag |= LOG_IN_STDERR;

  while(argc > 1) {
    if(*argv[1] == '-') {
      char* pArg = argv[1] + 1;
      if(!strcmp(pArg,"D")) {
        DBG("***> Run as a deamon\n");
        GlAppsFlag |= DAEMON_MODE;

        argv = &argv[1];  --argc;
        continue;
      }
      if(!strcmp(pArg,"slog")) {
        DBG("***> Use syslog\n");
        GlAppsFlag &= ~LOG_IN_STDERR;

        argv = &argv[1];  --argc;
        continue;
      }
      if(!strcmp(pArg,"f")) {
        // File to print LOG
        outFl = argv[2];

        argv  = &argv[2];  argc -= 2;
        continue;
      }
      if(!strcmp(pArg,"d")) {
        // Set debug level (0-4), default - 4
        dbgLevel = *argv[2] - '0';
        DBG("***> debug level %u\n", dbgLevel);

        argv = &argv[2];  argc -= 2;
        continue;
      }
      LOG(L_ERR,"Main: Bad option \"%s\"\n",argv[1]);
      return -2;
    }
  }
  {
    appsHome = getenv("ITS_APP_HOME");
    if(appsHome)
      strcpy(appsHomeArr,appsHome);
    else
      appsHome = getcwd(appsHomeArr,sizeof(appsHomeArr));
    LOG(L_WARN,"Main: Application HOME <%s>\n",(char*)appsHomeArr);
    strcat(appsHomeArr,"/lua/");
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
#if 0  // Timer test
  {
    uLong ntt = 0u;
    uLong Ttx = 0u;
    uLong tTm = tNow();
    DBG(">Main> Timer test Tm=%lu\n",tTm);

    tTm = tNow() + TimeQuant;

    while(Ttx < tTm) {
      Ttx = tNow();
      ++ntt;
    }
    DBG(">Main> Timer test Tm=%lu-%lu   N=%lu\n",tTm,Ttx,ntt);

    DBG(">Main> Timer genau Tm=%lu\n",tNow());
    nsleep(314);  DBG(">Main> Timer genau 314 Tm=%lu\n",tNow());
    nsleep(79);   DBG(">Main> Timer genau 79 Tm=%lu\n",tNow());
    nsleep(7221); DBG(">Main> Timer genau 7221 Tm=%lu\n",tNow());
    nsleep(7);    DBG(">Main> Timer genau 7 Tm=%lu\n",tNow());
    nsleep(713);  DBG(">Main> Timer genau 713 Tm=%lu\n",tNow());
    nsleep(113);  DBG(">Main> Timer genau 113 Tm=%lu\n",tNow());
    nsleep(1713); DBG(">Main> Timer genau 1713 Tm=%lu\n",tNow());
    nsleep(3013); DBG(">Main> Timer genau 3013 Tm=%lu\n",tNow());
    nsleep(17);   DBG(">Main> Timer genau 17 Tm=%lu\n",tNow());
  }
#endif // Timer test
  {
    // start SysLog interface
    CSysLog _AppSysLog(outFl);
    pHKContext = new CHKContext(CContext::reservId());
    pHKContext->hashCId();
    // Starting => everything wait until unlock
    GlEvFifo.getCond().lock();
    {
      // Start timer thread
      CTmThrd* pTmThrd = new CTmThrd;
      pTmThrd->start();
    }
    {
      lua_State* L = lua_open();
      luaL_openlibs(L);
      luaopen_socket_core(L);
      luaopen_lsqlite3(L);
      //luaopen_libRex_pcre(L); 
      tolua_tAdminLua_open(L);
      strcpy((char*)appsHome,"admin.lua");
      DBG("*** Start admin-LUA script <%s> ***\n\n",appsHomeArr);

      rrr = luaL_dofile(L,appsHomeArr);
      lua_close(L);
    }
  }
  LOG(L_NOTICE,"Main: Exit OK>%d<\n",rrr);
  return rrr;
}

// $Id: main.cpp 381 2010-05-10 20:16:27Z asus $
