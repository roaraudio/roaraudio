#!/bin/sh

PWD=`pwd`
export LD_LIBRARY_PATH=$PWD/../lib/

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

disp_sum() {
 SUM=`expr $OK + $FAILED`
 echo
 echo "Done $SUM tests: $OK ok, $FAILED failed."
 echo
}

#ll
