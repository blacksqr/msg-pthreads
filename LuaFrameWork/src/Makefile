# $Id: Makefile 360 2010-03-27 13:25:05Z asus $
CC  = gcc
CXX = g++
#CC  = /usr/Xxx/IcNova/build_avr32/staging_dir/usr/bin/avr32-linux-uclibc-gcc
#CXX = /usr/Xxx/IcNova/build_avr32/staging_dir/usr/bin/avr32-linux-uclibc-g++
AR  = ar rcu
RANLIB = ranlib

SRC_DIR = .

TOPD  = /Xxx/AsEee/LuaFr
LUA   = $(TOPD)/Lua/src	
LUAL  = $(TOPD)/Lib
TOLUA = $(TOPD)/tolua-5.1b
#SQL   = $(TOPD)/sqlite-3.6.6.2/src
SQL   = $(TOPD)/Sql

INC = -I$(SQL) -I$(LUA) -I$(TOLUA)/include -I. -I../inc

#CFLAGS = -O2   -pipe -Wall -pthread -mno-align-double -DTOLUA_RELEASE
CFLAGS = -g -O -pipe -Wall -pthread -mno-align-double
# -DNO_DEBUG -pg -DBIG_TmQueue -DBIG_FIFO 

PKGFILE   = tAdminLua.pkg tWrkLua.pkg
OBJECTSQL = luasql.o dprint.o
OBJECTS = \
	tWrkLua.o tWrkLuabind.o \
	tAdminLua.o tAdminLuabind.o \
	stlObj.o memPool.o pMutex.o getTime.o daemon.o Thread.o \
	EvObj.o WThread.o Context.o EvThread.o Timer.o fsmCtxt.o \
	tstSigThrd.o tstCntxt.o cashCtxt.o dbCntxt.o dprint.o \
	main.o libluasqlite3.o # udpSock.o

LIBS =	-lpthread -ldl -lm -L$(LUAL) -lsqlite3 -l_lua \
        -L$(TOLUA)/lib -ltolua -L./sock -lSockLua

#export ITS_APP_HOME    = /Xxx/FrWrk/src
#export LD_LIBRARY_PATH = /Xxx/FrWrk/Sql/.libs

# ========================================================

all:	its

its:	SockLib $(OBJECTS) $(PKGFILE)
	$(CXX) $(CFLAGS) -o $@ $(OBJECTS) ${LIBS}
#	rm -f *.o *~

SockLib:
	cd ./sock; make

tWrkLuabind.cpp:   tWrkLua.pkg
	$(TOLUA)/bin/tolua   -o $@  $<
tAdminLuabind.cpp: tAdminLua.pkg
	$(TOLUA)/bin/tolua   -o $@  $<

clean:
	rm -f /tmp/core *.o *~ ./lua/*~ ../inc/*~ t*Luabind.* its tLua
	cd sock; make clean
# ========================================================
.c.o:
	$(CC)  -c $(CFLAGS) $(INC) $< -o $@
.cpp.o:
	$(CXX) -c $(CFLAGS) $(INC) -Wno-deprecated $< -o $@
#	$(CXX) -c $(CFLAGS) $(INC) $< -o $@

#
# $Id: Makefile 360 2010-03-27 13:25:05Z asus $
