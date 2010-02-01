# Makefile to generate message model

TOPD  = /Xxx/AsEee/LuaFr/MsgParser

CC= gcc
AR= ar rcu
MODGEN = $(TOPD)/modelGen.pl

#CFLAGS = -O2 -pipe -Wall -pthread -mno-align-double -DUSE_DUMP
CFLAGS = -g  -pipe -Wall -pthread -mno-align-double -DUSE_DUMP

INC = -I$(TOPD)/inc -I$(TOPD)/gen -I$(TOPD)/user -I$(TOPD)/pcre-8.01
OBJS   = msgParser.o
MDLSRC = $(TOPD)/model/model_file1.tcl
#MLIB  = libModel.a

all:	  MODEL $(OBJS)

MODEL:    $(MDLSRC)
	cd $(TOPD); $(MODGEN) $<

clean:
	rm -f $(TOPD)/*~ $(TOPD)/*/*~ $(TOPD)/*/*.o $(TOPD)/*/*.a $(TOPD)/gen/*
# ========================================================
.c.o:
	$(CC)  -c $(CFLAGS) $(INC) $< -o $@
.cpp.o:
	$(CXX) -c $(CFLAGS) $(INC) $< -o $@
#
