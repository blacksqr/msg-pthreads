
CC= gcc
AR= ar rcu

TOPD  = /Xxx/AsEee/LuaFr
CFLAGS = -O2 -pipe -Wall -pthread -mno-align-double
#CFLAGS = -g -pipe -Wall -pthread -mno-align-double
INC = -I. -I$(TOPD)/Lua/src

SOCK_LIB = libSockLua.a

# Modules belonging to socket-core
SOCKET_OBJS = \
	luasocket.o timeout.o buffer.o \
	auxiliar.o options.o except.o  \
	select.o usocket.o inet.o io.o \
	tcp.o udp.o

all:	$(SOCK_LIB) unix.o mime.o

$(SOCK_LIB):	$(SOCKET_OBJS)
	$(AR)     $@ $?
#	rm -f $(SOCKET_OBJS)

clean:
	rm -f *.o *.a *~

# ========================================================

.c.o:
	$(CC)  -c $(CFLAGS) $(INC) $< -o $@
