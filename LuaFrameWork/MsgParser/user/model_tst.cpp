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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <unistd.h>
#include <factory_myFctr1.h>

using namespace MsgModel;

char dmp[0x20000];
char buff[0x20000];

template<typename T> T& r_(T x) { static T z = x; return z; }

const char* tstMsg1 = "Msg { msgId = 1; hdr = Rc { nm = \"my_msg1_HdrS\"; id = 31415; bmHdr = BM14_FE43; }; body = St { myt = AR8 [ 0=>St { fT03 = V3 [ 2.11, 2.22, 2.33 ]; iT03 = FV3 [ 11, -111, -71 ]; ii = 11; bmTst02 = BM12_ABC3; }, 2=>St { fT03 = V2 [ 3.11, 3.22 ]; iT03 = FV3 [ 21, 22, -21 ]; ii = 22; bmTst02 = BM12_ABC3; }, 3=>St { fT03 = V3 [ 4.11, 4.22, 4.33 ]; iT03 = FV3 [ 21, 22, -21 ]; ii = 33; bmTst02 = BM12_ABC3; }, 5=>St { fT03 = V3 [ 2.11, 2.22, 2.33 ]; iT03 = FV3 [ 35, 55, -51 ]; ii = 44; bmTst02 = BM12_73F; }, 6=>St { fT03 = V4 [ 6.11, 6.22, 6.33, 6.66 ]; iT03 = FV3 [ -21, 222, -21 ]; ii = 77; bmTst02 = BM12_AB4; } ]; iTst03 = AR8 [ 1=>11, 2=>21, 3=>31, 5=>41, 6=>66 ]; nn = 31415; m = 21; un0 = Un { hh1 = Rc { nm = \"my_str_union\"; id = 4321; bmHdr = BM14_0FF0; }; }; sf06 = \"Msg1_BdStrSf06_1\"; }; };";


const char* tstMsg2 = "Msg { msgId = 2; hdr = Rc { nmid = 7654; id = 321; }; body = St { fT03 = V4 [ 55.55, 995.e-2, 0.065E2, 21.1e5 ]; iT03 = FV3 [ 5563, 995, 0065 ]; ii = 5443; bmTst02 = BM12_AABB; }; };";

// operator << (((char*&)(& pcr)), ((MsgModel::CMsgGen_mymsg1&)(& mm1)));
//char*& operator << (char*& b, CMsgGen_mymsg1& m) { return m.store(b); }

template<typename T> class TMsgWrap {
private:
  T* pp;
public:
  TMsgWrap(T* p) : pp(p) {}
  ~TMsgWrap() { delete pp; }

  T& operator () () { return *pp; }
};

