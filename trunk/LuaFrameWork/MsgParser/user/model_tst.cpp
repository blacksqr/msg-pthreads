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
#include <factory_myFctr1.h>

using namespace MsgModel;

char dmp[0x80000];
char buff[0x80000];

template<typename T> T& r_(T x) { static T z = x; return z; }

const char* tstMsg1 = "Msg { msgId = 1; hdr = Rc { nm = \"my_msg1_HdrS\"; id = 31415; bmHdr = BM14_FE43; }; body = Rc { myt = AR8 [ 0=>St { fT03 = V3 [ 2.11, 2.22, 2.33 ]; iT03 = FV3 [ 11, -111, -71 ]; ii = 11; bmTst02 = BM12_ABC3; }, 2=>St { fT03 = V2 [ 3.11, 3.22 ]; iT03 = FV3 [ 21, 22, -21 ]; ii = 22; bmTst02 = BM12_ABC3; }, 3=>St { fT03 = V3 [ 4.11, 4.22, 4.33 ]; iT03 = FV3 [ 21, 22, -21 ]; ii = 33; bmTst02 = BM12_ABC3; }, 5=>St { fT03 = V3 [ 2.11, 2.22, 2.33 ]; iT03 = FV3 [ 35, 55, -51 ]; ii = 44; bmTst02 = BM12_73F; }, 6=>St { fT03 = V4 [ 6.11, 6.22, 6.33, 6.66 ]; iT03 = FV3 [ -21, 222, -21 ]; ii = 77; bmTst02 = BM12_AB4; } ]; iTst03 = AR8 [ 1=>11, 2=>21, 3=>31, 5=>41, 6=>66 ]; nn = 31415; m = 21; sf06 = \"Msg1_BdStrSf06_1\"; }; };";


const char* tstMsg2 = "Msg { msgId = 2; hdr = Rc { nmid = 7654; id = 321; }; body = St { fT03 = V4 [ 55.55, 995.e-2, 0.065E2, 21.1e5 ]; iT03 = FV3 [ 5563, 995, 0065 ]; ii = 5443; bmTst02 = BM12_AABB; }; };";

// operator << (((char*&)(& pcr)), ((MsgModel::CMsgGen_mymsg1&)(& mm1)));
//char*& operator << (char*& b, CMsgGen_mymsg1& m) { return m.store(b); }

int main() {
  char rr = '\0';
  char* pcr = &buff[0];
  PMessage pMsg = NULL;

  CFctryGen_myFctr1 mFactory;

  CMsgGen_mymsg1 mm1;
  CMsgGen_mymsg2 mm2;
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

  char* pcx = buff;

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

  printf("\n\t**** PERF TEST 30 * 50000 START ****\n\n");

  for(uInt2 ki=0; ki<50000; ++ki) {

    char* pcx = buff;

    for(uInt1 k=0; k<30; ++k) {

      pMsg = mFactory.reStore(pcx);

      switch(pMsg->getMsgId()) {
	case 1: {
	  CMsgGen_mymsg1* pMm1 = (CMsgGen_mymsg1*)pMsg;
	  rr = pMm1->parse0();
	  rr = pMm1->parse1();
	  pMm1->dump(&dmp[0]);
	  if(ki == (50000-1))
	    printf("CMsgGen_mymsg1 dump:\n%s\n", &dmp[0]);
	  break;
	}
	case 2: {
	  CMsgGen_mymsg2* pMm2 = (CMsgGen_mymsg2*)pMsg;
	  rr = pMm2->parse0();
	  rr = pMm2->parse1();
	  pMm2->dump(&dmp[0]);
	  if(ki == (50000-1))
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

  printf("\n\t**** PERF TEST 50000 END ****\n\n");

  return 0;
}

// $Id: model_tst.cpp 350 2010-02-08 18:38:31Z asus $
