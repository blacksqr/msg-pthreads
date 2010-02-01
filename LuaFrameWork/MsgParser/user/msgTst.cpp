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

#include <msgTst.h>

void wnsleep(int timeOut) { nsleep(timeOut); }

void CWrkLua::init() {
  lua_pushlightuserdata(pLua, (pVoid)this);
  lua_setfield(pLua, LUA_REGISTRYINDEX, "__IY__");
}

pContext CWrkLua::getCtxt() { return pth->pCont; }

int CWrkLua::getEvent() {
  DBG(">CWrkLua::getEvent\n");
  return (int)(pth->getEvent());
}

int CWrkLua::getThId() { return (short)(pth->getWTId()); }

int CWrkLua::ctxtType() { return pth->pCont->getCtxtType(); }

char* CWrkLua::hkCtxtState() {
  return ((CHKContext*)(pth->pCont))->getStatus();
}

void CWrkLua::sendMsg(int ev, int id, int pp) {
    pContext pc = pth->pCont;
    if(id >= 0) {
      pc->send(ev, id ? id : pc->getId(), (pVoid)pp);
    } else
      pc->send0(ev, -id, (pVoid)pp);
}

extern uInt HkCId;
void CWrkLua::hkCtxtTm(int cid, int tm, int type, int f) {
  pth->setTimer(cid,tm,type,f);
}
