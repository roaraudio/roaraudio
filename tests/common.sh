#!/bin/sh

PWD=`pwd`
PF="$PWD/sandbox"
export LD_LIBRARY_PATH="$PF/$PREFIX_LIB"
export BIN_PATH="$PF/$PREFIX_BIN"

OK=0
FAILED=0

if [ "$TEST_NAME" != '' ]
then
 echo "Test Name       : $TEST_NAME"
 if [ "$TEST_DESC" != '' ]
 then
  echo "Test Description: $TEST_DESC"
 fi
 echo '---------------------'
fi

_ok() {
 OK=`expr $OK + 1`
}
_failed() {
 FAILED=`expr $FAILED + 1`
}

disp_sum() {
 SUM=`expr $OK + $FAILED`
 echo
 echo "Done $SUM tests: $OK ok, $FAILED failed."
 echo
}

ret() {
 exit $FAILED
}

#ll
