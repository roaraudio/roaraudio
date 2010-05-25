#!/bin/sh

TEST_NAME='roard test'
TEST_DESC='Do tests on RoarAudio'\''s Sound Daemon'

. ./common.sh

c=roard
FORMAT="%-42s : "

printf "$FORMAT" 'Testing for general runnability'
if $EXEC_HELPER $BIN_PATH$c --no-listen -o null < /dev/null > /dev/null 2> /dev/null
then
 _ok
 echo ok
else
 _failed
 echo failed
fi

disp_sum
ret

#ll
