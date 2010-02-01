/*
** Lua binding: msgTst
** Generated automatically by tolua 5.1b on Wed Jan  6 14:56:10 2010.
*/

#ifndef __cplusplus
#include "stdlib.h"
#endif
#include "string.h"

#include "tolua.h"

/* Exported function */
TOLUA_API int tolua_msgTst_open (lua_State* tolua_S);
LUALIB_API int luaopen_msgTst (lua_State* tolua_S);

#include "msgTst.h"

/* function to release collected object */
#ifdef __cplusplus
#else
static int tolua_collect (lua_State* tolua_S)
{
 void* self = tolua_tousertype(tolua_S,1,0);
 free(self);
 return 0;
}
#endif


/* function to register type */
static void tolua_reg_types (lua_State* tolua_S)
{
 tolua_usertype(tolua_S,"CWrkLua");
}

/* function: ansleep */
static int tolua_msgTst_ansleep00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isnumber(tolua_S,1,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  int timeOut = ((int)  tolua_tonumber(tolua_S,1,0));
 {
  ansleep(timeOut);
 }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ansleep'.",&tolua_err);
 return 0;
#endif
}

/* method: getEvent of class  CWrkLua */
static int tolua_msgTst_CWrkLua_getEvent00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CWrkLua",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CWrkLua* self = (CWrkLua*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getEvent'",NULL);
#endif
 {
  int tolua_ret = (int)  self->getEvent();
 tolua_pushnumber(tolua_S,(double)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getEvent'.",&tolua_err);
 return 0;
#endif
}

/* method: ctxtType of class  CWrkLua */
static int tolua_msgTst_CWrkLua_ctxtType00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CWrkLua",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CWrkLua* self = (CWrkLua*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'ctxtType'",NULL);
#endif
 {
  int tolua_ret = (int)  self->ctxtType();
 tolua_pushnumber(tolua_S,(double)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'ctxtType'.",&tolua_err);
 return 0;
#endif
}

/* method: hkCtxtState of class  CWrkLua */
static int tolua_msgTst_CWrkLua_hkCtxtState00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CWrkLua",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CWrkLua* self = (CWrkLua*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'hkCtxtState'",NULL);
#endif
 {
  char* tolua_ret = (char*)  self->hkCtxtState();
 tolua_pushstring(tolua_S,(const char*)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'hkCtxtState'.",&tolua_err);
 return 0;
#endif
}

/* method: sendMsg of class  CWrkLua */
static int tolua_msgTst_CWrkLua_sendMsg00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CWrkLua",0,&tolua_err) ||
 !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
 !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
 !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,5,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CWrkLua* self = (CWrkLua*)  tolua_tousertype(tolua_S,1,0);
  int ev = ((int)  tolua_tonumber(tolua_S,2,0));
  int id = ((int)  tolua_tonumber(tolua_S,3,0));
  int pp = ((int)  tolua_tonumber(tolua_S,4,0));
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'sendMsg'",NULL);
#endif
 {
  self->sendMsg(ev,id,pp);
 }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'sendMsg'.",&tolua_err);
 return 0;
#endif
}

/* method: hkCtxtTm of class  CWrkLua */
static int tolua_msgTst_CWrkLua_hkCtxtTm00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CWrkLua",0,&tolua_err) ||
 !tolua_isnumber(tolua_S,2,0,&tolua_err) ||
 !tolua_isnumber(tolua_S,3,0,&tolua_err) ||
 !tolua_isnumber(tolua_S,4,0,&tolua_err) ||
 !tolua_isnumber(tolua_S,5,0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,6,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CWrkLua* self = (CWrkLua*)  tolua_tousertype(tolua_S,1,0);
  int cid = ((int)  tolua_tonumber(tolua_S,2,0));
  int tm = ((int)  tolua_tonumber(tolua_S,3,0));
  int type = ((int)  tolua_tonumber(tolua_S,4,0));
  int f = ((int)  tolua_tonumber(tolua_S,5,0));
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'hkCtxtTm'",NULL);
#endif
 {
  self->hkCtxtTm(cid,tm,type,f);
 }
 }
 return 0;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'hkCtxtTm'.",&tolua_err);
 return 0;
