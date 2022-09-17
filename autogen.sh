#!/bin/sh

aclocal -I .
autoconf
autoheader
automake --add-missing --copy

echo "Now you are ready to run ./configure"

