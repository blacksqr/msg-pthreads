#ifndef H_ICS_LIZENS
#define H_ICS_LIZENS

#include <ItsCcb.h>
#include <ItsLizens.h>

char CLizensMng::nProv;
CLizensMng* pGlobLizMNG = NULL;

CLizensMng::CLizensMng(long From,long To,char* SwName,long p,long s) {
  nProv = 3;
  lk1 = p; lk2 = s;
  startDate = From; endDate = To;
  SwitchName = strdup(SwName);
  getCardInfo();
}

void CLizensMng::getCardInfo() {
  char flag,*pc = NULL;
  char *bb = new char[512];
  *bb = '\0';
  for(short k=0; k<nAclPortInSys; k++) {
    ACL_SYSINFO sysInfo;
    sysInfo.net = k;
    .................................
    call_get_card_info ..............
    call_port_info ..................
    (void)call_system_info(&sysInfo);
    flag = 'x';
    if(pc) {
      flag = (strcmp(pc,sysInfo.serialnumber)) ? 'x' : '\0';
      free(pc); pc = NULL;
    }
    pc = strdup(sysInfo.serialnumber);
    if(flag) {
      strcat(bb,sysInfo.serialnumber);
      strcat(bb,":");
    }
  }
  if(pc)
    free(pc);
  cardSerNumStr = strdup(bb);
  delete bb;
}

void CLizensMng::calcCRC(char* string,short len) {
  static uChar z[8] = {0,4,2,6,1,5,3,7};
  uChar k, *p;
  uLLong tt = 8 * 8589934591ll - 3;
  uLLong pin =  143;
  pin *= tt;
  p = (uChar*)string;
  for(k='\0'; k<len; k+=2) {
    pin ^= p[k];
    pin = (pin<<3) | ((uLLong)z[pin>>61]);
  }
  p = (uChar*)string;
  for(k='\1'; k<len; k+=2) {
    pin ^= p[k];
    pin = (pin<<3) | ((uLLong)z[pin>>61]);
  }
  pin *= tt;
  long ll1 = (pin>>2)%100000000ll;
  pin *= tt;
  long ll2 = (pin>>2)%100000000ll;
  lizKey = (uLLong)ll1 * ll2;
  --nProv;
#ifdef SOFT_OWNER
  printf("\n\tpref-%ld suff-%ld\n\n",ll1,ll2);
#endif // SOFT_OWNER
}

bool validateApp() {
  if(pGlobLizMNG->getInst())
    return pGlobLizMNG->checkIt();
  if(pGlobLizMNG) {
    delete pGlobLizMNG;
    pGlobLizMNG = NULL;
  }
  return true;
}

void validateApp(bool& val) {
  if(pGlobLizMNG->getInst())
    val = pGlobLizMNG->checkIt();
  else
    if(pGlobLizMNG) {
      delete pGlobLizMNG;
      pGlobLizMNG = NULL;
    }
    val = true;
  return;
}

#endif // H_ICS_LIZENS
// $Id: ItsLizens.cpp 297 2010-01-08 20:41:13Z asus $
