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
#ifndef MSG_PARSER_H
#define MSG_PARSER_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <ModelConst.h>

namespace MsgModel {

// typedef's
typedef char*              pChar;
typedef void*              pVoid;

typedef unsigned char      uInt1;
typedef          char      sInt1;
typedef unsigned short     uInt2;
typedef          short     sInt2;
typedef unsigned int       uInt4;
typedef          int       sInt4;
typedef unsigned long long uInt8;
typedef          long long sInt8;

typedef          float     Float4;
typedef          double    Float8;

#ifdef DIFF_HARDWARE
// Functions forward declaration
uInt1  n2h(uInt1  v);
Float4 n2h(Float4 v);
Float8 n2h(Float8 v);
uInt2  n2h(uInt2  v);
uInt4  n2h(uInt4  v);
uInt8  n2h(uInt8  v);

uInt1  h2n(uInt1  v);
Float4 h2n(Float4 v);
Float8 h2n(Float8 v);
uInt2  h2n(uInt2  v);
uInt4  h2n(uInt4  v);
uInt8  h2n(uInt8  v);
#else
#define n2h(x) x
#define h2n(x) x
#endif // DIFF_HARDWARE


// To get array size
#define arrLen(arr) (sizeof(arr)/sizeof(*arr))
//template<typename T,int size> uInt2 arrLen(T(&)[size]) {return size;}

#ifdef USE_DUMP

#include <pcreposix.h>

class CRegExp {
  // disable copy constructor.
  CRegExp(const CRegExp&);
  void operator = (const CRegExp&);

 protected:
  regex_t     re;
  regmatch_t* pmatch;
  char*       str;
  uInt1       ss;
 public:
  CRegExp(const char* expr,uInt1 x);
  virtual ~CRegExp();

  int   exe(char* s);
  char* operator () (uInt1 x, uInt2* o=NULL);
  char* end(uInt1 x);
  regmatch_t& match(uInt1 k) { return pmatch[k]; }
};

static const char* hhx = "0123456789ABCDEF";
extern char* setDefOffset_(uInt1 n);
#define _prnDmpOffSet setDefOffset_(of)

// Forward declaration
void a2i(uInt1&, char*);
void a2i(uInt2&, char*);
void a2i(uInt4&, char*);
void a2i(uInt8&, char*);

void a2i(sInt1&, char*);
void a2i(sInt2&, char*);
void a2i(sInt4&, char*);
void a2i(sInt8&, char*);

void a2f(Float4&, char*);
void a2f(Float8&, char*);
#endif // USE_DUMP

//===========================================================

// Template for the basic data types - integer
template<typename T> class TIField {
  // disable copy constructor.
  TIField(const TIField&);
  void operator = (const TIField&);

 protected:
  T var;
 public:
 TIField(T v=0): var(v) {}
  // Reference to Pointer
  TIField(char*& buff) { buff = reStore(buff); }
  ~TIField() {}

  T get() const          { return var; }
  T operator () () const { return var; }
  void set(T& v) { var = v; }

  char* reStore(char* buff) {
    T tmp;
    memcpy(&tmp, buff, sizeof(T));
    var = n2h(tmp);
    return (buff + sizeof(T));
  }
  char* store(char* buff) const {
    T tmp = h2n(var);
    memcpy(buff, &tmp, sizeof(T));
    return (buff + sizeof(T));
  }
#ifdef USE_DUMP
  char* fromStr(char* bf) {
    CRegExp mi("^([+-]?[0-9]+)([;,]? |;)",0x2);
    if(!mi.exe(bf)) {
      char* b = mi(0x1);
      *(mi.end(0x1)) = '\0';
      a2i(var,b);
      return mi.end(0x0);
    }
    return bf;
  }
  char* dump(char* bf,uInt1 of=0u) const {
    sInt8 x = var;
    int k =  ::sprintf(bf, "%lld", x);
    //int k =  ::sprintf(bf, " 0x%X", var);
    return (bf + k);
  }
  char* schema(char* bf,uInt1 of=0u) const {
    strcpy(bf, "Integer"); bf += 7;
    return bf;
  }
#endif // USE_DUMP
};
typedef TIField<uInt1>  CFieldUI1;
typedef TIField<uInt2>  CFieldUI2;
typedef TIField<uInt4>  CFieldUI4;
typedef TIField<uInt8>  CFieldUI8;
typedef TIField<sInt1>  CFieldSI1;
typedef TIField<sInt2>  CFieldSI2;
typedef TIField<sInt4>  CFieldSI4;
typedef TIField<sInt8>  CFieldSI8;

//===========================================================

// Template for the basic data types - float
template<typename T> class TFField {
  // disable copy constructor.
  TFField(const TFField&);
  void operator = (const TFField&);

 protected:
  T var;
 public:
 TFField(T v=0): var(v) {}
  // Reference to Pointer
  TFField(char*& buff) { buff = reStore(buff); }
  ~TFField() {}

  T get() const          { return var; }
  T operator () () const { return var; }
  void set(T& v) { var = v; }

  char* reStore(char* buff) {
    T tmp;
    memcpy(&tmp, buff, sizeof(T));
    var = n2h(tmp);
    return (buff + sizeof(T));
  }
  char* store(char* buff) const {
    T tmp = n2h(var);
    memcpy(buff, &tmp, sizeof(T));
    return (buff + sizeof(T));
  }
#ifdef USE_DUMP
  char* fromStr(char* bf) {
    CRegExp mi("^([+-]?[0-9eE+-.]+)([;,]? |;)",0x2);
    if(!mi.exe(bf)) {
      char* b = mi(0x1);
      *(mi.end(0x1)) = '\0';
      a2f(var,b);
      return mi.end(0x0);
    }
    return bf;
  }
  char* dump(char* bf,uInt1 of=0u) const {
    int k =  ::sprintf(bf, "%g", var);
    return (bf + k);
  }
  char* schema(char* bf,uInt1 of=0u) const {
    strcpy(bf, "Float"); bf += 5;
    return bf;
  }
#endif // USE_DUMP
};
typedef TFField<Float4> CFieldFI4;
typedef TFField<Float8> CFieldFI8;

//===========================================================

// Classical C-string
class CString {
  // disable copy constructor.
  CString(const CString&);
  void operator = (const CString&);

