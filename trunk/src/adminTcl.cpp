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
#include <dprint.h>
#include <WThread.h>
#include <adminTcl.h>
#include <tstSigThrd.h>

// TCL-if to signal threads
int CSigThrd::cmdProc(int argc,Tcl_Obj* const argv[]) {
  int res = 0;
  CThreadObj* p= NULL;
  pParam(argc, argv);
  const char* sCmd = interp->tGetVal(argv[1]);
  switch(*(sCmd++)) {
    case 's': // start sig. thread
      if(!strcmp("tart",sCmd) && (argc >= 3)) {
	// Sign. thread to start
        const char* sThId = interp->tGetVal(argv[2]);
	switch(*(sThId++)) {
	  case 'x': // xtst - start tstSigThrd1
	    if(!strcmp("tst",sThId) && (argc == 7)) {
	      TGVal(int,m0,argv[3]);
	      TGVal(int,m1,argv[4]);
	      TGVal(short,t0,argv[5]);
	      TGVal(short,t1,argv[6]);
	      p = new CTstSgTh(m0,m1,t0,t1);
	      res = p ? p->getTId() : 0;
	    }
	    break;
	  case 'a': // atst - start tstSigThrd1
	    if(!strcmp("tst",sThId) && (argc == 5)) {
	      TGVal(int,m0,argv[3]);
	      TGVal(int,m1,argv[4]);
	      p = new CTstSgThX(m0,m1);
	      res = p ? p->getTId() : 0;
	    }
	    break;
	  default :
	    LOG(L_ERR,"CWrkThrd::cmdProc Error <%s>\n",sCmd);
	    pParam(argc, argv);
	}
	tSetResult(tSetObj(res));
      }
      break;
    default :
      LOG(L_ERR,"CSigThrd::cmdProc Error <%s>\n",sCmd);
      pParam(argc, argv);
  }
  return TCL_OK;
}

// TCL-if to work threads
int CWrkThrd::cmdProc(int argc,Tcl_Obj* const argv[]) {
  int ret = 0;
  CThreadObj* p= NULL;
  //pParam(argc, argv);
  const char* sCmd = interp->tGetVal(argv[1]);
  switch(*(sCmd++)) {
    case 's': // start wrk. thread
      if(!strcmp("tart",sCmd) && (argc == 2)) {
	for(uChar k=0x0; k<MAX_N_WTHREAD; ++k) {
	  if(!pWrkThArr[k]) {
	    p = new CWThread(k);
	    DBG("CWrkThrd::cmdProc> Start %d WrkThread\n",k+1);
	    // return thread ID
	    ret = p ? k+1 : 0;
	    tSetResult(tSetObj(ret));
	    break;
	  }
	}
      }
      break;
    case 'o': // off - stop N wrk. thread
      if(!strcmp("ff",sCmd) && (argc == 3)) {
	short k, N = 0;
	short nt = (short)CWThread::nRThrd();
	(void)interp->tGetVal(argv[2], N);
	nt -= N;
	CWThread::wStop();
	DBG("CWrkThrd::cmdProc> Stop %d WrkThread\n",N);
	for(k=0x0; k<N; ++k) {
	  CEvent* ev = CEvent::newEv(0u,Evnt_wThrdEnd);
	  ev->put();
	  ev->sign();
	  yield();
	  DBG("CWrkThrd::cmdProc> Stop WrkThread %d\n",CWThread::nRThrd());
	}
	while(nt != (short)CWThread::nRThrd())
	  yield();
	tSetResult(tSetObj(nt));
	CWThread::wStart();
      }
      break;
    case 'r': // rload TCL script (for all Wrk threads)
      if(!strcmp("load",sCmd) && (argc == 2)) {
	uChar k = 0x0;
	CWThread::reloadTcl();
	DBG("CWrkThrd::cmdProc> Reload WrkTcl-Script\n");
	for(k=0x0; k<CWThread::nRThrd(); ++k) {
	  CEvent* ev = CEvent::newEv(0u,Evnt_wThrdEnd);
	  ev->put('x');
	  ev->sign();
	  yield();
	}
	tSetResult(tSetObj((short)k));
      }
      break;
    case 'n': // num - return num of running wrk. thread
      if(!strcmp("um",sCmd) && (argc == 2)) {
	tSetResult(tSetObj(CWThread::nRThrd()));
      }
      break;
    default :
      LOG(L_ERR,"CWrkThrd::cmdProc Error <%s>\n",sCmd);
      pParam(argc, argv);
  }
  return TCL_OK;
}

