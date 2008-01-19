#include <stlObj.h>

char CUIntMap::next(iterator& i,pVoid& v) {
  if(i != end()) {
    v = i->second;
    ++i;
    return '\0';
  }
  return 'x';
}

// ====================================================

char CStrIntMap::next(iterator& i,uInt& v) {
  if(i != end()) {
    v = i->second;
    ++i;
    return '\0';
  }
  return 'x';
}

// ====================================================

char CStrPvMap::next(iterator& i,pVoid& v) {
  if(i != end()) {
    v = i->second;
    ++i;
    return '\0';
  }
  return 'x';
}

// ====================================================

char CLLMap::next(iterator& i,lLong& v) {
  if(i == end()) {
    v = i->second;
    ++i;
    return '\0';
  }
  return 'x';
}
