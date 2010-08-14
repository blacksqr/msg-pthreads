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
#ifndef tWrkLua_h
#define tWrkLua_h

extern "C" {
#include <lualib.h>
#include <lauxlib.h>
}

class CWThread;
class CContext;

void wnsleep(long timeOut);

class CWrkLua {
  friend class CWThread;
 private:
  // Disable copy constructor
  CWrkLua(const CWrkLua&);
  void operator = (const CWrkLua&);
 protected:
  CWThread* pth;
  lua_State* pLua;
  CContext* getCtxt();
 CWrkLua(lua_State* pl,CWThread* p): pth(p),pLua(pl) {}
  ~CWrkLua() {}
  void init();
 public:
  int   getEvent();
  int   ctxtType();
  char* hkCtxtState();
  void  sendMsg(int ev, int id, int pp);
  void  hkCtxtTm(int cid, int tm, int type, int f);
  lua_State* operator & () { return pLua; }
  const char* test()       { return "Test CWrkLua"; }
  int getThId();
};
inline void getWTObj(void* pp, CWrkLua*& px) { px = (CWrkLua*)pp; }

#endif // tWrkLua_h

// $Id: tWrkLua.h 375 2010-05-09 19:07:48Z asus $