#endif
}

/* method: test of class  CWrkLua */
static int tolua_msgTst_CWrkLua_test00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CWrkLua",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CWrkLua* self = (CWrkLua*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'test'",NULL);
#endif
 {
  const char* tolua_ret = (const char*)  self->test();
 tolua_pushstring(tolua_S,(const char*)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'test'.",&tolua_err);
 return 0;
#endif
}

/* method: getThId of class  CWrkLua */
static int tolua_msgTst_CWrkLua_getThId00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isusertype(tolua_S,1,"CWrkLua",0,&tolua_err) ||
 !tolua_isnoobj(tolua_S,2,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  CWrkLua* self = (CWrkLua*)  tolua_tousertype(tolua_S,1,0);
#ifndef TOLUA_RELEASE
 if (!self) tolua_error(tolua_S,"invalid 'self' in function 'getThId'",NULL);
#endif
 {
  int tolua_ret = (int)  self->getThId();
 tolua_pushnumber(tolua_S,(double)tolua_ret);
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getThId'.",&tolua_err);
 return 0;
#endif
}

/* function: getWTObj */
static int tolua_msgTst_getWTObj00(lua_State* tolua_S)
{
#ifndef TOLUA_RELEASE
 tolua_Error tolua_err;
 if (
 !tolua_isuserdata(tolua_S,1,0,&tolua_err) ||
 !tolua_isusertype(tolua_S,2,"CWrkLua",1,&tolua_err) ||
 !tolua_isnoobj(tolua_S,3,&tolua_err)
 )
 goto tolua_lerror;
 else
#endif
 {
  void* pp = ((void*)  tolua_touserdata(tolua_S,1,0));
  CWrkLua* px = ((CWrkLua*)  tolua_tousertype(tolua_S,2,0));
 {
  getWTObj(pp,px);
 tolua_pushusertype(tolua_S,(void*)px,"CWrkLua");
 }
 }
 return 1;
#ifndef TOLUA_RELEASE
 tolua_lerror:
 tolua_error(tolua_S,"#ferror in function 'getWTObj'.",&tolua_err);
 return 0;
#endif
}

/* Open lib function */
LUALIB_API int luaopen_msgTst (lua_State* tolua_S)
{
 tolua_open(tolua_S);
 tolua_reg_types(tolua_S);
 tolua_module(tolua_S,NULL,0);
 tolua_beginmodule(tolua_S,NULL);
 tolua_function(tolua_S,"ansleep",tolua_msgTst_ansleep00);
#ifdef __cplusplus
 tolua_cclass(tolua_S,"CWrkLua","CWrkLua","",0);
#else
 tolua_cclass(tolua_S,"CWrkLua","CWrkLua","",tolua_collect);
#endif
 tolua_beginmodule(tolua_S,"CWrkLua");
 tolua_function(tolua_S,"getEvent",tolua_msgTst_CWrkLua_getEvent00);
 tolua_function(tolua_S,"ctxtType",tolua_msgTst_CWrkLua_ctxtType00);
 tolua_function(tolua_S,"hkCtxtState",tolua_msgTst_CWrkLua_hkCtxtState00);
 tolua_function(tolua_S,"sendMsg",tolua_msgTst_CWrkLua_sendMsg00);
 tolua_function(tolua_S,"hkCtxtTm",tolua_msgTst_CWrkLua_hkCtxtTm00);
 tolua_function(tolua_S,"test",tolua_msgTst_CWrkLua_test00);
 tolua_function(tolua_S,"getThId",tolua_msgTst_CWrkLua_getThId00);
 tolua_endmodule(tolua_S);
 tolua_function(tolua_S,"getWTObj",tolua_msgTst_getWTObj00);
 tolua_endmodule(tolua_S);
 return 1;
}
/* Open tolua function */
TOLUA_API int tolua_msgTst_open (lua_State* tolua_S)
{
 lua_pushcfunction(tolua_S, luaopen_msgTst);
 lua_pushstring(tolua_S, "msgTst");
 lua_call(tolua_S, 1, 0);
 return 1;
}
