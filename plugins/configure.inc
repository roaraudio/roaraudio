#!/bin/sh

# check for bash...

if [ "$(echo -n)" = '-n' ]
then
 SHELL=$(which bash ksh sh 2> /dev/null | grep ^/ | head -n 1)
 exec $SHELL $0 "$@"
fi


TF_C=test.c
TF=./test

check_cc() {
 echo -n 'testing for C compiler... '
 CC=$(which gcc cc 2> /dev/null | head -n 1)
 if [ -x "$CC" ]
 then
  echo $CC
 else
  echo no.
  on_error;
 fi
}

check_pkg_config() {
 echo -n "checking for pkg-config... "
 if [ "$PKG_CONFIG" = '' ]
 then
  PKG_CONFIG=$(which pkg-config false 2> /dev/null | grep ^/ | head -n 1)
  if $PKG_CONFIG --help > /dev/null 2> /dev/null
  then
   echo $PKG_CONFIG
  else
   PKG_CONFIG=''
   echo no
   on_error;
  fi
 else
   echo $PKG_CONFIG '(forced)'
 fi
}

check_libroar() {
 echo -n 'testing for libroar... '
 if $HAVE_ROAR
 then
  echo 'yes (forced)'
 else
  cat > $TF_C << EOF
#include <roaraudio.h>
int main (void) { return 0; }
EOF

  $CC -o $TF $TF_C -lroar 2> /dev/null
  $TF 2> /dev/null

  if [ "$?" = '0' ]
  then
   echo yes
  else
   echo no.
   on_error
  fi
 fi
}

#ll