 protected:
  char* str;
 public:
 CString(char* s=NULL): str(s) {}
  CString(char*& buff) { buff = reStore(buff); }
  ~CString() { if(str) free(str); }

  char* get() const          { return str; }
  char* operator () () const { return str; }
  // it takes memory owners chip
  void set(char* v) { str = v; }

  char* reStore(char* buff) {
    CFieldUI2 ui(buff);
    str = (char*)malloc(ui.get() + 1);
    memcpy(str, buff, ui.get());
    str[ui.get() + 1] = '\0';
    return (buff + ui.get());
  }
  char* store(char* buff) const {
    uInt2 l = 0u;
    CFieldUI2 ui;
    char* s= str;
    char* b = buff;
    buff += 2;
    while(*s) {
      *(buff++) = *(s++); ++l;
    }
    ui.set(l);
    ui.store(b);
    return buff;
  }
#ifdef USE_DUMP
  char* fromStr(char* bf) {
    CRegExp mi("^\"([^\"]*)\"([;,]? |;)",0x2);
    if(!mi.exe(bf)) {
      char* b = mi(0x1);
      *(mi.end(0x1)) = '\0';
      if(str) free(str);
      str = strdup(b);
      return (mi.end(0x2));
    }
    return bf;
  }
  char* dump(char* bf,uInt1 of=0u) const {
    int k =  ::sprintf(bf, "\"%s\"", str ? str : "<null>") - 1;
    return (bf + k);
  }
  char* schema(char* bf,uInt1 of=0u) const {
    strcpy(bf, "\"String\""); bf += 8;
    return bf;
  }
#endif // USE_DUMP
};

//===========================================================

// String as the <char array of fix length>
template<uInt2 Kn> class TFString {
  // disable copy constructor.
  TFString(const TFString&);
  void operator = (const TFString&);

 protected:
  char str[Kn];
 public:
  TFString(char* s=NULL) { set(s); }
  TFString(char*& buff)  { buff = reStore(buff); }
  ~TFString() {}

  char* get() const          { return str; }
  char* operator () () const { return str; }
  uInt2 len() const { return Kn; }
  void set(char* s) {
    if(s) {
      memcpy(str, s, Kn);
    } else
      str[0] = '\0';  // To avoid error
  }

  char* reStore(char* buff) {
    memcpy(str, buff, Kn);
    return (buff + Kn);
  }
  char* store(char* buff) const {
    memcpy(buff, str, Kn);
    return (buff + Kn);
  }
#ifdef USE_DUMP
  char* fromStr(char* bf) {
    CRegExp mi("^\"([^\"]*)\"([;,]? |;)",0x2);
    if(!mi.exe(bf)) {
      char* b = mi(0x1);
      *(mi.end(0x1)) = '\0';
      strncpy(str,b,Kn);
      return mi.end(0x0);
    }
    return bf;
  }
  char* dump(char* bf,uInt1 of=0u) const {
    *(bf++) = '"';
    char* p = (char*)memccpy(bf, str, 0, Kn);
    bf = p ? --p : (bf + Kn); *(bf++) = '"';
    return bf;
  }
  char* schema(char* bf,uInt1 of=0u) const {
    strcpy(bf, "\"FString\""); bf += 9;
    return bf;
  }
#endif // USE_DUMP
};

//===========================================================

template<sInt2 Kn> class TBitMap;

// Bitmap iterator
template<sInt2 Kn> class TBitMapIt {
  /* disable copy constructor */
  TBitMapIt(const TBitMapIt&);
  void operator = (const TBitMapIt&);

 protected:
  TBitMap<Kn>& B;
  uInt2  ind;
  uInt1  map;

  bool   get()  { return (B.bm[ind/8] & map); }
 public:
  TBitMapIt(TBitMap<Kn>& m): B(m),ind(0u),map(0x01) {}
  ~TBitMapIt() {}

  bool  operator () () { return get(); }

  void   set() { B.bm[ind/8] |=  map; }
  void  uset() { B.bm[ind/8] &= ~map; }

  uInt2  pos()  { return ind; }
  void  sPos(uInt2 k) { uInt1 i = k % 8;  ind = k; map = 0x01 << i; }
  void top() { ind = 0x0; map = 0x01; }
  bool end() { return B.end(ind); }
  bool nxt() { ++ind; map  = (map == 0x80) ? 0x01 : (map << 1); return (ind < Kn); }
};


#define bs(x) (1+(x-1)/8)

template<sInt2 Kn> class TBitMap {
  /* disable copy constructor */
  TBitMap(const TBitMap&);
  void operator = (const TBitMap&);

  friend class TBitMapIt<Kn>;

 protected:
  uInt1 bm[ bs(Kn) ];

  bool get(uInt2 i) {
    uInt1 i0 = i/8;
    if(bm[i0]) {
      uInt1 i1 = i%8;
      i1 = 0x01 << i1;
      return (bm[i0] & i1);
    }
    return false;
  }
 public:
  TBitMap()            { memset(bm, 0, bs(Kn)); }
  TBitMap(char*& buff) { buff = reStore(buff); }
  TBitMap(uInt1 x[bs(Kn)]) { for(uInt2 k=0u; k<bs(Kn); ++k) bm[k] = x[k]; }
  ~TBitMap() {}

  bool operator () (uInt2 i) { return get(i); }
  bool end(uInt2 i) { return (i == Kn); }
  void reset()      { memset(bm, 0, bs(Kn)); }

  void set(uInt2 i) {
    uInt1 i0 = i/8;
    uInt1 i1 = i%8;
    i1      = (0x01 << i1);
    bm[i0] |=  i1;
  }
  void uset(uInt2 i) {
    uInt1 i0 = i/8;
    uInt1 i1 = i%8;
    i1      = ~(0x01 << i1);
    bm[i0] &=   i1;
  }

  char* reStore(char* buff) {
    memcpy(bm, buff, sizeof(bm));
    return (buff + sizeof(bm));
  }
  char* store(char* buff) const {
    memcpy(buff, bm, sizeof(bm));
    return (buff + sizeof(bm));
  }
#ifdef USE_DUMP
  char* fromStr(char* bf) {
    CRegExp mi("^BM[0-9]+_([0-9A-Fa-f]+)([;,]? |;)",0x2);
    if(!mi.exe(bf)) {
      sInt2 k=0u;
      char pp[4] = {'0','x','0','\0'};
      bf = mi(0x1); *(mi.end(0x1)) = '\0';
      while(bf[k]) {
	pp[2] = bf[k];
	bm[k>>1] |= (k & 0x01) ? strtoul(pp,NULL,16) : 0x10 * strtoul(pp,NULL,16);
	if(++k > 2*bs(Kn)) return mi.end(0x0);
      }
    }
    return mi.end(0x0);
  }
  char* dump(char* bf,uInt1 of=0u) const {
    uInt1 a, b;
    bf += sprintf(bf,"BM%d_",Kn);
    for(uInt1 k=0u; k<bs(Kn); ++k) {
      a = bm[k]>>4;  b = 0x0F & bm[k];
      *(bf++) = hhx[a]; *(bf++) = hhx[b];
    }
    return bf;
  }
  char* schema(char* bf,uInt1 of=0u) const {
    bf += sprintf(bf,"BM%d_XXXX",Kn);
    return bf;
  }
#endif // USE_DUMP
};

//===========================================================

template<typename T,uInt2 Kn> class TArray;

// TArray iterator
template<typename T,uInt2 Kn> class TArrayIt {
  /* disable copy constructor */
  TArrayIt(const TArrayIt&);
  void operator = (const TArrayIt&);

 protected:
  TArray<T,Kn>& Arr;
  TBitMapIt<Kn> bIt;

 public:
  TArrayIt(TArray<T,Kn>& a): Arr(a),bIt(Arr.bm) {}
  ~TArrayIt() {}

  void   bSet() { bIt.set(); }
  void  buSet() { bIt.uset(); }
  bool   bGet() { return bIt(); }
  uInt2   pos() { return bIt.pos(); }
  void   sPos(uInt2 k) { bIt.sPos(k); }
  void    top() { bIt.top(); }
  bool    end() { return bIt.end(); }
  bool    nxt() { return bIt.nxt(); }

  T & get()          { return Arr.ar[bIt.pos()]; }
  T & operator () () { return get(); }
};


template<typename T,uInt2 Kn> class TArray {
  /* disable copy constructor */
  TArray(const TArray&);
  void operator = (const TArray&);

  friend class TArrayIt<T,Kn>;

 protected:
  T           ar[Kn];
  TBitMap<Kn> bm;  // BitMap
 public:
  TArray() {}
  TArray(char*& bf) { bf = reStore(bf); }
  ~TArray() {}

  T&    get(uInt2 k)      { return      ar[k]; }
  bool bGet(uInt2 k)      { return      bm.get(k); }
  void  set(uInt2 k,T& v) { ar[k] = v;  bm.set(k); }
  void uset(uInt2 k)      { bm.uset(k); }

  char* reStore(char* bf) {
    bf = bm.reStore(bf);
    TArrayIt<T,Kn> it(*this);
    do {
      if(it.bGet()) {
	bf = ar[it.pos()].reStore(bf);
      }
    } while( it.nxt() );
    return bf;
  }
  char* store(char* bf) {
    TArrayIt<T,Kn> it(*this);
    bf = bm.store(bf);
    do {
      if(it.bGet()) {
	bf = ar[it.pos()].store(bf);
      }
    } while( it.nxt() );
    return bf;
  }
#ifdef USE_DUMP
  char* fromStr(char* bf) {
    CRegExp mi("^AR[0-9]+ \\[ ",0x1);
    if(!mi.exe(bf)) {
      CRegExp me("^]([;,]? |;)",0x1);
      CRegExp mAEl("^([0-9]+)=>",0x2);
      bm.reset();
      bf = mi.end(0x0);
      for(uInt2 k=0u; k<Kn; ++k) {
	if(!mAEl.exe(bf)) {
	  *(mAEl.end(0x1)) = '\0';
	  uInt2 ind = (uInt2)atoi(mAEl(0x1));
	  bf = ar[ind].fromStr(mAEl.end(0x0)); bm.set(ind);
	  if(!me.exe(bf))
	    return me.end(0x0);
	} else // Error
	  return NULL;
      }
    }
    return NULL;
  }
  char* dump(char* bf,uInt1 of=0u) {
    sInt1 x = '\0';
    TArrayIt<T,Kn> it(*this);
    bf += sprintf(bf,"AR%d [\n",Kn);
    of += 2; strcpy(bf, _prnDmpOffSet); bf += of;
    do {
      if(it.bGet()) {
	x = '\2';
	bf += ::sprintf(bf, "%d=>",it.pos()); 
	bf = ar[it.pos()].dump(bf,of);
	*(bf++) = ','; *(bf++) = ' ';
      }
    } while( it.nxt() );
    bf -= x; *(bf++) = '\n';
    of -= 2; strcpy(bf, _prnDmpOffSet); bf += of;
    strcpy(bf++,"]");
    return bf;
  }
  char* schema(char* bf,uInt1 of=0u) {
    sInt1 x = '\0'; uInt1 m = '\0';
    bf += sprintf(bf,"AR%d [\n",Kn);
    of += 2; strcpy(bf, _prnDmpOffSet); bf += of;
    do {
      x = '\2';
      if(m > 2) break;
      bf += ::sprintf(bf, "%d=>",m); 
      bf = ar[m].schema(bf,of);
      *(bf++) = ','; *(bf++) = ' ';
    } while( ++m < 2 );
    bf -= x;   *(bf++) = '\n';   of -= 2;
    strcpy(bf, _prnDmpOffSet);  bf += of;
    strcpy(bf++,"]");
    return bf;
  }
#endif // USE_DUMP
};

//===========================================================

// Fix size vector (All el. set)
template<typename T,uInt2 Kn> class TFVector {
  // disable copy constructor.
  TFVector(const TFVector&);
  void operator = (const TFVector&);

 protected:
  T vctr[Kn];
 public:
  TFVector() {}
  TFVector(char*& bf) { bf = reStore(bf); }
  ~TFVector() {}

  T & get(uInt2 k) { return vctr[k]; }

  char* reStore(char* bf) {
    for(uInt2 k = 0u; k<Kn; ++k)
      bf = vctr[k].reStore(bf);
    return bf;
  }
  char* store(char* bf) {
    for(uInt2 k = 0u; k<Kn; ++k)
      bf = vctr[k].store(bf);
    return bf;
  }
#ifdef USE_DUMP
  char* fromStr(char* bf) {
    CRegExp mi("^FV[0-9]+ \\[ ",0x1);
    if(!mi.exe(bf)) {
      CRegExp me("^]([;,]? |;)",0x1);
      bf = mi.end(0x0);
      for(uInt2 k=0u; k<Kn; ++k) {
	bf = vctr[k].fromStr(bf);
	if(!me.exe(bf))
	  return me.end(0x0);
      }
    }
    return NULL;
  }
  char* dump(char* bf,uInt1 of=0u) {
    bf += sprintf(bf,"FV%d [\n",Kn);
    of += 2; strcpy(bf, _prnDmpOffSet); bf += of;
    for(uInt2 k=0u; k<Kn; ++k) {
      bf = vctr[k].dump(bf,of);
      *(bf++) = ','; *(bf++) = ' ';
    }
    bf -= 2;
    *(bf++) = '\n';
    of -= 2; strcpy(bf, _prnDmpOffSet); bf += of;
    strcpy(bf++,"]");
    return bf;
  }
  char* schema(char* bf,uInt1 of=0u) {
    bf += sprintf(bf,"FV%d [\n",Kn);
    of += 2; strcpy(bf, _prnDmpOffSet); bf += of;
    for(uInt2 k=0u; k<2; ++k) {
      bf = vctr[k].schema(bf,of);
      *(bf++) = ','; *(bf++) = ' ';
    }
    bf -= 2;
    *(bf++) = '\n';
    of -= 2; strcpy(bf, _prnDmpOffSet); bf += of;
    strcpy(bf++,"]");
    return bf;
  }
#endif // USE_DUMP
};

//===========================================================

// Variable size vector (All el. set)
template<typename T> class TVector {
  // disable copy constructor.
  TVector(const TVector&);
  void operator = (const TVector&);

 protected:
  uInt2 vs;
  T*    pT;
 public:
  TVector(uInt2 s=0): vs(s), pT(s ? new T[s] : NULL) {}
  TVector(char*& bf) { bf = reStore(bf); }
  ~TVector() { delete [] pT; }

  void set(uInt2 s) {
    delete [] pT;
    pT = new T[s];
  }
  T&   get(uInt2 k) { return pT[k]; }

  char* reStore(char* bf) {
    CFieldUI2 ui2;
    bf = ui2.reStore(bf);
    vs = ui2();
    delete [] pT;
    pT = vs ? new T[vs] : NULL;
    for(uInt2 k = 0u; k<vs; ++k)
      bf = pT[k].reStore(bf);
    return bf;
  }
  char* store(char* bf) {
    CFieldUI2 ui2;
    ui2.set(vs);
    bf = ui2.store(bf);
    for(uInt2 k = 0u; k<vs; ++k)
      bf = pT[k].store(bf);
    return bf;
  }
#ifdef USE_DUMP
  char* fromStr(char* bf) {
    CRegExp mi("^V([0-9]+) \\[ ",0x2);
    if(!mi.exe(bf)) {
      CRegExp me("^]([;,]? |;)",0x1);
      bf = mi.end(0x0);
      *(mi.end(0x1)) = '\0';
      vs = atoi(mi(0x1));
      delete [] pT;
      pT = vs ? new T[vs] : NULL;
      for(uInt2 k=0u; k<vs; ++k) {
	bf = pT[k].fromStr(bf);
	if(!me.exe(bf))
	  return me.end(0x0);
      }
    }
    return NULL;
  }
  char* dump(char* bf,uInt1 of=0u) {
    bf += sprintf(bf,"V%u [\n",vs);
    of += 2; strcpy(bf, _prnDmpOffSet); bf += of;
    for(uInt2 k=0u; k<vs; ++k) {
      bf = pT[k].dump(bf,of);
      *(bf++) = ','; *(bf++) = ' ';
    }
    bf -= 2;
    *(bf++) = '\n';
    of -= 2; strcpy(bf, _prnDmpOffSet); bf += of;
    strcpy(bf++,"]");
    return bf;
  }
  char* schema(char* bf,uInt1 of=0u) {
    T* pp = pT ? pT : new T[2];
    bf += sprintf(bf,"V%u [\n",vs);
    of += 2; strcpy(bf, _prnDmpOffSet); bf += of;
    for(uInt2 k=0u; k<2; ++k) {
      bf = pp[k].schema(bf,of);
      *(bf++) = ','; *(bf++) = ' ';
    }
    delete [] pp;
    bf -= 2;  *(bf++) = '\n';
    of -= 2; strcpy(bf, _prnDmpOffSet); bf += of;
    strcpy(bf++,"]");
    return bf;
  }
#endif // USE_DUMP
};

//===========================================================

// Virtual base class for all message factories
//   MsgFactory has it own list of construted messages
class CMessage;

class CMsgFactory {
  // disable copy constructor.
  CMsgFactory(const CMsgFactory&);
  void operator = (const CMsgFactory&);

