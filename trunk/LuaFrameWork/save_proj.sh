#!/bin/bash
cd /Xxx
tar -cvf - ./LuaFr ./Avr32Fr ./FrWrk ./SVN_FW --exclude=*/*.o --exclude=*/.svn --exclude=*/.snprj --exclude=*/*.a --exclude=*/*.so | bzip2 -c9 > /tmp/frWork_proj.tar.bz2

# $Id: save_proj.sh 301 2010-01-11 12:08:00Z asus $
