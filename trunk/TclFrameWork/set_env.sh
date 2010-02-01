#!/bin/bash
ITS_APP_HOME=`pwd`
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$ITS_APP_HOME/Tcl/unix:$ITS_APP_HOME/Sql/.libs:$ITS_APP_HOME/BDb/.libs
export ITS_APP_HOME=$ITS_APP_HOME/src
