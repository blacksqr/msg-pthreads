#!/bin/bash
TOP=`pwd`
echo "set enviroment ITS_APP_HOME=$TOP"
export ITS_APP_HOME=$TOP/src
export LUA_PATH="$TOP/src/lua/?.lua;$TOP/Lib/?.lua"
export LUA_CPATH="$TOP/Lib/?.so;?.so"
export LD_LIBRARY_PATH=$TOP/Lib

# $Id: set_env.sh 301 2010-01-11 12:08:00Z asus $
