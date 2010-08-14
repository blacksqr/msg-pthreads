# $Id: Makefile 387 2010-05-15 21:02:11Z asus $
CC  = gcc
CXX = g++
#CC  = /usr/Xxx/IcNova/build_avr32/staging_dir/usr/bin/avr32-linux-uclibc-gcc
#CXX = /usr/Xxx/IcNova/build_avr32/staging_dir/usr/bin/avr32-linux-uclibc-g++
AR  = ar rcu
RANLIB = ranlib

SRC_DIR = .

TOPD    = /Xxx/AsEee/LuaFr
TOPM    = $(TOPD)/MsgParser
LUA     = $(TOPD)/Lua/src	
LIBDIR  = $(TOPD)/Lib
TOLUA   = $(TOPD)/tolua-5.1b
SQL     = $(TOPD)/Sql23

INC =	-I$(TOPM)/inc -I$(TOPM)/gen -I$(TOPM)/pcre-8.02 \
	-I$(SQL) -I$(LUA) -I$(TOLUA)/include \
	-I../inc -I.

CFLAGS = -O2 -pipe -Wall -pthread -mno-align-double -DTOLUA_RELEASE -DUSE_DUMP
#CFLAGS =  -g -pipe -Wall -pthread -mno-align-double -DUSE_DUMP
# -DNO_DEBUG -pg -DBIG_TmQueue -DBIG_FIFO 

PKGFILE   = tAdminLua.pkg tWrkLua.pkg
OBJECTSQL = luasql.o dprint.o
OBJECTS = $(TOPM)/src/msgParser.o \
	tWrkLua.o tWrkLuabind.o tAdminLua.o tAdminLuabind.o      \
	stlObj.o memPool.o pMutex.o getTime.o daemon.o Thread.o  \
	EvObj.o WThread.o Context.o EvThread.o Timer.o fsmCtxt.o \
	tstSigThrd.o tstCntxt.o cashCtxt.o dbCntxt.o dprint.o    \
	main.o # udpSock.o

LIBS =	-lpthread -ldl -lm -L$(LIBDIR) -lsqlite3 \
	-l_lua -lLSqlite3 -ltolua -lpcre -L./sock -lSockLua
# -lRex_pcre

#export ITS_APP_HOME    = /Xxx/FrWrk/src
#export LD_LIBRARY_PATH = /Xxx/FrWrk/Sql/.libs

# ========================================================

all:	its

its:	model SockLib $(OBJECTS) $(PKGFILE)
	$(CXX) $(CFLAGS) -o $@ $(OBJECTS) ${LIBS}
#	rm -f *.o *~

model:
	cd $(TOPM)/src; make

SockLib:
	cd ./sock; make

tWrkLuabind.cpp:   tWrkLua.pkg
	$(TOLUA)/bin/tolua   -o $@  $<
tAdminLuabind.cpp: tAdminLua.pkg
	$(TOLUA)/bin/tolua   -o $@  $<

clean:
	rm -f /tmp/core *.o *~ ./lua/*~ ../inc/*~ t*Luabind.* its tLua
	cd $(TOPM)/src; make clean
	cd sock; make clean

# ========================================================
.c.o:
	$(CC)  -c $(CFLAGS) $(INC) $< -o $@
.cpp.o:
	$(CXX) -c $(CFLAGS) $(INC) -Wno-deprecated $< -o $@
#	$(CXX) -c $(CFLAGS) $(INC) $< -o $@

#
# $Id: Makefile 387 2010-05-15 21:02:11Z asus $
