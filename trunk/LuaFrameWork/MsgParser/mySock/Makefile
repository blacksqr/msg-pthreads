# $Id: Makefile 323 2010-01-17 20:29:38Z asus $

CC  = gcc
CXX = g++
AR  = ar rcu

SRC_DIR  = .
TOPD     = /Xxx/AsEee/LuaFr/MsgParser/mySock
LIBS     = -L$(TOPD) -lpthread -ldl -lm
SOCK_LIB = libmysock.a

#CFLAGS = -O2 -pipe -Wall -pthread -mno-align-double -DHAVE_CONFIG_H
CFLAGS = -g -pipe -Wall -pthread -mno-align-double -DHAVE_CONFIG_H
INC = -I$(TOPD)

# Modules belonging to socket-core
SOCKET_OBJS = skpoll.o skserver.o skstream.o

all:	$(SOCK_LIB) ping

$(SOCK_LIB):	$(SOCKET_OBJS)
	$(AR)     $@ $?
	rm -f $(SOCKET_OBJS)

# Test programm
ping:	ping.o
	$(CXX) $(CFLAGS) -o $@ ping.o ${LIBS} -lmysock
	rm -f *.o *~

clean:
	rm -f *.o *.a *~ ping

# ========================================================
.c.o:
	$(CC)  -c $(CFLAGS) $(INC) $< -o $@
.cpp.o:
	$(CXX) -c $(CFLAGS) $(INC) $< -o $@
#
# $Id: Makefile 323 2010-01-17 20:29:38Z asus $
