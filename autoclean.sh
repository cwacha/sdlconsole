#!/bin/sh

TMP=`pwd`; cd `dirname $0`; BASEDIR=`pwd`; cd $TMP

rm -rf autom4te.cache
rm -f aclocal.m4
rm -f aclocal.m4~
rm -f compile
rm -f config.guess
rm -f config.h
rm -f config.h.in
rm -f config.h.in~
rm -f config.log
rm -f config.status
rm -f config.sub
rm -f configure
rm -f configure~
rm -f depcomp
rm -f install-sh
rm -f libtool
rm -f ltmain.sh
rm -f Makefile
rm -f Makefile.in
rm -f missing
rm -f stamp-h1
find $BASEDIR -name "*.o" -delete
find $BASEDIR -name "*.lo" -delete
find $BASEDIR -name "*.la" -delete
find $BASEDIR -name "*.a" -delete
rm -f src/Makefile.in
rm -f include/Makefile.in
rm -f example/Makefile.in
rm -f src/Makefile
rm -f include/Makefile
rm -f example/Makefile
rm -rf src/.deps
rm -rf src/.libs
rm -rf example/.deps
rm -rf example/.libs
rm -rf example/autom4te.cache


# cmake clean
rm -f CMakeCache.txt
find $BASEDIR -name CMakeFiles -delete
find $BASEDIR -name cmake_install.cmake -delete

echo "autotools cleaned."

