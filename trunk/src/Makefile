# $Id$
CC  = gcc
CXX = g++
SRC_DIR = .

GDEFS   = #-DSOFT_OWNER
ACDEFFS = -DLINUX

INC = -I../inc -I../Tcl/generic -I../Sql

#CFLAGS = -g  -pipe -Wall -pthread $(GDEFS) -mno-align-double -DBIG_FIFO #-DNO_DEBUG -pg
CFLAGS = -O2 -pipe -Wall -pthread $(GDEFS) -mno-align-double -DBIG_FIFO #-DNO_DEBUG -pg

OBJECTSQL = tclsql.o tclAdapter.o md5.o dprint.o
OBJECTS = \
	stlObj.o memPool.o pMutex.o getTime.o daemon.o Thread.o EvObj.o WThread.o \
	tclAdapter.o Context.o Timer.o EvThread.o tstSigThrd.o adminTcl.o workTcl.o \
	tstCntxt.o cashCtxt.o dbCntxt.o dprint.o main.o

LIBS =	-L../Tcl/unix -L../Sql/.libs \
	-lsqlite3 -ltclsqlite3 -ltcl8.4 -lpthread -ldl

# ========================================================
all:	its tclSql

tclSql: $(OBJECTSQL) psip
	$(CXX) $(CFLAGS) -o tclSql $(OBJECTSQL) ${LIBS}

its:	$(OBJECTS) psip lsip
	$(CXX) $(CFLAGS) -o its $(OBJECTS) ${LIBS}

clean:
	rm -f /tmp/core *~ ../inc/*~ *.o ./tcl/*~ tclSql its

# ========================================================
.c.o:
	$(CC) -c $(CFLAGS) $(ACDEFFS) $(INC) $< -o $@
.cpp.o:
	$(CXX) -c $(CFLAGS) $(ACDEFFS) $(INC) $< -o $@
#*.o:	*.h
