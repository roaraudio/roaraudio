#!/bin/sh

# check for bash...

if [ "$(echo -n)" = '-n' ]
then
 SHELL=$(which bash ksh sh 2> /dev/null | grep ^/ | head -n 1)
 exec $SHELL $0 "$@"
fi

#ll
