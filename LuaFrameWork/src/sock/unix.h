#ifndef UNIX_H
#define UNIX_H
/*=========================================================================*\
* Unix domain object
* LuaSocket toolkit
*
* This module is just an example of how to extend LuaSocket with a new 
* domain.
*
* RCS ID: $Id: unix.h 297 2010-01-08 20:41:13Z asus $
\*=========================================================================*/
#include "lua.h"

#include "buffer.h"
#include "timeout.h"
#include "socket.h"

typedef struct t_unix_ {
    t_socket sock;
    t_io io;
    t_buffer buf;
    t_timeout tm;
} t_unix;
typedef t_unix *p_unix;

int luaopen_socket_unix(lua_State *L);

#endif /* UNIX_H */
