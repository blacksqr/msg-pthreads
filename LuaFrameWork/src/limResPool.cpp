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

// Limited resource pool
// For example TCP-Socket
class CRsrce {
protected:
  FFInd i;
  uChar flg; // 0x0 - free
public:
  CRsrce() {}
  ~CRsrce() {}
  void sFree() { flg = 0x0; }
  FFInd gInd() { return i; }
  void sInd(FFInd k) { i = k; }
};
typedef CRsrce* pCRsrce;

class CResPool {
#define nFf ((beg <= end) ? (end - beg) : (beg + (FfQueSize - end)))
protected:
  const pCRsrce pObj[FfQueSize];
  FFInd         indAr[FfQueSize];
  FFInd beg, end, Nn;
public:
  CResPool(pCRsrce objAr, uChar n) {
    beg = 0x00;
    Nn = end = n;
    memset(indAr,0,sizeof(indAr));
    for(FFInd k=0x0; k<Nn; ++k) {
      objAr[k].sInd(k);
      pObj[k]  = &objAr[k];
      indAr[k] = k;
    }
  }
  ~CResPool() {
  }
  FFInd putFree(CRsrce p) {
    p->sFree();
    pObj[end++] = p;
    indAr[] = ;
    return end;
  }
  FFInd putUse(CRsrce p) {
    pObj[--beg] = p;
    return beg;
  }
  CRsrce getFree() {
    return pObj[beg++];
  }
  CRsrce getUse(FFInd i) {
    // main problem to get element from mitte
    pCRsrce pp = 
    
    //return pObj[beg++];
  }
  FFInd getNN() { return nFf; }
};
