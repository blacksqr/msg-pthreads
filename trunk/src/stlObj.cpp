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