int main() {
  char rr = '\0';
  char* pcr = &buff[0];
  PMessage pMsg = NULL;

  CFctryGen_myFctr1 mFactory;

#if 0
  CMsgGen_mymsg1 mm1;
  CMsgGen_mymsg2 mm2;
#else
  TMsgWrap<CMsgGen_mymsg1> pm1(new CMsgGen_mymsg1);
  TMsgWrap<CMsgGen_mymsg2> pm2(new CMsgGen_mymsg2);
  CMsgGen_mymsg1& mm1  = pm1();
  CMsgGen_mymsg2& mm2  = pm2();
#endif // 0

  mm1.schema(&buff[0]);
  printf("Schema of CMsgGen_mymsg1:\n%s\n", &buff[0]);
  mm2.schema(&buff[0]);
  printf("Schema of CMsgGen_mymsg2:\n%s\n", &buff[0]);

  printf("\n\t**** MESSAGES FROM_STR METHOD ****\n\n");

  mm1.fromStr(strdup(tstMsg1));
  mm1.dump(&dmp[0]);
  printf("CMsgGen_mymsg1 dump:\n%s\n", &dmp[0]);
  mm2.fromStr(strdup(tstMsg2));
  mm2.dump(&dmp[0]);
  printf("CMsgGen_mymsg2 dump:\n%s\n", &dmp[0]);

  printf("\n\t**** STORE-RESTORE MESSAGES ****\n\n");

  for(uInt1 k=0; k<100; ++k) {
    pcr = mm1.store(pcr);
    pcr = mm2.store(pcr);
    pcr = mm2.store(pcr);
    pcr = mm1.store(pcr);
    pcr = mm2.store(pcr);
    pcr = mm1.store(pcr);
    pcr = mm1.store(pcr);
    pcr = mm2.store(pcr);
    pcr = mm1.store(pcr);
    pcr = mm2.store(pcr);
    pcr = mm1.store(pcr);
    pcr = mm1.store(pcr);
    pcr = mm1.store(pcr);
    pcr = mm1.store(pcr);
    pcr = mm2.store(pcr);
    pcr = mm1.store(pcr);
    pcr = mm2.store(pcr);
    pcr = mm1.store(pcr);
    pcr = mm1.store(pcr);
    pcr = mm2.store(pcr);
    pcr = mm1.store(pcr);
    pcr = mm2.store(pcr);
    pcr = mm1.store(pcr);
    pcr = mm1.store(pcr);
    pcr = mm2.store(pcr);
    pcr = mm1.store(pcr);
    pcr = mm2.store(pcr);
    pcr = mm1.store(pcr);
    pcr = mm1.store(pcr);
    pcr = mm2.store(pcr);
    pcr = mm1.store(pcr);
    pcr = mm2.store(pcr);
    pcr = mm1.store(pcr);
  }

  char* pcx = buff;
  uInt2 lx = pcr - pcx;
  printf("Buffer length %u\n", lx);

  int fd = open("/tmp/msg_store.bin", O_RDWR|O_CREAT|O_TRUNC);
  lx = write(fd, pcx, lx+1);
  close(fd);
  printf("Written buffer length %u\n", lx);
  sleep(5);

  for(uInt1 k=0; k<30; ++k) {

    pMsg = mFactory.reStore(pcx);
    printf("MsgFactory restore MsgId %u\n", pMsg->getMsgId());

    switch(pMsg->getMsgId()) {
      case 1: {
	CMsgGen_mymsg1* pMm1 = (CMsgGen_mymsg1*)pMsg;
	rr = pMm1->parse0();
	rr = pMm1->parse1();
	pMm1->dump(&dmp[0]);
	printf("CMsgGen_mymsg1 dump:\n%s\n", &dmp[0]);
	break;
      }
      case 2: {
	CMsgGen_mymsg2* pMm2 = (CMsgGen_mymsg2*)pMsg;
	rr = pMm2->parse0();
	rr = pMm2->parse1();
	pMm2->dump(&dmp[0]);
	printf("CMsgGen_mymsg2 dump:\n%s\n", &dmp[0]);
	break;
      }
      default: {
	printf("Error - unknown MsgId\n");
      }
    }

    delete pMsg;
    pMsg = NULL;

  }

  const uInt4 NZIKL = 10000;
  printf("\n\t**** PERF TEST 30 * %u START ****\n\n", NZIKL);

  for(uInt2 ki=0; ki<(NZIKL+1); ++ki) {

    char* pcx = buff;

    for(uInt1 k=0; k<30; ++k) {

      pMsg = mFactory.reStore(pcx);

      switch(pMsg->getMsgId()) {
	case 1: {
	  CMsgGen_mymsg1* pMm1 = (CMsgGen_mymsg1*)pMsg;
	  rr = pMm1->parse0();
	  rr = pMm1->parse1();
	  pMm1->dump(&dmp[0]);
	  if((ki == NZIKL) && !k)
	    printf("CMsgGen_mymsg1 dump:\n%s\n", &dmp[0]);
	  break;
	}
	case 2: {
	  CMsgGen_mymsg2* pMm2 = (CMsgGen_mymsg2*)pMsg;
	  rr = pMm2->parse0();
	  rr = pMm2->parse1();
	  pMm2->dump(&dmp[0]);
	  if((ki == NZIKL) && (k == 1))
	    printf("CMsgGen_mymsg2 dump:\n%s\n", &dmp[0]);
	  break;
	}
	default: {
	  printf("Error - unknown MsgId\n");
	}
      }
      delete pMsg;
      pMsg = NULL;

    }
  }
  printf("\n\t**** PERF TEST %u END ****\n\n", NZIKL);
  return 0;
}

// $Id: model_tst.cpp 385 2010-05-15 15:12:24Z asus $
