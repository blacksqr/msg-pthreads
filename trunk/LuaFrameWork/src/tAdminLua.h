#ifndef tAdminLua_h
#define tAdminLua_h
extern "C" {
 #include <lualib.h>
 #include <lauxlib.h>
}

void ansleep(int timeOut);

// Start signaling threads
void tstSgThX(int m1, int m2);
void tstSgTh(int m1, int m2, int t1, int t2);
void tstFsmTh(int m);
// Manage work threads
int startWrkTh();
int stopWrkTh();
int rLoadScrpt();
int numRunWTh();
// Common application tasks
void aStart();
int  aStop();
// Set new HauseKeep timeout
void checkTOut(int tm, short tm_type, short sIi);
void stopAllSgTh();
// Stop new context factory
const char* stopCFactory(int n=0);
// Delete all context from hashCntxt
void ctxtHashClean();
// Exit all work threads & timer
void preExitApps();

#endif // tAdminLua_h

// $Id: tAdminLua.h 323 2010-01-17 20:29:38Z asus $