extern char GlobEvThreadStoped;

// new TCL commant for coomon Apps taks - <apps>
extern uInt HkCId;
int CComApp::cmdProc(int argc,Tcl_Obj* const argv[]) {
  pParam(argc, argv);
  const char* sCmd = interp->tGetVal(argv[1]);
  switch(*(sCmd++)) {
    case 's': // start all
      if(!strcmp("tart",sCmd) && (argc == 2))
	(void)GlEvFifo.getCond().unlock();
      break;
    case 'x': // xstat - start/stop work thread statistik
      if(!strcmp("stat",sCmd) && (argc == 3)) {
	char* res = NULL;
	// work thread statistik - Start / Stop
	const char* prm = interp->tGetVal(argv[2]);
	if(!strcmp("on",prm)) {
	  res = (char*)"Run";
	  CWThread::gWrkStat('x');
	} else if(!strcmp("off",prm)) {
	  res = (char*)"Stop";
	  CWThread::gWrkStat(0x0);
	}
	tSetResult(tSetObj(res));
      }
      break;
    case 't': // tout tOur Type shortIi - set new timeout
      if(!strcmp("out",sCmd) && (argc == 6)) {
	TGVal(int,tm,argv[2]);
	TGVal(short,tm_type,argv[3]);
	TGVal(int,CId,argv[4]);
	TGVal(short,sIi,argv[5]);
	tmQueue.set(tm,tm_type,CId,sIi);
      }
      break;
    case 'b': // break all
      if(!strcmp("reak",sCmd) && (argc == 2)) {
	int k = (GlEvFifo.getCond()).lock();
	tSetResult(tSetObj(k));
      }
      break;
    case 'g': // goff - all sign
      if(!strcmp("off",sCmd) && (argc == 2)) {
	GlAppsFlag |= STOP_EV_THRD;
	for(uChar k=0x0; k<MAX_N_STHREAD; ++k) {
	  if(g_evThrdArr[k]) {
	    DBG("CComApp::cmdProc> Stop sign.thread %s\n",g_evThrdArr[k]->thName());
	    g_evThrdArr[k]->stop();
	  }
	}
	GlobEvThreadStoped = 'x';
      }
      break;
    case 'e': // endNC  - stop new context factory
      if(!strcmp("ndNC",sCmd) && (argc == 3)) {
	TGVal(char,nn,argv[2]);
	stopNewCtxt = nn ? 'x' : '\0';
	DBG("CComApp::cmdProc> Stop context factory %d\n",nn);
	tSetResult(tSetObj((char*)(nn ? "Off" : "On")));
      }
      break;
    case 'h': // halt - running context's
      if(!strcmp("alt",sCmd) && (argc == 2)) {
	// Delete all context from hashCntxt
	CCnxtMap::iterator I = hashCntxt.begin();
	while(I != hashCntxt.end()) {
	  uInt cid     = (*I).first;
	  CContext* pc = (*I).second;
	  if(pc) {
	    DBG("CComApp::cmdProc Halt context[%u]\n",pc->getId());
	    pc->send(Evnt_PreDelCtxt,cid);
	  }
	  ++I;
	}
      }
      break;
    case 'w': // woff - exit all work threads & timer
      if(!strcmp("off",sCmd) && (argc == 2)) {
	GlAppsFlag |= APPL_EXIT;  // Stop timer
	CWThread::wStop();
	CWThread::reloadTcl();
	for(uChar k=0u; k<MAX_N_WTHREAD; ++k) {
	  DBG("CComApp::cmdProc woff %d\n",CWThread::nRThrd());
	  CEvent* ev = CEvent::newEv(0u,Evnt_wThrdEnd);
	  ev->put();
	  ev->sign();
	}
      }
      break;
    default :
      LOG(L_ERR,"CSigThrd::cmdProc Error <%s>\n",sCmd);
      pParam(argc, argv);
  }
  return TCL_OK;
}

// =================================

// init Custom Admin commands
char CAdmTcl::Init() {
  // Clean all pointer to wrkThreads
  for(uChar k='\0'; k<MAX_N_WTHREAD; ++k)
    pWrkThArr[k] = NULL;
  // include Sqlite commands
  (void)Sqlite3_Init(getInterp());
  // init new commands
  new CSigThrd(this);
  new CWrkThrd(this);
  new CComApp(this);
  return 0x0;
}

// $Log$
