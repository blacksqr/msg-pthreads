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
#ifndef CSTLOBJ_H
#define CSTLOBJ_H

#include <dprint.h>
#include <Thread.h>
#include <ext/hash_map>

typedef void* pVoid;

namespace __gnu_cxx {
  template<> struct hash<lLong> {
    size_t operator()(lLong __x) const { return (0xFFFFFFFF & __x); }
  };
}

struct eqUInt {
  bool operator()(const uInt s1, const uInt s2) const { return (s1 == s2); }
};
struct eqLLong {
  bool operator()(const lLong s1, const lLong s2) const { return (s1 == s2); }
};
struct eqstr {
  bool operator()(const char* s1, const char* s2) const { return (strcmp(s1, s2) == 0); }
};

class CUIntMap: public __gnu_cxx::hash_map < uInt,pVoid,__gnu_cxx::hash<uInt>,eqUInt > {
 public:
  CUIntMap() {}
  virtual ~CUIntMap() {}
  pVoid getv(const uInt n) {
    iterator i = find(n);
    return (i==end()) ? NULL : (i->second);
  }
  void del(const uInt n) {
    DBG("Class CUIntMap.erase[ %d ]\n",n);
    erase(n);
  }
  // to traverse all items
  void top(iterator& i) { i = begin(); }
  char next(iterator& i,pVoid& v);
};

class CStrIntMap: public __gnu_cxx::hash_map < const char*,uInt,__gnu_cxx::hash<const char*>,eqstr > {
 public:
  CStrIntMap() {}
  virtual ~CStrIntMap() {}
  uInt getv(const char* n) {
    iterator i = find(n);
    return (i==end()) ? 0 : (i->second);
  }
  void del(const char* n) { erase(n); }
  // to traverse all items
  void top(iterator& i) { i = begin(); }
  char next(iterator& i,uInt& v);
};

class CStrPvMap: public __gnu_cxx::hash_map < const char*,pVoid,__gnu_cxx::hash<const char*>,eqstr > {
 public:
  CStrPvMap() {}
  virtual ~CStrPvMap() {}
  pVoid getv(const char* n) {
    DBG("Class CStrPvMap.erase[ %s ]\n",n);
    iterator i = find(n);
    return (i==end()) ? NULL : (i->second);
  }
  void del(const char* n) { erase(n); }
  // to traverse all items
  void top(iterator& i) {i = begin(); } 
  char next(iterator& i,pVoid& v);
};

class CLLMap: public __gnu_cxx::hash_map < lLong,lLong,__gnu_cxx::hash<lLong>,eqLLong > {
 public:
  CLLMap() {}
  virtual ~CLLMap() {}
  lLong getv(const lLong n) {
    DBG("Class CLLMap.erase[%lld]\n",n);
    iterator i = find(n);
    return (i==end()) ? 0ll : (i->second);
  }
  void del(const lLong n) { erase(n); }
  // to traverse all items
  void top(iterator& i) {i = begin(); } 
  char next(iterator& i,lLong& v);
};

#endif // CSTLOBJ_H
