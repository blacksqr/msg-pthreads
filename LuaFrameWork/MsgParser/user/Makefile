# Model test

TOPD  = /Xxx/AsEee/LuaFr/MsgParser

CC  = gcc
CXX = g++
MODGEN = $(TOPD)/modelGen.pl
TOLUA  = $(TOPD)/../tolua-5.1b

CFLAGS = -O2 -pipe -Wall -pthread -mno-align-double -DUSE_DUMP
#CFLAGS = -g  -pipe -Wall -pthread -mno-align-double -DUSE_DUMP

INC = -I$(TOPD)/inc -I$(TOPD)/gen -I$(TOPD)/user -I$(TOPD)/pcre-8.01

OBJS  = model_tst.o # msgTst.o msgTstbind.o

PKGFILE = msgTst.pkg

all:	Model mTest

mTest:	$(OBJS) # $(PKGFILE)
	$(CXX) $(CFLAGS) -o $@ $(OBJS) $(TOPD)/src/msgParser.o \
		$(TOPD)/pcre-8.01/.libs/libpcreposix.a \
		$(TOPD)/pcre-8.01/.libs/libpcre.a
#	rm -f *.o *~

Model:
	cd $(TOPD)/src; make

msgTstbind.cpp:	msgTst.pkg
	$(TOLUA)/bin/tolua   -o $@  $<

clean:
	rm -f mTest $(TOPD)/*~ $(TOPD)/*/*~ $(TOPD)/*/*.o $(TOPD)/*/*.a $(TOPD)/gen/*
	cd $(TOPD)/src; make clean

# ========================================================
.c.o:
	$(CC)  -c $(CFLAGS) $(INC) $< -o $@
.cpp.o:
	$(CXX) -c $(CFLAGS) $(INC) $< -o $@
#
