# makefile for tolua executable

TOLUA= /Xxx/LuaFr/tolua-5.1b
include $(TOLUA)/config
LIBS = -ltolua -l_lua
EXE = tmodule tnamespace tclass tinheritance tconstant tvariable tfunction tarray tdirective

all:    $(EXE) 
	./tmodule;./tnamespace;./tclass;./tinheritance;./tconstant;./tvariable;./tfunction; \
	./tarray;./tdirective;./tfunction

tarray: tarray.o tarraybind.o
	$(CC) $(CFLAGS) -o $@ tarray.o tarraybind.o $(TLIB) $(LIB) $(LIBS)
tarraybind.c: tarray.pkg
	$(TOLUA)/bin/tolua -o $@ $<

tvariable: tvariable.o tvariablebind.o
	$(CC) $(CFLAGS) -o $@ tvariable.o tvariablebind.o $(TLIB) $(LIB) $(LIBS)
tvariablebind.c: tvariable.pkg
	$(TOLUA)/bin/tolua -o $@ $<

tmodule: tmodule.o tmodulebind.o
	$(CC) $(CFLAGS) -o $@ tmodule.o tmodulebind.o $(TLIB) $(LIB) $(LIBS)
tmodulebind.c: tmodule.pkg
	$(TOLUA)/bin/tolua -o $@ $<

tnamespace: tnamespace.o tnamespacebind.o
	$(CCPP) $(CFLAGS) -o $@ tnamespace.o tnamespacebind.o $(TLIB) $(LIB) $(LIBS)
tnamespacebind.cpp: tnamespace.pkg
	$(TOLUA)/bin/tolua -o $@ $<

tdirective: tdirectivebind.o
	$(CC) $(CFLAGS) -o $@ tdirectivebind.o $(TLIB) $(LIB) $(LIBS)
tdirectivebind.c: tdirective.pkg
	$(TOLUA)/bin/tolua -o $@ $<

tfunction: tfunction.o tfunctionbind.o
	$(CCPP) $(CPPFLAGS) -o $@ tfunction.o tfunctionbind.o $(TLIB) $(LIB) $(LIBS)
tfunctionbind.cpp: tfunction.pkg
	$(TOLUA)/bin/tolua -o $@ $<

tconstant: tconstant.o tconstantbind.o
	$(CCPP) $(CPPFLAGS) -o $@ tconstant.o tconstantbind.o $(TLIB) $(LIB) $(LIBS)
tconstantbind.cpp: tconstant.pkg
	$(TOLUA)/bin/tolua -o $@ $<

tclass:  tclass.o tclassbind.o
	$(CCPP) $(CPPFLAGS) -o $@ tclass.o tclassbind.o $(TLIB) $(LIB) $(LIBS)
tclassbind.cpp: tclass.pkg
	$(TOLUA)/bin/tolua -o $@ $<

tinheritance:  tinheritance.o tinheritancebind.o
	$(CCPP) $(CPPFLAGS) -o $@ tinheritance.o tinheritancebind.o $(TLIB) $(LIB) $(LIBS)
tinheritancebind.cpp: tinheritance.pkg
	$(TOLUA)/bin/tolua -o $@ $<

.cpp.o:
	$(CCPP) -c $(CPPFLAGS) -o $@ $<

.pkg.cpp:
	$(TOLUA)/bin/tolua -o $@ $<

clean:
	rm -f t*bind.* *.o $(EXE) *~



