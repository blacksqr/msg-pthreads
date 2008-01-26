# $Id$
CC  = gcc
CXX = g++
SRC_DIR = .

GDEFS   = -DHAVE_64BIT_TYPES
ACDEFFS = -DLINUX

INC = -I../inc -I../Tcl/generic -I../Sql -I../BDb

#CFLAGS = -g  -pipe -Wall -pthread $(GDEFS) -mno-align-double -DBIG_FIFO #-DNO_DEBUG -pg
CFLAGS = -O2 -pipe -Wall -pthread $(GDEFS) -mno-align-double -DBIG_FIFO #-DNO_DEBUG -pg

# Berkly DB Files
BDb   = bDb/tcl_compat.o bDb/tcl_db_pkg.o bDb/tcl_lock.o bDb/tcl_rep.o bDb/tcl_util.o \
	bDb/tcl_db.o bDb/tcl_env.o bDb/tcl_log.o bDb/tcl_seq.o bDb/tcl_dbcursor.o \
	bDb/tcl_internal.o bDb/tcl_mp.o bDb/tcl_txn.o

OBJECTSQL = tclsql.o tclAdapter.o dprint.o

OBJECTS = \
	stlObj.o memPool.o pMutex.o getTime.o daemon.o Thread.o EvObj.o WThread.o \
	tclAdapter.o Context.o Timer.o EvThread.o tstSigThrd.o adminTcl.o workTcl.o \
	tstCntxt.o cashCtxt.o dbCntxt.o dprint.o main.o

LIBS =	-L../Tcl/unix -L../Sql/.libs -L../BDb/.libs \
	-ltcl8.4 -lsqlite3 -ltclsqlite3 -lpthread -ldl -ldb-4.6
#-ldb_tcl-4.6

# ========================================================

all:	its tclSql

tclSql: $(BDb) $(OBJECTSQL)
	$(CXX) $(CFLAGS) -o tclSql $(OBJECTSQL) $(BDb) ${LIBS}

its:	$(BDb) $(OBJECTS) 
	$(CXX) $(CFLAGS) -o its $(OBJECTS) $(BDb) ${LIBS}

clean:
	rm -f /tmp/core *~ ../inc/*~ *.o ./bDb/*.o ./bDb/*~ ./tcl/*~ tclSql its

# ========================================================

.c.o:
	$(CC) -c $(CFLAGS) $(ACDEFFS) $(INC) $< -o $@
.cpp.o:
	$(CXX) -c $(CFLAGS) $(ACDEFFS) $(INC) $< -o $@
#*.o:	*.h