 protected:
  //CFieldUI2 srcId;  // Message source ID
  //CFieldUI2 sqwNm;  // Message sequence number
  CFieldUI2 mLen;   // Message length
  CFieldUI2 msgId;  // Message type id

 public:
  CMsgFactory() {}
  virtual ~CMsgFactory() {}

  // return - bf. pointer to the next message
  virtual CMessage* reStore(char*& bf) = 0;
};

//===========================================================

// Virtual base class for all messages
class CMessage {
  // disable copy constructor.
  CMessage(const CMessage&);
  void operator = (const CMessage&);

 protected:
  char*     mBuff;
  //uInt4   bHndl;  // Handle to release buffer
  CFieldUI2 msgId;  // Message type id

  // signal to release mBuff pointer - reuse buffer
  void relsBuff() { /* TODO: some code here */ }
 public:
  CMessage(char* p, uInt2 i): mBuff(p), msgId(i) {}
  virtual ~CMessage() {}

  uInt2 getMsgId() const { return msgId.get(); }

  // Parse0 to allow message dispatching
  virtual char parse0() = 0;
  virtual char parse1() = 0;
  virtual char* store(char* bf /* , uInt2 id=0u, uInt2 sqw=0u */) = 0;

#ifdef USE_DUMP
  virtual char* fromStr(char* bf) = 0;
  virtual char* dump(char* bf)    = 0;
  virtual char* schema(char* bf)  = 0;
#endif // USE_DUMP
};
typedef CMessage* PMessage;

}  // namespace MsgModel

#endif // MSG_PARSER_H

// $Id: msgParser.h 348 2010-01-29 21:45:52Z asus $
