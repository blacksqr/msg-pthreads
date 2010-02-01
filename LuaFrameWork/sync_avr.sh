#!/bin/bash

#TOP=`pwd`
SRC_DIR=/Xxx/LuaFr
DST_DIR=/Xxx/Avr32Fr

cp -f $SRC_DIR/inc/*.h   $DST_DIR/inc
cp -f $SRC_DIR/src/*.cpp $DST_DIR/src
cp -f $SRC_DIR/src/*.[ch]   $DST_DIR/src
cp -f $SRC_DIR/src/*.pkg $DST_DIR/src
cp -f $SRC_DIR/src/sock/*.[ch] $DST_DIR/src/sock
#cp -f $SRC_DIR/src/*.sh $DST_DIR/src

cp -f $SRC_DIR/src/lua/*.lua $DST_DIR/src/lua
#
# $Id: sync_avr.sh 301 2010-01-11 12:08:00Z asus $
