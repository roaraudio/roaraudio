#!/bin/sh

TEST_NAME='Runnable test'
TEST_DESC='Test clients for runnability'

. ./common.sh

CLIENTS="$1"

for c in $CLIENTS;
do
 printf "%-16s : " "$c"

 $EXEC_HELPER $BIN_PATH$c --server '(INVALID)' < /dev/null > /dev/null 2> /dev/null
 R=$?

 if [ "$R" -le 64 ]
 then
  _ok
  echo ok
 else
  _failed
  echo "FAILED! ret=$R"
 fi

done

disp_sum
ret

#ll
